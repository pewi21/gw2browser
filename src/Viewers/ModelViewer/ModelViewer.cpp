/* \file       Viewers/ModelViewer.cpp
*  \brief      Contains the definition of the model viewer class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2015-2017 Khral Steelforge <https://github.com/kytulendu>
Copyright (C) 2012 Rhoot <https://github.com/rhoot>

This file is part of Gw2Browser.

Gw2Browser is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"

#include <chrono>

#include "Exception.h"

#include "ModelViewer.h"

namespace gw2b {

	//----------------------------------------------------------------------------
	//      RenderTimer
	//----------------------------------------------------------------------------

	RenderTimer::RenderTimer( ModelViewer* canvas ) : wxTimer( ) {
		RenderTimer::canvas = canvas;
	}

	void RenderTimer::Notify( ) {
		canvas->Refresh( );
	}

	void RenderTimer::start( ) {
		wxTimer::Start( 10 );
	}

	//----------------------------------------------------------------------------
	//      ModelViewer
	//----------------------------------------------------------------------------

	ModelViewer::ModelViewer( wxWindow* p_parent, const int *p_attrib, const wxPoint& p_pos, const wxSize& p_size, long p_style, DatFile& p_datFile )
		: ViewerGLCanvas( p_parent, p_attrib, p_pos, p_size, p_style )
		, m_datFile( p_datFile )
		, m_lastMousePos( std::numeric_limits<int>::min( ), std::numeric_limits<int>::min( ) ) {

		// Initialize OpenGL
		if ( !m_glInitialized ) {
			if ( !this->initGL( ) ) {
				throw exception::Exception( "Could not initialize OpenGL." );
			}
			m_glInitialized = true;

			m_renderTimer = new RenderTimer( this );
			m_renderTimer->start( );
		}

		m_movementKeyTimer = new wxTimer( this );

		// Hook up events
		this->Bind( wxEVT_PAINT, &ModelViewer::onPaintEvt, this );
		this->Bind( wxEVT_MOTION, &ModelViewer::onMotionEvt, this );
		this->Bind( wxEVT_MOUSEWHEEL, &ModelViewer::onMouseWheelEvt, this );
		this->Bind( wxEVT_KEY_DOWN, &ModelViewer::onKeyDownEvt, this );
		this->Bind( wxEVT_CLOSE_WINDOW, &ModelViewer::onClose, this );
		this->Bind( wxEVT_SIZE, &ModelViewer::onResize, this );
	}

	ModelViewer::~ModelViewer( ) {
		// Clean shaders
		this->clearShader( );

		delete m_renderTimer;
		delete m_movementKeyTimer;

		delete m_glContext;
	}

	void ModelViewer::clear( ) {
		m_model.clear( );
		ViewerGLCanvas::clear( );
	}

	void ModelViewer::clearShader( ) {
		if ( m_mainShader ) {
			delete m_mainShader;
		}
		if ( m_normalVisualizerShader ) {
			delete m_normalVisualizerShader;
		}
		if ( m_zVisualizerShader ) {
			delete m_zVisualizerShader;
		}
		if ( m_screenShader ) {
			delete m_screenShader;
		}
	}

	void ModelViewer::setReader( FileReader* p_reader ) {
		if ( !m_glInitialized ) {
			// Could not initialize OpenGL
			return;
		}

		ViewerGLCanvas::setReader( p_reader );
		if ( !p_reader ) {
			return;
		}

		if ( isOfType<MapReader>( p_reader ) ) {
			// we are viewing a map file
			m_isViewingMap = true;

			auto reader = this->mapReader( );
			auto test = reader->getMapData( );


		} if ( isOfType<ModelReader>( p_reader ) ) {
			// Load model
			auto reader = this->modelReader( );
			auto model = reader->getModel( );

			// load model to m_model
			m_model.push_back( std::unique_ptr<Model>( new Model( model, m_datFile ) ) );
		}

		// Re-focus and re-render
		this->focus( );
		this->render( );
	}

	void ModelViewer::loadShader( ) {
		try {
			m_mainShader = new Shader( "..//data//shaders//shader.vert", "..//data//shaders//shader.frag" );
		} catch ( const exception::Exception& err ) {
			wxLogMessage( wxT( "m_mainShader : %s" ), wxString( err.what( ) ) );
			throw exception::Exception( "Failed to load shader." );
		}

		try {
			m_normalVisualizerShader = new Shader( "..//data//shaders//normal_visualizer.vert", "..//data//shaders//normal_visualizer.frag", "..//data//shaders//normal_visualizer.geom" );
		} catch ( const exception::Exception& err ) {
			wxLogMessage( wxT( "m_normalVisualizerShader : %s" ), wxString( err.what( ) ) );
			throw exception::Exception( "Failed to load shader." );
		}

		try {
			m_zVisualizerShader = new Shader( "..//data//shaders//z_visualizer.vert", "..//data//shaders//z_visualizer.frag" );
		} catch ( const exception::Exception& err ) {
			wxLogMessage( wxT( "m_zVisualizerShader : %s" ), wxString( err.what( ) ) );
			throw exception::Exception( "Failed to load shader." );
		}

		try {
			m_screenShader = new Shader( "..//data//shaders//framebuffer.vert", "..//data//shaders//framebuffer.frag" );
		} catch ( const exception::Exception& err ) {
			wxLogMessage( wxT( "m_screenShader : %s" ), wxString( err.what( ) ) );
			throw exception::Exception( "Failed to load shader." );
		}
	}

	void ModelViewer::reloadShader( ) {
		this->clearShader( );
		this->loadShader( );
	}

	int ModelViewer::initGL( ) {
		wxLogMessage( wxT( "Initializing OpenGL..." ) );
		// Create OpenGL context
		m_glContext = new wxGLContext( this );
		if ( !m_glContext ) {
			wxLogMessage( wxT( "Unable to create OpenGL context." ) );
			return false;
		}
		this->SetCurrent( *m_glContext );

		// Initialize GLEW to setup the OpenGL Function pointers
		glewExperimental = true;
		GLenum glewerr = glewInit( );
		if ( GLEW_OK != glewerr ) {
			wxLogMessage( wxT( "GLEW: Could not initialize GLEW library.\nError : %s" ), wxString( glewGetErrorString( glewerr ) ) );
			return false;
		}

		if ( !GLEW_VERSION_3_3 ) {
			throw exception::Exception( "The modelviewer required OpenGL 3.3 support." );
		}

		wxLogMessage( wxT( "GLEW version %s" ), wxString( glewGetString( GLEW_VERSION ) ) );
		wxLogMessage( wxT( "Running on %s from %s" ), wxString( glGetString( GL_RENDERER ) ), wxString( glGetString( GL_VENDOR ) ) );
		wxLogMessage( wxT( "OpenGL version %s" ), wxString( glGetString( GL_VERSION ) ) );

		// Update client size of wxGLCanvas.
		m_clientSize = this->GetClientSize( );

		// Set background color
		glClearColor( 0.21f, 0.21f, 0.21f, 1.0f );

		// Accept fragment if it closer to the camera than the former one
		glDepthFunc( GL_LESS );

		glEnable( GL_MULTISAMPLE );

		this->loadShader( );

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

		// create framebuffer
		this->createFrameBuffer( );

		return true;
	}

	void ModelViewer::onPaintEvt( wxPaintEvent& p_event ) {
		wxPaintDC dc( this );

		this->render( );
	}

	void ModelViewer::render( ) {
		// Set frame time
		auto now = std::chrono::high_resolution_clock::now( );
		auto currentFrame = std::chrono::duration_cast<std::chrono::duration<float>>( now.time_since_epoch( ) ).count( );
		m_deltaTime = currentFrame - m_lastFrame;
		m_lastFrame = currentFrame;

		// Update client size of wxGLCanvas
		m_clientSize = this->GetClientSize( );
		// Set the OpenGL viewport according to the client size of wxGLCanvas
		glViewport( 0, 0, m_clientSize.x, m_clientSize.y );

		// Bind to framebuffer and draw to framebuffer texture
		m_framebuffer->bind( );

		// Enable depth test
		glEnable( GL_DEPTH_TEST );

		// Clear color buffer and depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		this->updateMatrices( );

		if ( !m_model.empty( ) ) {
			// Setup light properties (currently is front of the model)
			m_light.setPosition( m_model[0]->getBounds( ).center( ) + glm::vec3( 100, 25, 200 ) );
		}

		// No need since already initilized with this value
		//m_light.setAmbient( glm::vec3( 0.5f, 0.5f, 0.5f ) );
		//m_light.setDiffuse( glm::vec3( 0.5f, 0.5f, 0.5f ) );
		//m_light.setSpecular( glm::vec3( 0.5f, 0.5f, 0.5f ) );

		// Transformation matrix
		glm::mat4 trans;
		// Model position
		trans = glm::translate( trans, glm::vec3( 0.0f, 0.0f, 0.0f ) );
		// Model rotation
		//trans = glm::rotate( trans, angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );
		//angle = angle + ( 0.5f * deltaTime );
		// Model scale
		//trans = glm::scale( trans, glm::vec3( 0.5f ) );

		if ( !m_statusVisualizeZbuffer ) {
			// Draw normally
			this->drawModel( m_mainShader, trans );
			// Draw normal lines for debugging/visualization
			if ( m_statusVisualizeNormal ) {
				this->drawModel( m_normalVisualizerShader, trans );
			}
		} else {
			// Draw only Z-Buffer (for debugging/visualization)
			this->drawModel( m_zVisualizerShader, trans );
		}

		// Render light source (for debugging/visualization)
		if ( m_statusRenderLightSource ) {
			m_lightBox->renderCube( m_light.position( ), m_light.specular( ) );
		}

		// Draw status text
		if ( m_statusText ) {
			this->displayStatusText( );
		}

		// Bind to default framebuffer again and draw the quad plane with attched screen texture
		m_framebuffer->unbind( );

		// Clear all relevant buffers
		glClear( GL_COLOR_BUFFER_BIT );
		glDisable( GL_DEPTH_TEST ); // We don't care about depth information when rendering a single quad

		// Draw Screen
		m_screenShader->use( );
		m_framebuffer->draw( );

		SwapBuffers( );
	}

	void ModelViewer::drawModel( Shader* p_shader, const glm::mat4& p_trans ) {
		bool oldStatusLighting = m_statusLighting;

		if ( m_statusCullFace ) {
			glEnable( GL_CULL_FACE );
		} else {
			glDisable( GL_CULL_FACE );
		}

		if ( m_statusWireframe ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			// Disable lighting for wireframe rendering
			m_statusLighting = false;
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		// Draw meshes
		for ( auto& it : m_model ) {
			// Use the shader
			p_shader->use( );

			// Set renderer status flag
			glUniform1i( glGetUniformLocation( p_shader->getProgramId( ), "mode.wireframe" ), m_statusWireframe );
			glUniform1i( glGetUniformLocation( p_shader->getProgramId( ), "mode.textured" ), m_statusTextured );
			glUniform1i( glGetUniformLocation( p_shader->getProgramId( ), "mode.normalMapping" ), m_statusNormalMapping );
			glUniform1i( glGetUniformLocation( p_shader->getProgramId( ), "mode.lighting" ), m_statusLighting );

			// Set lights properties
			glUniform3fv( glGetUniformLocation( p_shader->getProgramId( ), "light.position" ), 1, glm::value_ptr( m_light.position( ) ) );
			glUniform3fv( glGetUniformLocation( p_shader->getProgramId( ), "light.ambient" ), 1, glm::value_ptr( m_light.ambient( ) ) );
			glUniform3fv( glGetUniformLocation( p_shader->getProgramId( ), "light.diffuse" ), 1, glm::value_ptr( m_light.diffuse( ) ) );
			glUniform3fv( glGetUniformLocation( p_shader->getProgramId( ), "light.specular" ), 1, glm::value_ptr( m_light.specular( ) ) );

			// Set material properties
			glUniform1f( glGetUniformLocation( p_shader->getProgramId( ), "material.shininess" ), 32.0f );

			// Model matrix
			glUniformMatrix4fv( glGetUniformLocation( p_shader->getProgramId( ), "model" ), 1, GL_FALSE, glm::value_ptr( p_trans ) );
			// View matrix
			glUniformMatrix4fv( glGetUniformLocation( p_shader->getProgramId( ), "view" ), 1, GL_FALSE, glm::value_ptr( m_camera.calculateViewMatrix( ) ) );

			it->draw( p_shader );
		}

		if ( m_statusWireframe ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			// Set lighting flag to old value
			m_statusLighting = oldStatusLighting;
		}
	}

	void ModelViewer::displayStatusText( ) {
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

	void ModelViewer::updateMatrices( ) {
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
		auto projection = glm::perspective( fov, aspectRatio, static_cast<float>( minZ ), static_cast<float>( maxZ ) );

		m_mainShader->use( );
		// Send projection matrix to main shader
		glUniformMatrix4fv( glGetUniformLocation( m_mainShader->getProgramId( ), "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
		// View position
		glUniform3fv( glGetUniformLocation( m_mainShader->getProgramId( ), "viewPos" ), 1, glm::value_ptr( m_camera.position( ) ) );

		if ( m_statusVisualizeNormal ) {
			m_normalVisualizerShader->use( );
			// Send projection matrix to normal visualizer shader
			glUniformMatrix4fv( glGetUniformLocation( m_normalVisualizerShader->getProgramId( ), "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
		}

		if ( m_statusVisualizeZbuffer ) {
			m_zVisualizerShader->use( );
			// Send projection matrix to Z-Buffer visualizer shader
			glUniformMatrix4fv( glGetUniformLocation( m_zVisualizerShader->getProgramId( ), "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
			glUniform1f( glGetUniformLocation( m_zVisualizerShader->getProgramId( ), "near" ), minZ );
			glUniform1f( glGetUniformLocation( m_zVisualizerShader->getProgramId( ), "far" ), maxZ );
		}

		// Send projection matrix to lightbox renderer
		m_lightBox->setProjectionMatrix( projection );
		m_lightBox->setViewMatrix( m_camera.calculateViewMatrix( ) );
	}

	void ModelViewer::focus( ) {
		float fov = ( 5.0f / 12.0f ) * glm::pi<float>( );

		uint meshCount = m_model.size( );
		if ( !meshCount ) {
			return;
		}

		Bounds bounds;
		if ( !m_model.empty( ) ) {
			// Calculate complete bounds
			bounds = m_model[0]->getBounds( );
		}
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
		this->render( );
	}

	void ModelViewer::createFrameBuffer( ) {
		if ( m_statusAntiAlising ) {
			m_framebuffer = std::unique_ptr<FrameBuffer>( new FrameBuffer( m_clientSize, 4 ) );
		} else {
			m_framebuffer = std::unique_ptr<FrameBuffer>( new FrameBuffer( m_clientSize ) );
		}
	}

	void ModelViewer::onMotionEvt( wxMouseEvent& p_event ) {
		if ( m_lastMousePos.x == std::numeric_limits<int>::min( ) &&
			m_lastMousePos.y == std::numeric_limits<int>::min( ) ) {
			m_lastMousePos = p_event.GetPosition( );
		}

		glm::vec2 mousePos( ( p_event.GetX( ) - m_lastMousePos.x ), ( p_event.GetY( ) - m_lastMousePos.y ) );

		// Yaw/Pitch
		if ( p_event.LeftIsDown( ) ) {
			if ( m_cameraMode ) {
				// FPS Mode
				m_camera.processMouseMovement( mousePos.x, -mousePos.y );
			} else {
				// ORBITAL Mode
				m_camera.processMouseMovement( -mousePos.x, mousePos.y );
			}
			this->render( );
		}

		// Pan
		if ( p_event.RightIsDown( ) && !m_cameraMode ) {
			m_camera.pan( mousePos.x, mousePos.y ); // Y mouse position is reversed since y-coordinates go from bottom to left
			this->render( );
		}

		m_lastMousePos = p_event.GetPosition( );

	}

	void ModelViewer::onMouseWheelEvt( wxMouseEvent& p_event ) {
		float zoomSteps = static_cast<float>( p_event.GetWheelRotation( ) ) / static_cast<float>( p_event.GetWheelDelta( ) );
		m_camera.processMouseScroll( -zoomSteps );
		this->render( );
	}

	void ModelViewer::onKeyDownEvt( wxKeyEvent& p_event ) {
		// Rendering control
		if ( p_event.GetKeyCode( ) == 'F' ) {
			this->focus( );
		} else if ( p_event.GetKeyCode( ) == '1' ) {
			m_statusText = !m_statusText;
		} else if ( p_event.GetKeyCode( ) == '2' ) {
			m_statusWireframe = !m_statusWireframe;
		} else if ( p_event.GetKeyCode( ) == '3' ) {
			m_statusCullFace = !m_statusCullFace;
		} else if ( p_event.GetKeyCode( ) == '4' ) {
			m_statusTextured = !m_statusTextured;
		} else if ( p_event.GetKeyCode( ) == '5' ) {
			m_statusLighting = !m_statusLighting;
		} else if ( p_event.GetKeyCode( ) == '6' ) {
			m_statusNormalMapping = !m_statusNormalMapping;
		} else if ( p_event.GetKeyCode( ) == '7' ) {
			m_statusAntiAlising = !m_statusAntiAlising;
			// Force re-create framebuffer
			this->createFrameBuffer( );
		}

		// Rendering debugging/visualization control
		else if ( p_event.GetKeyCode( ) == 'N' ) {
			m_statusVisualizeNormal = !m_statusVisualizeNormal;
		} else if ( p_event.GetKeyCode( ) == 'M' ) {
			m_statusVisualizeZbuffer = !m_statusVisualizeZbuffer;
		} else if ( p_event.GetKeyCode( ) == 'L' ) {
			m_statusRenderLightSource = !m_statusRenderLightSource;
		}

		// Camera control
		else if ( p_event.GetKeyCode( ) == 'C' ) {
			Camera::CameraMode mode;
			float sensivity = 0.25f;

			m_cameraMode = !m_cameraMode;

			if ( m_cameraMode ) {
				mode = Camera::CameraMode::FPSCAM;
				sensivity = 0.25f;

				// Scan for input every 3ms
				m_movementKeyTimer->Start( 3 );
				this->Bind( wxEVT_TIMER, &ModelViewer::onMovementKeyTimerEvt, this );
			} else {
				mode = Camera::CameraMode::ORBITALCAM;
				sensivity = 0.5f * ( glm::pi<float>( ) / 180.0f );  // 0.5 degrees per pixel

				m_movementKeyTimer->Stop( );
				this->Unbind( wxEVT_TIMER, &ModelViewer::onMovementKeyTimerEvt, this );
			}
			m_camera.setCameraMode( mode );
			m_camera.setMouseSensitivity( sensivity );
		}

		// Reload shaders
		else if ( p_event.GetKeyCode( ) == '=' ) {
			wxLogMessage( wxT( "Reloading shader..." ) );
			this->reloadShader( );
			wxLogMessage( wxT( "Done." ) );
		}
	}

	void ModelViewer::onMovementKeyTimerEvt( wxTimerEvent&p_event ) {
		m_movementKeyTimer->Stop( );
		// Prevent modify camera value if the camera is in orbital mode
		if ( m_cameraMode ) {
			// First person camera control
			if ( wxGetKeyState( wxKeyCode( 'W' ) ) ) {
				m_camera.processKeyboard( Camera::CameraMovement::FORWARD, m_deltaTime );
			}
			if ( wxGetKeyState( wxKeyCode( 'S' ) ) ) {
				m_camera.processKeyboard( Camera::CameraMovement::BACKWARD, m_deltaTime );
			}
			if ( wxGetKeyState( wxKeyCode( 'A' ) ) ) {
				m_camera.processKeyboard( Camera::CameraMovement::LEFT, m_deltaTime );
			}
			if ( wxGetKeyState( wxKeyCode( 'D' ) ) ) {
				m_camera.processKeyboard( Camera::CameraMovement::RIGHT, m_deltaTime );
			}
		}
		m_movementKeyTimer->Start( );
	}

	void ModelViewer::onClose( wxCloseEvent& evt ) {
		m_renderTimer->Stop( );
		evt.Skip( );
	}

	void ModelViewer::onResize( wxSizeEvent& evt ) {
		// Check if framebuffer is already create. If found, delete it.
		if ( m_framebuffer ) {
			m_framebuffer.reset( );
		}

		// Update client size of wxGLCanvas.
		m_clientSize = this->GetClientSize( );

		this->createFrameBuffer( );
	}

}; // namespace gw2b
