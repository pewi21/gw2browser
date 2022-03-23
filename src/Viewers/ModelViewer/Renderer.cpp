/** \file       Viewers/Renderer.cpp
 *  \brief      Contains the definition of the model renderer class.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2018-2019 Khralkatorrix <https://github.com/kytulendu>
 *
 * This file is part of Gw2Browser.
 *
 * Gw2Browser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

#include "Data.h"
#include "Exception.h"

#include "Renderer.h"

namespace gw2b {

    //----------------------------------------------------------------------------
    //      Renderer
    //----------------------------------------------------------------------------

    Renderer::Renderer() {

    }

    Renderer::~Renderer() {
        // Force GPU finishing before the context is deleted
        glFinish();
    }

    void Renderer::clear() {
        if( m_IsModelLoaded ) {
            m_model.clear();
            m_texture.clear();
            m_IsModelLoaded = false;
        }
    }

    void Renderer::setup() {
        // Set background color
        glClearColor( 0.21f, 0.21f, 0.21f, 1.0f );

        // Accept fragment if it closer to the camera than the former one
        glDepthFunc( GL_LESS );

        if ( !this->loadShader( ) ) {
            this->clearShader( );
        }

        // Initialize text renderer stuff
        try {
            m_text = std::unique_ptr<Text2D>( new Text2D( ) );
        } catch ( const exception::Exception& err ) {
            wxLogMessage( wxT( "m_text : %s" ), wxString( err.what( ) ) );
            throw exception::Exception( "Failed to initialize text renderer." );
        }

        // Initialize lightbox renderer
        try {
            m_lightBox = std::unique_ptr<LightBox>( new LightBox( ) );
        } catch ( const exception::Exception& err ) {
            wxLogMessage( wxT( "m_lightBox : %s" ), wxString( err.what( ) ) );
            throw exception::Exception( "Failed to initialize lightbox renderer." );
        }

        // Setup camera
        m_camera.setCameraMode( Camera::CameraMode::ORBITALCAM );
        m_camera.setMouseSensitivity( 0.5f * ( glm::pi<float>( ) / 180.0f ) );  // 0.5 degrees per pixel

        // Create framebuffer
        this->createFrameBuffer( );

        // Reset fps counter
        m_fpsStartTime = Time::now( );
    }

    const GLubyte* Renderer::getGLVersion() {
        return glGetString(GL_VERSION);
    }

    const GLubyte* Renderer::getGLVendor() {
        return glGetString(GL_VENDOR);
    }

    const GLubyte* Renderer::getGLRenderer() {
        return glGetString(GL_RENDERER);
    }

    void Renderer::setViewport(int p_x, int p_y, wxSize p_clientSize) {
        int width = p_clientSize.x;
        int height = p_clientSize.y;

        m_clientSize = p_clientSize;

        // Check if framebuffer is already create. If found, delete it.
        if ( m_framebuffer ) {
            m_framebuffer.reset( );
        }

        this->createFrameBuffer();

        if (width < 1) width = 1;
        if (height < 1) height = 1;

        glViewport(p_x, p_y, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

    }

    bool Renderer::loadShader()
    {
        if (!m_shaders.add("main", getPath("shaders/shader.vert").mb_str(), getPath("shaders/shader.frag").mb_str()))
        {
            return false;
        }
        if (!m_shaders.add("framebuffer", getPath("shaders/framebuffer.vert").mb_str(), getPath("shaders/framebuffer.frag").mb_str()))
        {
            return false;
        }
        if (!m_shaders.add("normal_visualizer", getPath("shaders/normal_visualizer.vert").mb_str(), getPath("shaders/normal_visualizer.frag").mb_str(), getPath("shaders/normal_visualizer.geom").mb_str()))
        {
            return false;
        }
        if (!m_shaders.add("z_visualizer", getPath("shaders/z_visualizer.vert").mb_str(), getPath("shaders/z_visualizer.frag").mb_str()))
        {
            return false;
        }
        this->initShaderValue();

        return true;
    }

    void Renderer::clearShader() {
        m_shaders.clear( );
    }

    void Renderer::initShaderValue() {
        if ( !m_shaders.empty( ) ) {
            m_shaders.get( "main" )->use( );
            m_shaders.get( "main" )->setTexture( "material.diffuseMap", 0 );    // Set our "diffuseMap" sampler to user Texture Unit 0
            m_shaders.get( "main" )->setTexture( "material.normalMap", 1 );     // Set our "normalMap" sampler to user Texture Unit 1
            m_shaders.get( "main" )->setTexture( "material.lightMap", 2 );      // Set our "lightMap" sampler to user Texture Unit 2

            m_shaders.get( "framebuffer" )->use( );
            m_shaders.get( "framebuffer" )->setFloat( "exposure", 6.0f );
            m_shaders.get( "framebuffer" )->setFloat( "whitePoint", 11.2f );
            m_shaders.get( "framebuffer" )->setTexture( "screenTexture", 0 );
        }
    }

    void Renderer::reloadShader() {
        this->clearShader( );
        if ( !this->loadShader( ) ) {
            this->clearShader( );
            wxLogMessage( wxT( "SHADER ERROR!!! FIX IT AND PRESS = KEY TO RELOAD SHADER!" ) );
        }
    }

    void Renderer::createFrameBuffer() {
        if ( m_statusAntiAlising ) {
            m_framebuffer = std::unique_ptr<FrameBuffer>( new FrameBuffer( m_clientSize, 4, 1 ) );
        } else {
            m_framebuffer = std::unique_ptr<FrameBuffer>( new FrameBuffer( m_clientSize, 1 ) );
        }
    }

    bool Renderer::isLightmapExcluded(const uint32& p_id) {
        switch ( p_id ) {
            // Zhaitan (454456) model
        case 454385:
        case 454421:
            return true;
        }
        return false;
    }

    void Renderer::loadModel(DatFile& p_datFile, const GW2Model& p_model) {
        auto& material = p_model.material( );

        // load model to m_model
        m_model.push_back( std::unique_ptr<Model>( new Model( p_model ) ) );

        // load texture into texture manager
        for ( auto& mat : material ) {
            // Load diffuse texture
            if ( mat.diffuseMap ) {
                m_texture.load( p_datFile, mat.diffuseMap );
            }
            // Load normal map texture
            if ( mat.normalMap ) {
                m_texture.load( p_datFile, mat.normalMap );
            }
            // Load light map texture
            if ( mat.lightMap ) {
                // check for excluded lightmap texture
                if ( !this->isLightmapExcluded( mat.lightMap ) ) {
                    m_texture.load( p_datFile, mat.lightMap );
                }
            }
        }
        m_IsModelLoaded = true;
    }

    void Renderer::render() {
        // Get current time
        auto currentTime = Time::now( );
        // Get time elapsed
        m_deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>( currentTime - m_oldstartTime ).count( );
        m_oldstartTime = currentTime;

        // Check if shader is properly loaded
        if ( m_shaders.empty( ) ) {
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            // Send ClientSize variable to text renderer
            m_text->setClientSize( m_clientSize );
            // Display error message
            m_text->drawText( wxString::Format( wxT( "SHADER ERROR!!! PRESS = KEY TO RELOAD SHADER!" ) ), 0.0f, m_clientSize.y - 12.0f, 1.0f, glm::vec3( 1.0f ) );

            return;
        }

        // ------------------------------------------------
        // Render the scene into m_framebuffer frame buffer.

        // Bind to framebuffer and draw to framebuffer texture
        m_framebuffer->bind( );

        // Enable depth test
        glEnable( GL_DEPTH_TEST );

        // Clear color buffer and depth buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        this->updateMatrices( );

        if ( !m_model.empty( ) ) {
            // Set light position at camera position
            m_light.setPosition( m_camera.position( ) );
        }

        // No need since already initilized with this value
        //m_light.setAmbient( glm::vec3( 0.5f, 0.5f, 0.5f ) );
        //m_light.setDiffuse( glm::vec3( 0.5f, 0.5f, 0.5f ) );
        //m_light.setSpecular( glm::vec3( 0.5f, 0.5f, 0.5f ) );

        // Transformation matrix
        glm::mat4 trans(1.0f);
        // Model position
        trans = glm::translate( trans, glm::vec3( 0.0f, 0.0f, 0.0f ) );
        // Model rotation
        //trans = glm::rotate( trans, angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );
        //angle = angle + ( 0.5f * deltaTime );
        // Model scale
        //trans = glm::scale( trans, glm::vec3( 0.5f ) );

        if ( !m_statusVisualizeZbuffer ) {
            // Draw normally
            this->drawModel( m_shaders.get( "main" ), trans );
            // Draw normal lines for debugging/visualization
            if ( m_statusVisualizeNormal ) {
                this->drawModel( m_shaders.get( "normal_visualizer" ), trans );
            }
        } else {
            // Draw only Z-Buffer (for debugging/visualization)
            this->drawModel( m_shaders.get( "z_visualizer" ), trans );
        }

        // Render light source (for debugging/visualization)
        if ( m_statusRenderLightSource ) {
            m_lightBox->renderCube( m_light.position( ), m_light.specular( ) );
        }

        // Bind to default framebuffer again and draw the quad plane with attched screen texture
        m_framebuffer->unbind( );

        // ------------------------------------------------

        // Clear all relevant buffers
        glClear( GL_COLOR_BUFFER_BIT );
        glDisable( GL_DEPTH_TEST ); // We don't care about depth information when rendering a single quad

        // Use the hdr framebuffer shader
        m_shaders.get( "framebuffer" )->use( );

        // Draw the frame buffer
        m_framebuffer->draw( );

        // ------------------------------------------------

        // Draw status text
        if ( m_statusText ) {
            this->displayStatusText( );

            // fps meter
            m_frameCounter++;
            // Get time elapsed
            m_fpsDiffTime = std::chrono::duration_cast<std::chrono::duration<double>>( currentTime - m_fpsStartTime ).count( );

            if ( m_fpsDiffTime > 0.5 && m_frameCounter > 10 ) {
                // Calculate frame per secound
                m_fps = static_cast<float>( static_cast<double>( m_frameCounter ) / m_fpsDiffTime );
                // Reset frame counter
                m_frameCounter = 0;
                m_fpsStartTime = Time::now( );
            }
            // Draw fps meter to screen
            m_text->drawText( wxString::Format( wxT( "%.2f fps" ), m_fps ), 0.0f, m_clientSize.y - 48.0f, 1.0f, glm::vec3( 1.0f ) );
        }
    }

    void Renderer::drawModel(Shader* p_shader, const glm::mat4& p_trans) {

        if ( m_statusCullFace ) {
            glEnable( GL_CULL_FACE );
        } else {
            glDisable( GL_CULL_FACE );
        }

        if ( m_statusWireframe ) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            // Disable lighting for wireframe rendering
            m_shaders.get( "main" )->setInt( "mode.lighting", 0 );
        } else {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }

        // Draw meshes
        for ( auto& it : m_model ) {
            // Use the shader
            p_shader->use( );

            // Set material properties
            p_shader->setFloat( "material.shininess", 32.0f );

            // Model matrix
            p_shader->setMat4( "model", p_trans );
            // View matrix
            p_shader->setMat4( "view", m_camera.calculateViewMatrix( ) );

            it->draw( m_texture );
        }

        if ( m_statusWireframe ) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
    }

    void Renderer::displayStatusText() {
        // Send ClientSize variable to text renderer
        m_text->setClientSize( m_clientSize );

        glm::vec3 color = glm::vec3( 1.0f );
        GLfloat scale = 1.0f;

        uint numMeshes = 0;
        uint vertexCount = 0;
        uint triangleCount = 0;
        // Count vertex and triangle of model
        for ( auto& it : m_model ) {
            numMeshes += it->getNumMeshes( );
            vertexCount += it->getTriSize( );
            triangleCount += it->getVertSize( );
        }

        // Top-left text
        m_text->drawText( wxString::Format( wxT( "Meshes: %d" ), numMeshes ), 0.0f, m_clientSize.y - 12.0f, scale, color );
        m_text->drawText( wxString::Format( wxT( "Vertices: %d" ), vertexCount ), 0.0f, m_clientSize.y - 24.0f, scale, color );
        m_text->drawText( wxString::Format( wxT( "Triangles: %d" ), triangleCount ), 0.0f, m_clientSize.y - 36.0f, scale, color );

        // Bottom-left text
        m_text->drawText( wxT( "Zoom: Scroll wheel" ), 0.0f, 0.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Rotate: Left mouse button" ), 0.0f, 12.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Pan: Right mouse button" ), 0.0f, 24.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Focus: press F" ), 0.0f, 36.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Toggle anti alising: press 7" ), 25.0f, 48.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Toggle normal mapping: press 6" ), 25.0f, 60.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Toggle lighting: press 5" ), 25.0f, 72.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Toggle texture: press 4" ), 25.0f, 84.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Toggle back-face culling: press 3" ), 25.0f, 96.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Toggle wireframe: press 2" ), 25.0f, 108.0f + 2.0f, scale, color );
        m_text->drawText( wxT( "Toggle status text: press 1" ), 25.0f, 120.0f + 2.0f, scale, color );

        // Status text
        auto gray = glm::vec3( 0.5f, 0.5f, 0.5f );
        auto green = glm::vec3( 0.0f, 1.0f, 0.0f );

        if ( m_statusAntiAlising ) {
            m_text->drawText( wxT( "ON" ), 0.0f, 48.0f + 2.0f, scale, green );
        } else {
            m_text->drawText( wxT( "OFF" ), 0.0f, 48.0f + 2.0f, scale, gray );
        }

        if ( m_statusNormalMapping ) {
            m_text->drawText( wxT( "ON" ), 0.0f, 60.0f + 2.0f, scale, green );
        } else {
            m_text->drawText( wxT( "OFF" ), 0.0f, 60.0f + 2.0f, scale, gray );
        }

        if ( m_statusLighting ) {
            m_text->drawText( wxT( "ON" ), 0.0f, 72.0f + 2.0f, scale, green );
        } else {
            m_text->drawText( wxT( "OFF" ), 0.0f, 72.0f + 2.0f, scale, gray );
        }

        if ( m_statusTextured ) {
            m_text->drawText( wxT( "ON" ), 0.0f, 84.0f + 2.0f, scale, green );
        } else {
            m_text->drawText( wxT( "OFF" ), 0.0f, 84.0f + 2.0f, scale, gray );
        }

        if ( m_statusCullFace ) {
            m_text->drawText( wxT( "ON" ), 0.0f, 96.0f + 2.0f, scale, green );
        } else {
            m_text->drawText( wxT( "OFF" ), 0.0f, 96.0f + 2.0f, scale, gray );
        }

        if ( m_statusWireframe ) {
            m_text->drawText( wxT( "ON" ), 0.0f, 108.0f + 2.0f, scale, green );
        } else {
            m_text->drawText( wxT( "OFF" ), 0.0f, 108.0f + 2.0f, scale, gray );
        }

    }

    void Renderer::updateMatrices() {
        // All models are located at 0,0,0 with no rotation, so no world matrix is needed

        // Calculate minZ/maxZ
        Bounds bounds;
        if ( !m_model.empty( ) ) {
            bounds = m_model[0]->getBounds( );
        }
        auto size = bounds.size( );
        auto distance = m_camera.distance( );
        auto extents = glm::vec3( size.x * 0.5f, size.y * 0.5f, size.z * 0.5f );

        auto maxSize = ::sqrt( extents.x * extents.x + extents.y * extents.y + extents.z * extents.z );
        auto maxZ = ( maxSize + distance ) * 10.0f;
        auto minZ = maxZ * 0.001f;

        float aspectRatio = ( static_cast<float>( m_clientSize.x ) / static_cast<float>( m_clientSize.y ) );
        auto fov = 45.0f;

        // Projection matrix
        auto projection = glm::perspective( glm::radians( fov ), aspectRatio, static_cast<float>( minZ ), static_cast<float>( maxZ ) );

        m_shaders.get( "main" )->use( );
        // Send projection matrix to main shader
        m_shaders.get( "main" )->setMat4( "projection", projection );
        // View position
        m_shaders.get( "main" )->setVec3( "viewPos", m_camera.position( ) );

        // Set the shader render status flag
        m_shaders.get( "main" )->setInt( "mode.wireframe", m_statusWireframe );
        m_shaders.get( "main" )->setInt( "mode.textured", m_statusTextured );
        m_shaders.get( "main" )->setInt( "mode.normalMapping", m_statusNormalMapping );
        m_shaders.get( "main" )->setInt( "mode.lighting", m_statusLighting );

        // Set lights properties
        m_shaders.get( "main" )->setVec3( "light.position", m_light.position( ) );
        m_shaders.get( "main" )->setVec3( "light.ambient", m_light.ambient( ) );
        m_shaders.get( "main" )->setVec3( "light.diffuse", m_light.diffuse( ) );
        m_shaders.get( "main" )->setVec3( "light.specular", m_light.specular( ) );

        if ( m_statusVisualizeNormal ) {
            m_shaders.get( "normal_visualizer" )->use( );
            // Send projection matrix to normal visualizer shader
            m_shaders.get( "normal_visualizer" )->setMat4( "projection", projection );
        }

        if ( m_statusVisualizeZbuffer ) {
            m_shaders.get( "z_visualizer" )->use( );
            // Send projection matrix to Z-Buffer visualizer shader
            m_shaders.get( "z_visualizer" )->setMat4( "projection", projection );
            m_shaders.get( "z_visualizer" )->setFloat( "near", minZ );
            m_shaders.get( "z_visualizer" )->setFloat( "far", maxZ );
        }

        // Send projection matrix to lightbox renderer
        m_lightBox->setProjectionMatrix( projection );
        m_lightBox->setViewMatrix( m_camera.calculateViewMatrix( ) );
    }

    void Renderer::focus() {
        float fov = ( 5.0f / 12.0f ) * glm::pi<float>( );

        uint meshCount = m_model.size( );
        if ( !meshCount ) {
            return;
        }

        Bounds bounds;
        if ( !m_model.empty( ) ) {
            // Calculate complete bounds
            bounds = m_model[0]->getBounds( );

            float height = bounds.max.y - bounds.min.y;
            if ( height <= 0 ) {
                return;
            }

            float distance = bounds.min.z - ( ( height * 0.5f ) / ::tanf( fov * 0.5f ) );
            if ( distance < 0 ) {
                distance *= -1;
            }

            // Update camera and render
            m_camera.setPivot( bounds.center( ) );
            m_camera.setDistance( distance );
        }

        this->render( );
    }

    bool Renderer::getCameraMode() {
        return m_cameraMode;
    }

    void Renderer::setCameraMode(bool p_mode) {
        m_cameraMode = p_mode;
    }

    void Renderer::setCameraMouseSensitivity(float p_sensitivity) {
        m_camera.setMouseSensitivity(p_sensitivity);
    }

    void Renderer::processCameraKeyboard(Camera::CameraMovement p_direction) {
        m_camera.processKeyboard(p_direction, m_deltaTime);
    }

    void Renderer::processCameraMouseMovement(float p_xoffset, float p_yoffset, GLboolean p_constrainPitch) {
        m_camera.processMouseMovement(p_xoffset, p_yoffset, p_constrainPitch);
    }

    void Renderer::processCameraMouseScroll(float p_yoffset) {
        m_camera.processMouseScroll(p_yoffset);
    }

    void Renderer::panCamera(float p_x, float p_y) {
        m_camera.pan(p_x, p_y);
    }

}; // namespace gw2b
