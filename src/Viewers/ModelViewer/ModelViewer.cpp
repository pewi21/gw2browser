/* \file       Viewers/ModelViewer.cpp
*  \brief      Contains the definition of the model viewer class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2015-2016 Khral Steelforge <https://github.com/kytulendu>
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

	struct ModelViewer::MeshCache {
		std::vector<glm::vec3>	vertices;
		std::vector<glm::vec3>	normals;
		std::vector<glm::vec2>	uvs;
		std::vector<uint>		indices;
		std::vector<glm::vec3>	tangents;
	};

	struct ModelViewer::VAO {
		GLuint					vertexArray;
	};

	struct ModelViewer::VBO {
		GLuint					vertexBuffer;
		GLuint					normalBuffer;
		GLuint					uvBuffer;
		GLuint					tangentbuffer;
	};

	struct ModelViewer::IBO {
		GLuint					elementBuffer;
	};

	struct ModelViewer::TextureList {
		uint					diffuseMap;
		uint					normalMap;
		uint					lightMap;
	};

	struct ModelViewer::PackedVertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;

		bool operator < ( const PackedVertex& that ) const {
			return memcmp( ( void* )this, ( void* ) &that, sizeof( PackedVertex ) ) > 0;
		};
	};

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
	}

	ModelViewer::~ModelViewer( ) {
		this->clearBuffer( );

		// Clean dummy textures
		glDeleteTextures( 1, &m_dummyBlackTexture );
		glDeleteTextures( 1, &m_dummyWhiteTexture );

		// Clean shaders
		this->clearShader( );

		// Clean text renderer
		delete m_text;
		// Clean light box renderer
		delete m_lightBox;

		delete m_renderTimer;
		delete m_movementKeyTimer;

		delete m_glContext;
	}

	void ModelViewer::clear( ) {
		this->clearBuffer( );

		m_vertexBuffer.clear( );
		m_indexBuffer.clear( );
		m_textureList.clear( );
		m_textureMap.clear( );
		m_meshCache.clear( );
		m_model = GW2Model( );
		ViewerGLCanvas::clear( );
	}

	void ModelViewer::clearBuffer( ) {
		// Clean VBO
		for ( auto& it : m_vertexBuffer ) {
			if ( it.vertexBuffer ) {
				glDeleteBuffers( 1, &it.vertexBuffer );
			}
			if ( it.normalBuffer ) {
				glDeleteBuffers( 1, &it.normalBuffer );
			}
			if ( it.uvBuffer ) {
				glDeleteBuffers( 1, &it.uvBuffer );
			}
			if ( it.tangentbuffer ) {
				glDeleteBuffers( 1, &it.tangentbuffer );
			}
		}

		// Clean IBO
		for ( auto& it : m_indexBuffer ) {
			if ( it.elementBuffer ) {
				glDeleteBuffers( 1, &it.elementBuffer );
			}
		}

		// Clean Texture Map
		for ( auto& it : m_textureMap ) {
			if ( it.second ) {
				delete it.second;
			}
		}

		// Clean VAO
		for ( auto& it : m_vertexArray ) {
			if ( it.vertexArray ) {
				glDeleteVertexArrays( 1, &it.vertexArray );
			}
		}
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

	}

	void ModelViewer::setReader( FileReader* p_reader ) {
		if ( !m_glInitialized ) {
			// Could not initialize OpenGL
			return;
		}

		Ensure::isOfType<ModelReader>( p_reader );
		ViewerGLCanvas::setReader( p_reader );

		// Load model
		auto reader = this->modelReader( );
		m_model = reader->getModel( );

		this->loadModel( m_model );
		this->loadMaterial( m_model );

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

		// Set background color
		glClearColor( 0.21f, 0.21f, 0.21f, 1.0f );

		// Enable multisampling, not really need since it was enabled at context creation
		glEnable( GL_MULTISAMPLE );

		// Enable depth test
		glEnable( GL_DEPTH_TEST );

		// Accept fragment if it closer to the camera than the former one
		glDepthFunc( GL_LESS );

		this->loadShader( );

		// Initialize text renderer stuff
		try {
			m_text = new Text2D( );
		} catch ( const exception::Exception& err ) {
			wxLogMessage( wxT( "m_text : %s" ), wxString( err.what( ) ) );
			throw exception::Exception( "Failed to initialize text renderer." );
		}

		// Initialize lightbox renderer
		try {
			m_lightBox = new LightBox( );
		} catch ( const exception::Exception& err ) {
			wxLogMessage( wxT( "m_lightBox : %s" ), wxString( err.what( ) ) );
			throw exception::Exception( "Failed to initialize lightbox renderer." );
		}

		// Create dummy texture
		GLubyte blackTextureData[] = { 0, 0, 0, 255 };
		m_dummyBlackTexture = createDummyTexture( blackTextureData );

		GLubyte whiteTextureData[] = { 255, 255, 255, 255 };
		m_dummyWhiteTexture = createDummyTexture( whiteTextureData );

		// Setup camera
		m_camera.setCameraMode( Camera::CameraMode::ORBITALCAM );
		m_camera.setMouseSensitivity( 0.5f * ( glm::pi<float>( ) / 180.0f ) );  // 0.5 degrees per pixel

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

		// Set the OpenGL viewport according to the client size of wxGLCanvas.
		wxSize ClientSize = this->GetClientSize( );
		glViewport( 0, 0, ClientSize.x, ClientSize.y );

		// Clear color buffer and depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		this->updateMatrices( );

		// Setup light properties (currently is front of the model)
		m_light.setPosition( m_model.bounds( ).center( ) + glm::vec3( 100, 25, 200 ) );
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

		SwapBuffers( );
	}

	void ModelViewer::drawModel( Shader* p_shader, const glm::mat4& p_trans ) {
		// Draw meshes
		for ( uint i = 0; i < m_model.numMeshes( ); i++ ) {
			// Draw normally
			this->drawMesh( p_shader, p_trans, i );

			// Draw normal lines for debugging/visualization
			if ( m_statusVisualizeNormal ) {
				this->drawMesh( m_normalVisualizerShader, p_trans, i );
			}
		}
	}

	void ModelViewer::drawMesh( Shader* p_shader, const glm::mat4& p_trans, const uint p_meshIndex ) {
		auto& vao = m_vertexArray[p_meshIndex];
		auto& cache = m_meshCache[p_meshIndex];

		auto materialIndex = m_model.mesh( p_meshIndex ).materialIndex;

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

		// Use the shader
		p_shader->use( );

		// Set renderer status flag
		glUniform1i( glGetUniformLocation( p_shader->getProgramId( ), "mode.normalMapping" ), m_statusNormalMapping );
		glUniform1i( glGetUniformLocation( p_shader->getProgramId( ), "mode.lighting" ), m_statusLighting );

		// Set lights properties
		glUniform3fv( glGetUniformLocation( p_shader->getProgramId( ), "light.position" ), 1, glm::value_ptr( m_light.position( ) ) );
		glUniform3fv( glGetUniformLocation( p_shader->getProgramId( ), "light.ambient" ), 1, glm::value_ptr( m_light.ambient( ) ) );
		glUniform3fv( glGetUniformLocation( p_shader->getProgramId( ), "light.diffuse" ), 1, glm::value_ptr( m_light.diffuse( ) ) );
		glUniform3fv( glGetUniformLocation( p_shader->getProgramId( ), "light.specular" ), 1, glm::value_ptr( m_light.specular( ) ) );

		// Set material properties
		glUniform1f( glGetUniformLocation( p_shader->getProgramId( ), "material.shininess" ), 32.0f );

		// View matrix
		glUniformMatrix4fv( glGetUniformLocation( p_shader->getProgramId( ), "view" ), 1, GL_FALSE, glm::value_ptr( m_camera.calculateViewMatrix( ) ) );
		// Model matrix
		glUniformMatrix4fv( glGetUniformLocation( p_shader->getProgramId( ), "model" ), 1, GL_FALSE, glm::value_ptr( p_trans ) );

		// Texture Maping
		if ( !m_textureList.empty( ) ) {

			// Use Texture Unit 0
			glActiveTexture( GL_TEXTURE0 );

			if ( m_statusTextured ) {
				if ( m_statusWireframe ) {
					// Black texture, for wireframe view
					glBindTexture( GL_TEXTURE_2D, m_dummyBlackTexture );
				} else if ( materialIndex >= 0 && m_textureList[materialIndex].diffuseMap ) {
					auto& texture = m_textureMap.find( m_textureList[materialIndex].diffuseMap )->second;
					if ( texture ) {
						texture->bind( );
					}
				}
			} else {
				if ( m_statusWireframe ) {
					// Black texture, for wireframe view
					glBindTexture( GL_TEXTURE_2D, m_dummyBlackTexture );
				} else {
					// White texture, no texture
					glBindTexture( GL_TEXTURE_2D, m_dummyWhiteTexture );
				}
			}

			// Set our "diffuseMap" sampler to user Texture Unit 0
			glUniform1i( glGetUniformLocation( p_shader->getProgramId( ), "material.diffuseMap" ), 0 );

			// Bind our normal texture in Texture Unit 1
			glActiveTexture( GL_TEXTURE1 );

			if ( materialIndex >= 0 && m_textureList[materialIndex].normalMap ) {
				auto& texture = m_textureMap.find( m_textureList[materialIndex].normalMap )->second;
				if ( texture ) {
					texture->bind( );
				}
			}
			// Set our "normalMap" sampler to user Texture Unit 1
			glUniform1i( glGetUniformLocation( p_shader->getProgramId( ), "material.normalMap" ), 1 );
		}

		// Bind Vertex Array Object
		glBindVertexArray( vao.vertexArray );
		// Draw the triangles!
		glDrawElements( GL_TRIANGLES, cache.indices.size( ), GL_UNSIGNED_INT, ( GLvoid* ) 0 );
		// Unbind Vertex Array Object
		glBindVertexArray( 0 );

		// Unbind texture from Texture Unit 1
		//glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		// Unbind texture from Texture Unit 0
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		if ( m_statusWireframe ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			// Set lighting flag to old value
			m_statusLighting = oldStatusLighting;
		}
	}

	void ModelViewer::displayStatusText( ) {
		wxSize ClientSize = this->GetClientSize( );

		// Send ClientSize variable to text renderer
		m_text->setClientSize( ClientSize );

		glm::vec3 color = glm::vec3( 1.0f );
		GLfloat scale = 1.0f;

		uint vertexCount = 0;
		uint triangleCount = 0;
		// Count vertex and triangle of model
		for ( uint i = 0; i < m_model.numMeshes( ); i++ ) {
			vertexCount += m_model.mesh( i ).vertices.size( );
			triangleCount += m_model.mesh( i ).triangles.size( );
		}

		// Top-left text
		m_text->drawText( wxString::Format( wxT( "Meshes: %d" ), m_model.numMeshes( ) ), 0.0f, ClientSize.y - 12.0f, scale, color );
		m_text->drawText( wxString::Format( wxT( "Vertices: %d" ), vertexCount ), 0.0f, ClientSize.y - 24.0f, scale, color );
		m_text->drawText( wxString::Format( wxT( "Triangles: %d" ), triangleCount ), 0.0f, ClientSize.y - 36.0f, scale, color );

		// Bottom-left text
		m_text->drawText( wxT( "Zoom: Scroll wheel" ), 0.0f, 0.0f + 2.0f, scale, color );
		m_text->drawText( wxT( "Rotate: Left mouse button" ), 0.0f, 12.0f + 2.0f, scale, color );
		m_text->drawText( wxT( "Pan: Right mouse button" ), 0.0f, 24.0f + 2.0f, scale, color );
		m_text->drawText( wxT( "Focus: press F" ), 0.0f, 36.0f + 2.0f, scale, color );
		m_text->drawText( wxT( "Toggle normal mapping: press 6" ), 25.0f, 48.0f + 2.0f, scale, color );
		m_text->drawText( wxT( "Toggle lighting: press 5" ), 25.0f, 60.0f + 2.0f, scale, color );
		m_text->drawText( wxT( "Toggle texture: press 4" ), 25.0f, 72.0f + 2.0f, scale, color );
		m_text->drawText( wxT( "Toggle back-face culling: press 3" ), 25.0f, 84.0f + 2.0f, scale, color );
		m_text->drawText( wxT( "Toggle wireframe: press 2" ), 25.0f, 96.0f + 2.0f, scale, color );
		m_text->drawText( wxT( "Toggle status text: press 1" ), 25.0f, 108.0f + 2.0f, scale, color );

		// Status text
		auto gray = glm::vec3( 0.5f, 0.5f, 0.5f );
		auto green = glm::vec3( 0.0f, 1.0f, 0.0f );
		if ( m_statusNormalMapping ) {
			m_text->drawText( wxT( "ON" ), 0.0f, 48.0f + 2.0f, scale, green );
		} else {
			m_text->drawText( wxT( "OFF" ), 0.0f, 48.0f + 2.0f, scale, gray );
		}

		if ( m_statusLighting ) {
			m_text->drawText( wxT( "ON" ), 0.0f, 60.0f + 2.0f, scale, green );
		} else {
			m_text->drawText( wxT( "OFF" ), 0.0f, 60.0f + 2.0f, scale, gray );
		}

		if ( m_statusTextured ) {
			m_text->drawText( wxT( "ON" ), 0.0f, 72.0f + 2.0f, scale, green );
		} else {
			m_text->drawText( wxT( "OFF" ), 0.0f, 72.0f + 2.0f, scale, gray );
		}

		if ( m_statusCullFace ) {
			m_text->drawText( wxT( "ON" ), 0.0f, 84.0f + 2.0f, scale, green );
		} else {
			m_text->drawText( wxT( "OFF" ), 0.0f, 84.0f + 2.0f, scale, gray );
		}

		if ( m_statusWireframe ) {
			m_text->drawText( wxT( "ON" ), 0.0f, 96.0f + 2.0f, scale, green );
		} else {
			m_text->drawText( wxT( "OFF" ), 0.0f, 96.0f + 2.0f, scale, gray );
		}
	}

	void ModelViewer::loadModel( GW2Model& p_model ) {
		auto numMeshes = p_model.numMeshes( );

		// Create mesh cache
		m_meshCache.resize( numMeshes );

		// Load mesh to mesh cache
#pragma omp parallel for
		for ( int i = 0; i < static_cast<int>( numMeshes ); i++ ) {
			auto& mesh = p_model.mesh( i );
			auto& cache = m_meshCache[i];

			this->loadMesh( cache, mesh );
		}

		// Create Vertex Array Object, Vertex Buffer Object and Index Buffer Object
		m_vertexArray.resize( numMeshes );
		m_vertexBuffer.resize( numMeshes );
		m_indexBuffer.resize( numMeshes );

		// Populate Buffer Object
		for ( uint i = 0; i < m_meshCache.size( ); i++ ) {
			auto& cache = m_meshCache[i];
			auto& vao = m_vertexArray[i];
			auto& vbo = m_vertexBuffer[i];
			auto& ibo = m_indexBuffer[i];

			this->populateBuffers( vao, vbo, ibo, cache );
		}
	}

	void ModelViewer::loadMesh( MeshCache& p_cache, const GW2Mesh& p_mesh ) {
		// Tempoarary buffers
		MeshCache temp;

		// Read faces
		for ( auto& it : p_mesh.triangles ) {
			temp.indices.push_back( it.index1 );
			temp.indices.push_back( it.index2 );
			temp.indices.push_back( it.index3 );
		}

		// For each vertex of each triangle
		for ( auto& it : temp.indices ) {
			auto& vertex = p_mesh.vertices[it].position;
			temp.vertices.push_back( vertex );

			if ( p_mesh.hasNormal ) {
				auto& normal = p_mesh.vertices[it].normal;
				temp.normals.push_back( normal );
			}

			if ( p_mesh.hasUV ) {
				auto& uv = p_mesh.vertices[it].uv;
				temp.uvs.push_back( uv );
			} else {
				temp.uvs.push_back( glm::vec2( 0.0f, 0.0f ) );
			}
		}

		this->computeTangent( temp );
		this->indexVBO( temp, p_cache );
	}

	void ModelViewer::computeTangent( MeshCache& p_mesh ) {
		for ( uint i = 0; i < p_mesh.vertices.size( ); i += 3 ) {
			// Shortcuts for vertices
			glm::vec3& v0 = p_mesh.vertices[i + 0];
			glm::vec3& v1 = p_mesh.vertices[i + 1];
			glm::vec3& v2 = p_mesh.vertices[i + 2];

			// Shortcuts for UVs
			glm::vec2& uv0 = p_mesh.uvs[i + 0];
			glm::vec2& uv1 = p_mesh.uvs[i + 1];
			glm::vec2& uv2 = p_mesh.uvs[i + 2];

			// Edges of the triangle : postion delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			GLfloat f = 1.0f / ( deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y );

			glm::vec3 tangent;
			tangent.x = f * ( deltaUV2.y * deltaPos1.x - deltaUV1.y * deltaPos2.x );
			tangent.y = f * ( deltaUV2.y * deltaPos1.y - deltaUV1.y * deltaPos2.y );
			tangent.z = f * ( deltaUV2.y * deltaPos1.z - deltaUV1.y * deltaPos2.z );
			tangent = glm::normalize( tangent );

			// Set the same tangent for all three vertices of the triangle.
			// They will be merged later in VBO indexer.
			p_mesh.tangents.push_back( tangent );
			p_mesh.tangents.push_back( tangent );
			p_mesh.tangents.push_back( tangent );
		}
	}

	bool ModelViewer::getSimilarVertexIndex( PackedVertex & p_packed, std::map<PackedVertex, uint>& p_vertexToOutIndex, uint& p_result ) {
		std::map<PackedVertex, uint>::iterator it = p_vertexToOutIndex.find( p_packed );
		if ( it == p_vertexToOutIndex.end( ) ) {
			return false;
		} else {
			p_result = it->second;
			return true;
		}
	}

	void ModelViewer::indexVBO( const MeshCache& p_inMesh, MeshCache& p_outMesh ) {
		std::map<PackedVertex, uint> VertexToOutIndex;

		// For each input vertex
		for ( uint i = 0; i < p_inMesh.vertices.size( ); i++ ) {
			glm::vec3 vertices;
			glm::vec3 normals;
			glm::vec2 uvs;

			vertices = p_inMesh.vertices[i];
			normals = p_inMesh.normals[i];
			uvs = p_inMesh.uvs[i];

			PackedVertex packed = { vertices, normals, uvs };

			// Try to find a similar vertex in p_outMesh
			uint index;
			bool found = getSimilarVertexIndex( packed, VertexToOutIndex, index );

			if ( found ) { // A similar vertex is already in the VBO, use it instead !
				p_outMesh.indices.push_back( index );

				// Average the tangents
				p_outMesh.tangents[index] += p_inMesh.tangents[i];
			} else { // If not, it needs to be added in the output data.
				p_outMesh.vertices.push_back( p_inMesh.vertices[i] );
				p_outMesh.normals.push_back( p_inMesh.normals[i] );
				p_outMesh.uvs.push_back( p_inMesh.uvs[i] );
				p_outMesh.tangents.push_back( p_inMesh.tangents[i] );

				uint newindex = ( uint ) p_outMesh.vertices.size( ) - 1;
				p_outMesh.indices.push_back( newindex );
				VertexToOutIndex[packed] = newindex;
			}
		}
	}

	void ModelViewer::populateBuffers( VAO& p_vao, VBO& p_vbo, IBO& p_ibo, const MeshCache& p_cache ) {
		// Generate Vertex Array Object
		glGenVertexArrays( 1, &p_vao.vertexArray );
		// Bind Vertex Array Object
		glBindVertexArray( p_vao.vertexArray );

		// Generate buffers
		glGenBuffers( 1, &p_vbo.vertexBuffer );
		glGenBuffers( 1, &p_vbo.normalBuffer );
		glGenBuffers( 1, &p_vbo.uvBuffer );
		glGenBuffers( 1, &p_vbo.tangentbuffer );
		glGenBuffers( 1, &p_ibo.elementBuffer );

		// Load data into vertex buffers
		// Vertex Positions
		glBindBuffer( GL_ARRAY_BUFFER, p_vbo.vertexBuffer );
		glBufferData( GL_ARRAY_BUFFER, p_cache.vertices.size( ) * sizeof( glm::vec3 ), &p_cache.vertices[0], GL_STATIC_DRAW );
		// Vertex Normals
		glBindBuffer( GL_ARRAY_BUFFER, p_vbo.normalBuffer );
		glBufferData( GL_ARRAY_BUFFER, p_cache.normals.size( ) * sizeof( glm::vec3 ), &p_cache.normals[0], GL_STATIC_DRAW );
		// Vertex Texture Coords
		glBindBuffer( GL_ARRAY_BUFFER, p_vbo.uvBuffer );
		glBufferData( GL_ARRAY_BUFFER, p_cache.uvs.size( ) * sizeof( glm::vec2 ), &p_cache.uvs[0], GL_STATIC_DRAW );
		// Vertex Tangent
		glBindBuffer( GL_ARRAY_BUFFER, p_vbo.tangentbuffer );
		glBufferData( GL_ARRAY_BUFFER, p_cache.tangents.size( ) * sizeof( glm::vec3 ), &p_cache.tangents[0], GL_STATIC_DRAW );

		// Element Buffer for the indices
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_ibo.elementBuffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, p_cache.indices.size( ) * sizeof( uint ), &p_cache.indices[0], GL_STATIC_DRAW );

		// Set the vertex attribute pointers
		// positions
		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, p_vbo.vertexBuffer );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );
		// normals
		glEnableVertexAttribArray( 1 );
		glBindBuffer( GL_ARRAY_BUFFER, p_vbo.normalBuffer );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );
		// texCoords
		glEnableVertexAttribArray( 2 );
		glBindBuffer( GL_ARRAY_BUFFER, p_vbo.uvBuffer );
		glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );
		// tangents
		glEnableVertexAttribArray( 3 );
		glBindBuffer( GL_ARRAY_BUFFER, p_vbo.tangentbuffer );
		glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );

		// Unbind Vertex Array Object
		glBindVertexArray( 0 );
	}

	void ModelViewer::loadMaterial( GW2Model& p_model ) {
		auto numMaterial = p_model.numMaterial( );
		auto& texture = m_textureMap;

		// Load textures to texture map
		for ( int i = 0; i < static_cast<int>( numMaterial ); i++ ) {
			auto& material = p_model.material( i );
			std::unordered_map<uint32, Texture2D*>::iterator it;

			// Load diffuse texture
			if ( material.diffuseMap ) {
				it = texture.find( material.diffuseMap );
				if ( it == texture.end( ) ) {
					try {
						texture.insert( std::pair<uint32, Texture2D*>( material.diffuseMap, new Texture2D( m_datFile, material.diffuseMap ) ) );
					} catch ( const exception::Exception& err ) {
						wxLogMessage( wxT( "Failed to load texture %d : %s" ), material.diffuseMap, wxString( err.what( ) ) );
					}
				}
			}

			if ( material.normalMap ) {
				it = texture.find( material.normalMap );
				if ( it == texture.end( ) ) {
					try {
						texture.insert( std::pair<uint32, Texture2D*>( material.normalMap, new Texture2D( m_datFile, material.normalMap ) ) );
					} catch ( const exception::Exception& err ) {
						wxLogMessage( wxT( "Failed to load texture %d : %s" ), material.normalMap, wxString( err.what( ) ) );
					}
				}
			}

			if ( material.lightMap ) {
				it = texture.find( material.lightMap );
				if ( it == texture.end( ) ) {
					try {
						texture.insert( std::pair<uint32, Texture2D*>( material.lightMap, new Texture2D( m_datFile, material.lightMap ) ) );
					} catch ( const exception::Exception& err ) {
						wxLogMessage( wxT( "Failed to load texture %d : %s" ), material.lightMap, wxString( err.what( ) ) );
					}
				}
			}
		}

		// Create Texture Buffer Object
		m_textureList.resize( numMaterial );

		// Copy texture information from material of GW2Model to m_textureList
		for ( int i = 0; i < static_cast<int>( numMaterial ); i++ ) {
			auto& material = p_model.material( i );
			auto& list = m_textureList[i];

			// Load diffuse texture
			if ( material.diffuseMap ) {
				list.diffuseMap = material.diffuseMap;
			} else {
				list.diffuseMap = 0;
			}

			if ( material.normalMap ) {
				list.normalMap = material.normalMap;
			} else {
				list.normalMap = 0;
			}

			if ( material.lightMap ) {
				list.lightMap = material.lightMap;
			} else {
				list.lightMap = 0;
			}
		}
	}

	GLuint ModelViewer::createDummyTexture( const GLubyte* p_data ) {
		// Create one OpenGL texture
		GLuint Texture;
		glGenTextures( 1, &Texture );

		// "Bind" the newly created texture
		glBindTexture( GL_TEXTURE_2D, Texture );

		// Give the image to OpenGL
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_data );

		glBindTexture( GL_TEXTURE_2D, 0 );

		return Texture;
	}

	void ModelViewer::updateMatrices( ) {
		// All models are located at 0,0,0 with no rotation, so no world matrix is needed

		// Calculate minZ/maxZ
		auto bounds = m_model.bounds( );
		auto size = bounds.size( );
		auto distance = m_camera.distance( );
		auto extents = glm::vec3( size.x * 0.5f, size.y * 0.5f, size.z * 0.5f );

		auto maxSize = ::sqrt( extents.x * extents.x + extents.y * extents.y + extents.z * extents.z );
		auto maxZ = ( maxSize + distance ) * 10.0f;
		auto minZ = maxZ * 0.001f;

		const wxSize ClientSize = this->GetClientSize( );
		float aspectRatio = ( static_cast<float>( ClientSize.x ) / static_cast<float>( ClientSize.y ) );
		auto fov = 45.0f;

		// Projection matrix
		auto projection = glm::perspective( fov, aspectRatio, static_cast<float>( minZ ), static_cast<float>( maxZ ) );

		m_mainShader->use( );
		// Send projection matrix to main shader
		glUniformMatrix4fv( glGetUniformLocation( m_mainShader->getProgramId( ), "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
		// View position
		glUniform3fv( glGetUniformLocation( m_mainShader->getProgramId( ), "viewPos" ), 1, glm::value_ptr( m_camera.position( ) ) );

		m_normalVisualizerShader->use( );
		// Send projection matrix to normal visualizer shader
		glUniformMatrix4fv( glGetUniformLocation( m_normalVisualizerShader->getProgramId( ), "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );

		m_zVisualizerShader->use( );
		// Send projection matrix to Z-Buffer visualizer shader
		glUniformMatrix4fv( glGetUniformLocation( m_zVisualizerShader->getProgramId( ), "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
		glUniform1f( glGetUniformLocation( m_zVisualizerShader->getProgramId( ), "near" ), minZ );
		glUniform1f( glGetUniformLocation( m_zVisualizerShader->getProgramId( ), "far" ), maxZ );

		// Send projection matrix to lightbox renderer
		m_lightBox->setProjectionMatrix( projection );
		m_lightBox->setViewMatrix( m_camera.calculateViewMatrix( ) );
	}

	void ModelViewer::focus( ) {
		float fov = ( 5.0f / 12.0f ) * glm::pi<float>( );

		uint meshCount = m_model.numMeshes( );
		if ( !meshCount ) {
			return;
		}

		// Calculate complete bounds
		Bounds bounds = m_model.bounds( );
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

}; // namespace gw2b
