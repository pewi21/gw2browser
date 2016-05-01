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

#include <map>
#include <vector>

#include "Readers/ImageReader.h"

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
		std::vector<glm::vec2>	uvs;
		std::vector<glm::vec3>	normals;
		std::vector<uint>		indices;
	};

	struct ModelViewer::VBO {
		GLuint					vertexBuffer;
		GLuint					uvBuffer;
		GLuint					normalBuffer;
	};

	struct ModelViewer::IBO {
		GLuint					elementBuffer;
	};

	struct ModelViewer::TBO {
		GLuint					diffuseMap;
		GLuint					normalMap;
		GLuint					lightMap;
	};

	struct ModelViewer::PackedVertex {
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;

		bool operator < ( const PackedVertex that ) const {
			return memcmp( ( void* )this, ( void* ) &that, sizeof( PackedVertex ) ) > 0;
		};
	};

	struct ModelViewer::Character {
		GLuint TextureID;		// ID handle of the glyph texture
		glm::ivec2 Size;		// Size of glyph
		glm::ivec2 Bearing;		// Offset from baseline to left/top of glyph
		GLuint Advance;			// Horizontal offset to advance to next glyph
	};

	ModelViewer::ModelViewer( wxWindow* p_parent, const int *p_attrib, const wxPoint& p_pos, const wxSize& p_size, long p_style, DatFile& p_datFile )
		: ViewerGLCanvas( p_parent, p_attrib, p_pos, p_size, p_style )
		, m_datFile( p_datFile )
		, m_lastMousePos( std::numeric_limits<int>::min( ), std::numeric_limits<int>::min( ) ) {

		// Initialize OpenGL
		if ( !m_glInitialized ) {
			if ( !this->initGL( ) ) {
				wxLogMessage( wxT( "Could not initialize OpenGL!" ) );
				wxMessageBox( wxT( "Could not initialize OpenGL!" ), wxT( "" ), wxICON_ERROR );
				return;
			}
			m_glInitialized = true;

			m_renderTimer = new RenderTimer( this );
			m_renderTimer->start( );
		}

		// Hook up events
		this->Bind( wxEVT_PAINT, &ModelViewer::onPaintEvt, this );
		this->Bind( wxEVT_MOTION, &ModelViewer::onMotionEvt, this );
		this->Bind( wxEVT_MOUSEWHEEL, &ModelViewer::onMouseWheelEvt, this );
		this->Bind( wxEVT_KEY_DOWN, &ModelViewer::onKeyDownEvt, this );
		this->Bind( wxEVT_CLOSE_WINDOW, &ModelViewer::onClose, this );
	}

	ModelViewer::~ModelViewer( ) {
		// Clean VBO
		for ( auto& it : m_vertexBuffer ) {
			if ( it.vertexBuffer ) {
				glDeleteBuffers( 1, &it.vertexBuffer );
			}
			if ( it.uvBuffer ) {
				glDeleteBuffers( 1, &it.uvBuffer );
			}
			if ( it.normalBuffer ) {
				glDeleteBuffers( 1, &it.normalBuffer );
			}
		}

		// Clean IBO
		for ( auto& it : m_indexBuffer ) {
			if ( it.elementBuffer ) {
				glDeleteBuffers( 1, &it.elementBuffer );
			}
		}

		// Clean TBO
		for ( auto& it : m_textureBuffer ) {
			if ( it.diffuseMap ) {
				glDeleteBuffers( 1, &it.diffuseMap );
			}
			if ( it.normalMap ) {
				glDeleteBuffers( 1, &it.normalMap );
			}
			if ( it.lightMap ) {
				glDeleteBuffers( 1, &it.lightMap );
			}
		}

		// Clean character textures
		for ( auto& it : m_characterTextureMap ) {
			if ( it.first ) {
				glDeleteBuffers( 1, &it.second.TextureID );
			}
		}

		// Clean dummy textures
		glDeleteBuffers( 1, &m_dummyBlackTexture );
		glDeleteBuffers( 1, &m_dummyWhiteTexture );

		// Clean shaders
		glDeleteProgram( programID );
		glDeleteProgram( textShader );

		// Clean VAO
		glDeleteVertexArrays( 1, &modelVAO );
		glDeleteVertexArrays( 1, &textVAO );

		delete m_renderTimer;

		delete m_glContext;
	}

	void ModelViewer::clear( ) {
		// Clean VBO
		for ( auto& it : m_vertexBuffer ) {
			if ( it.vertexBuffer ) {
				glDeleteBuffers( 1, &it.vertexBuffer );
			}
			if ( it.uvBuffer ) {
				glDeleteBuffers( 1, &it.uvBuffer );
			}
			if ( it.normalBuffer ) {
				glDeleteBuffers( 1, &it.normalBuffer );
			}
		}

		// Clean IBO
		for ( auto& it : m_indexBuffer ) {
			if ( it.elementBuffer ) {
				glDeleteBuffers( 1, &it.elementBuffer );
			}
		}

		// Clean TBO
		for ( auto& it : m_textureBuffer ) {
			if ( it.diffuseMap ) {
				glDeleteBuffers( 1, &it.diffuseMap );
			}
			if ( it.normalMap ) {
				glDeleteBuffers( 1, &it.normalMap );
			}
			if ( it.lightMap ) {
				glDeleteBuffers( 1, &it.lightMap );
			}
		}

		m_vertexBuffer.clear( );
		m_indexBuffer.clear( );
		m_textureBuffer.clear( );
		m_meshCache.clear( );
		m_model = Model( );
		ViewerGLCanvas::clear( );
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

		// Create mesh cache
		m_meshCache.resize( m_model.numMeshes( ) );

		uint indexBase = 1;
		// Load mesh to mesh cache
		for ( int i = 0; i < static_cast<int>( m_model.numMeshes( ) ); i++ ) {
			auto& mesh = m_model.mesh( i );
			auto& cache = m_meshCache[i];

			if ( !this->loadMeshes( cache, mesh, indexBase ) ) {
				continue;
			}
		}

		// Create Vertex Buffer Object and Index Buffer Object
		m_vertexBuffer.resize( m_model.numMeshes( ) );
		m_indexBuffer.resize( m_model.numMeshes( ) );

		// Populate Buffer Object
		for ( int i = 0; i < static_cast<int>( m_meshCache.size( ) ); i++ ) {
			auto& cache = m_meshCache[i];
			auto& vbo = m_vertexBuffer[i];
			auto& ibo = m_indexBuffer[i];

			// Load mesh to mesh cache
			if ( !this->populateBuffers( vbo, ibo, cache ) ) {
				continue;
			}
		}

		// Load textures to texture map
		std::map<uint32, GLuint> textureMap;
		for ( int i = 0; i < static_cast<int>( m_model.numMaterial( ) ); i++ ) {
			auto& material = m_model.material( i );
			std::map<uint32, GLuint>::iterator it;

			// Load diffuse texture
			if ( material.diffuseMap ) {
				it = textureMap.find( material.diffuseMap );
				if ( it == textureMap.end( ) ) {
					textureMap.insert( std::pair<uint32, GLuint>( material.diffuseMap, this->loadTexture( material.diffuseMap ) ) );
				}
			}

			if ( material.normalMap ) {
				it = textureMap.find( material.normalMap );
				if ( it == textureMap.end( ) ) {
					textureMap.insert( std::pair<uint32, GLuint>( material.normalMap, this->loadTexture( material.normalMap ) ) );
				}
			}

			if ( material.lightMap ) {
				it = textureMap.find( material.lightMap );
				if ( it == textureMap.end( ) ) {
					textureMap.insert( std::pair<uint32, GLuint>( material.lightMap, this->loadTexture( material.lightMap ) ) );
				}
			}
		}

		// Create Texture Buffer Object
		m_textureBuffer.resize( m_model.numMaterial( ) );

		// Copy texture id from texture map to TBO
		for ( int i = 0; i < static_cast<int>( m_model.numMaterial( ) ); i++ ) {
			auto& material = m_model.material( i );
			auto& cache = m_textureBuffer[i];

			// Load diffuse texture
			if ( material.diffuseMap ) {
				cache.diffuseMap = textureMap.find( material.diffuseMap )->second;
			} else {
				cache.diffuseMap = 0;
			}

			if ( material.normalMap ) {
				cache.normalMap = textureMap.find( material.normalMap )->second;
			} else {
				cache.normalMap = 0;
			}

			if ( material.lightMap ) {
				cache.lightMap = textureMap.find( material.lightMap )->second;
			} else {
				cache.lightMap = 0;
			}
		}

		// Clear texture map
		textureMap.clear( );

		// Re-focus and re-render
		this->focus( );
		this->render( );
	}

	int ModelViewer::initGL( ) {
		wxLogMessage( wxT( "Initializing OpenGL..." ) );
		// Create OpenGL context
		m_glContext = new wxGLContext( this );
		if ( !m_glContext ) {
			wxLogMessage( wxT( "Unable to create OpenGL context." ) );
			return false;
		}

		SetCurrent( *m_glContext );

		// Initialize GLEW to setup the OpenGL Function pointers
		glewExperimental = true;
		GLenum err = glewInit( );
		if ( GLEW_OK != err ) {
			wxLogMessage( wxT( "GLEW: Could not initialize GLEW library.\nError : %s" ), wxString( glewGetErrorString( err ) ) );
			return false;
		}

		if ( !GLEW_VERSION_3_3 ) {
			wxLogMessage( wxT( "GLEW: The modelviewer required OpenGL 3.3 support!" ) );
			wxMessageBox( wxT( "GLEW: The modelviewer required OpenGL 3.3 support!" ), wxT( "" ), wxICON_ERROR );
			return false;
		}

		wxLogMessage( wxT( "GLEW version %s" ), wxString( glewGetString( GLEW_VERSION ) ) );
		wxLogMessage( wxT( "Running on a %s from %s" ), wxString( glGetString( GL_RENDERER ) ), wxString( glGetString( GL_VENDOR ) ) );
		wxLogMessage( wxT( "OpenGL version %s" ), wxString( glGetString( GL_VERSION ) ) );

		// Set background color
		glClearColor( 0.21f, 0.21f, 0.21f, 1.0f );

		// Enable multisampling, not really need since it was enabled at context creation
		glEnable( GL_MULTISAMPLE );

		// Enable depth test
		glEnable( GL_DEPTH_TEST );

		// Accept fragment if it closer to the camera than the former one
		glDepthFunc( GL_LESS );

		// Cull triangles which normal is not towards the camera
		//glEnable( GL_CULL_FACE );

		// Generate Vertex Array Object
		glGenVertexArrays( 1, &modelVAO );
		// Bind Vertex Array Object
		glBindVertexArray( modelVAO );

		// Default shader
		if ( !loadShaders( programID, "..//data//shaders//shader.vert", "..//data//shaders//shader.frag" ) ) {
			wxLogMessage( wxT( "Fail to load shaders." ) );
			return false;
		}

		// Text shader
		if ( !loadShaders( textShader, "..//data//shaders//text.vert", "..//data//shaders//text.frag" ) ) {
			wxLogMessage( wxT( "Fail to load text shaders." ) );
			return false;
		}

		FT_UInt fontsize = 12;
		// Load font
		if ( !loadFont( m_characterTextureMap, "..//data//fonts//LiberationSans-Regular.ttf", fontsize ) ) {
			wxLogMessage( wxT( "Fail to load font." ) );
			return false;
		}

		// Get a handle for our "MVP" uniform
		projectionMatrixID = glGetUniformLocation( programID, "projection" );
		viewMatrixID = glGetUniformLocation( programID, "view" );
		modelMatrixID = glGetUniformLocation( programID, "model" );

		// Get a handle for our "myTexture" uniform
		TextureArrayID = glGetUniformLocation( programID, "texture1" );

		// Create dummy texture
		GLubyte blackTextureData[] = { 0, 0, 0, 255 };
		m_dummyBlackTexture = createDummyTexture( blackTextureData );

		GLubyte whiteTextureData[] = { 255, 255, 255, 255 };
		m_dummyWhiteTexture = createDummyTexture( whiteTextureData );

		return true;
	}

	void ModelViewer::onPaintEvt( wxPaintEvent& p_event ) {
		wxPaintDC dc( this );

		this->render( );
	}

	void ModelViewer::render( ) {
		// Set the OpenGL viewport according to the client size of wxGLCanvas.
		wxSize ClientSize = this->GetClientSize( );
		glViewport( 0, 0, ClientSize.x, ClientSize.y );

		// Clear color buffer and depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Use the shader
		glUseProgram( programID );

		// Update view matrix
		this->updateMatrices( );

		if ( m_statusWireframe ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		uint vertexCount = 0;
		uint triangleCount = 0;

		// Model matrix
		glm::mat4 ModelMatrix;

		// Draw each model

		// Draw meshes
		for ( uint i = 0; i < m_model.numMeshes( ); i++ ) {
			this->drawMesh( i );

			vertexCount += m_model.mesh( i ).vertices.size( );
			triangleCount += m_model.mesh( i ).triangles.size( );
		}

		// Transform the model
		ModelMatrix = glm::translate( ModelMatrix, glm::vec3( 0.0f, 0.0f, 0.0f ) );

		// Send model transformation to the currently bound shader
		glUniformMatrix4fv( modelMatrixID, 1, GL_FALSE, glm::value_ptr( ModelMatrix ) );

		if ( m_statusWireframe ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		// Draw status text
		if ( m_statusText ) {
			this->displayStatusText( textShader, vertexCount, triangleCount );
		}

		SwapBuffers( );
	}

	void ModelViewer::drawMesh( const uint p_meshIndex ) {
		auto& vbo = m_vertexBuffer[p_meshIndex];
		auto& ibo = m_indexBuffer[p_meshIndex];
		auto& cache = m_meshCache[p_meshIndex];

		auto materialIndex = m_model.mesh( p_meshIndex ).materialIndex;

		// Use Texture Unit 0
		glActiveTexture( GL_TEXTURE0 );

		if ( m_statusTextured && !m_textureBuffer.empty( ) ) {
			if ( m_statusWireframe ) {
				// Black texture, for wireframe view
				glBindTexture( GL_TEXTURE_2D, m_dummyBlackTexture );
			} else if ( materialIndex >= 0 && m_textureBuffer[materialIndex].diffuseMap ) {
				// "Bind" the texture : all future texture functions will modify this texture
				glBindTexture( GL_TEXTURE_2D, m_textureBuffer[materialIndex].diffuseMap );
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

		// Set our "myTexture" sampler to user Texture Unit 0
		glUniform1i( TextureArrayID, 0 );

		// Vertices attribute
		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, vbo.vertexBuffer );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );

		// UVs attribute
		glEnableVertexAttribArray( 1 );
		glBindBuffer( GL_ARRAY_BUFFER, vbo.uvBuffer );
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );

		// Normals attribute
		glEnableVertexAttribArray( 2 );
		glBindBuffer( GL_ARRAY_BUFFER, vbo.normalBuffer );
		glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );

		// Index buffer
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo.elementBuffer );

		// Draw the triangles!
		glDrawElements( GL_TRIANGLES, cache.indices.size( ), GL_UNSIGNED_INT, ( void* ) 0 );

		// Unbind texture from Texture Unit 0
		glBindTexture( GL_TEXTURE_2D, 0 );

	}

	void ModelViewer::drawText( const GLuint &p_shader, const wxString& p_text, GLfloat p_x, GLfloat p_y, GLfloat p_scale, glm::vec3 p_color ) {
		// Activate corresponding render state
		glUseProgram( p_shader );
		glUniform3f( glGetUniformLocation( textShader, "textColor" ), p_color.x, p_color.y, p_color.z );

		glActiveTexture( GL_TEXTURE0 );
		glBindVertexArray( textVAO );

		// Iterate through all characters
		for ( auto c : p_text ) {
			Character ch = m_characterTextureMap[c];

			GLfloat xpos = p_x + ch.Bearing.x * p_scale;
			GLfloat ypos = p_y - ( ch.Size.y - ch.Bearing.y ) * p_scale;

			GLfloat w = ch.Size.x * p_scale;
			GLfloat h = ch.Size.y * p_scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos, ypos + h, 0.0, 0.0 },
				{ xpos, ypos, 0.0, 1.0 },
				{ xpos + w, ypos, 1.0, 1.0 },

				{ xpos, ypos + h, 0.0, 0.0 },
				{ xpos + w, ypos, 1.0, 1.0 },
				{ xpos + w, ypos + h, 1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture( GL_TEXTURE_2D, ch.TextureID );
			// Update content of Vertex Buffer Object memory
			glBindBuffer( GL_ARRAY_BUFFER, textVBO );
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices ); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			// Render quad
			glDrawArrays( GL_TRIANGLES, 0, 6 );
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			p_x += ( ch.Advance >> 6 ) * p_scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}
		glBindVertexArray( 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	void ModelViewer::displayStatusText( const GLuint &p_shader, const uint p_vertexCount, const uint p_triangleCount ) {
		wxSize ClientSize = this->GetClientSize( );

		// Use text shader
		glUseProgram( p_shader );
		glm::mat4 projection = glm::ortho( 0.0f, static_cast<GLfloat>( ClientSize.x ), 0.0f, static_cast<GLfloat>( ClientSize.y ) );
		glUniformMatrix4fv( glGetUniformLocation( p_shader, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );

		// Enable blending
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glm::vec3 color = glm::vec3( 1.0f );
		GLfloat scale = 1.0f;

		// Top-left text
		this->drawText( p_shader, wxString::Format( wxT( "Meshes: %d" ), m_model.numMeshes( ) ), 0.0f, ClientSize.y - 12.0f, scale, color );
		this->drawText( p_shader, wxString::Format( wxT( "Vertices: %d" ), p_vertexCount ), 0.0f, ClientSize.y - 24.0f, scale, color );
		this->drawText( p_shader, wxString::Format( wxT( "Triangles: %d" ), p_triangleCount ), 0.0f, ClientSize.y - 36.0f, scale, color );

		// Bottom-left text
		this->drawText( p_shader, wxT( "Zoom: Scroll wheel" ), 0.0f, 0.0f + 2.0f, scale, color );
		this->drawText( p_shader, wxT( "Rotate: Left mouse button" ), 0.0f, 12.0f + 2.0f, scale, color );
		this->drawText( p_shader, wxT( "Pan: Right mouse button" ), 0.0f, 24.0f + 2.0f, scale, color );
		this->drawText( p_shader, wxT( "Focus: press F" ), 0.0f, 36.0f + 2.0f, scale, color );
		this->drawText( p_shader, wxT( "Toggle texture: press 3" ), 0.0f, 48.0f + 2.0f, scale, color );
		this->drawText( p_shader, wxT( "Toggle wireframe: press 2" ), 0.0f, 60.0f + 2.0f, scale, color );
		this->drawText( p_shader, wxT( "Toggle status text: press 1" ), 0.0f, 72.0f + 2.0f, scale, color );

		// Disable blending
		glDisable( GL_BLEND );
	}

	bool ModelViewer::loadMeshes( MeshCache& p_cache, const Mesh& p_mesh, uint p_indexBase ) {

		// Tempoarary buffers
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;

		// Read positions
		for ( uint i = 0; i < p_mesh.vertices.size( ); i++ ) {
			auto tempVertices = p_mesh.vertices[i].position;
			temp_vertices.push_back( tempVertices );
		}

		// Read UVs
		if ( p_mesh.hasUV ) {
			for ( uint i = 0; i < p_mesh.vertices.size( ); i++ ) {
				auto tempUvs = p_mesh.vertices[i].uv;
				temp_uvs.push_back( tempUvs );
			}
		}

		// Read normals
		if ( p_mesh.hasNormal ) {
			for ( uint i = 0; i < p_mesh.vertices.size( ); i++ ) {
				auto tempNormals = p_mesh.vertices[i].normal;
				temp_normals.push_back( tempNormals );
			}
		}

		// Tempoarary buffers
		std::vector<uint> vertexIndices, uvIndices, normalIndices;

		// Read faces
		for ( uint i = 0; i < p_mesh.triangles.size( ); i++ ) {
			const Triangle& triangle = p_mesh.triangles[i];

			uint vertexIndex[3], uvIndex[3], normalIndex[3];

			for ( uint j = 0; j < 3; j++ ) {
				uint index = triangle.indices[j] + p_indexBase;

				vertexIndex[j] = index;
				// UV reference
				if ( p_mesh.hasUV ) {
					uvIndex[j] = index;
				}

				// Normal reference
				if ( p_mesh.hasNormal ) {
					normalIndex[j] = index;
				}
			}

			vertexIndices.push_back( vertexIndex[0] );
			vertexIndices.push_back( vertexIndex[1] );
			vertexIndices.push_back( vertexIndex[2] );

			if ( p_mesh.hasUV ) {
				uvIndices.push_back( uvIndex[0] );
				uvIndices.push_back( uvIndex[1] );
				uvIndices.push_back( uvIndex[2] );
			}

			if ( p_mesh.hasNormal ) {
				normalIndices.push_back( normalIndex[0] );
				normalIndices.push_back( normalIndex[1] );
				normalIndices.push_back( normalIndex[2] );
			}
		}
		p_indexBase += p_mesh.vertices.size( );

		// Temporary buffer before send to VBO indexer
		std::vector<glm::vec3> temp2_vertices;
		std::vector<glm::vec2> temp2_uvs;
		std::vector<glm::vec3> temp2_normals;

		// For each vertex of each triangle
		for ( uint i = 0; i < vertexIndices.size( ); i++ ) {
			// Get the indices of its attributes
			uint vertexIndex = vertexIndices[i];
			// Get the indices of its attributes
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			// Put the attributes in buffers
			temp2_vertices.push_back( vertex );

			if ( p_mesh.hasUV ) {
				uint uvIndex = uvIndices[i];
				glm::vec2 uv = temp_uvs[uvIndex - 1];
				temp2_uvs.push_back( uv );
			}

			if ( p_mesh.hasNormal ) {
				uint normalIndex = normalIndices[i];
				glm::vec3 normal = temp_normals[normalIndex - 1];
				temp2_normals.push_back( normal );
			}
		}

		this->indexVBO( temp2_vertices, temp2_uvs, temp2_normals,
			p_cache.indices, p_cache.vertices, p_cache.uvs, p_cache.normals );

		return true;
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

	void ModelViewer::indexVBO(
		const std::vector<glm::vec3>& in_vertices,
		const std::vector<glm::vec2>& in_uvs,
		const std::vector<glm::vec3>& in_normals,

		std::vector<uint>& out_indices,
		std::vector<glm::vec3>& out_vertices,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals ) {

		std::map<PackedVertex, uint> VertexToOutIndex;

		// For each input vertex
		for ( uint i = 0; i < in_vertices.size( ); i++ ) {
			glm::vec3 vertices;
			glm::vec2 uvs;
			glm::vec3 normals;

			vertices = in_vertices[i];

			if ( !in_uvs.empty( ) ) {
				uvs = in_uvs[i];
			}

			if ( !in_normals.empty( ) ) {
				normals = in_normals[i];
			}

			PackedVertex packed = { vertices, uvs, normals };

			// Try to find a similar vertex in out_XXXX
			uint index;
			bool found = getSimilarVertexIndex( packed, VertexToOutIndex, index );

			if ( found ) { // A similar vertex is already in the VBO, use it instead !
				out_indices.push_back( index );
			} else { // If not, it needs to be added in the output data.
				out_vertices.push_back( in_vertices[i] );

				if ( !in_uvs.empty( ) ) {
					out_uvs.push_back( in_uvs[i] );
				}

				if ( !in_normals.empty( ) ) {
					out_normals.push_back( in_normals[i] );
				}
				uint newindex = ( uint ) out_vertices.size( ) - 1;
				out_indices.push_back( newindex );
				VertexToOutIndex[packed] = newindex;
			}
		}
	}

	bool ModelViewer::populateBuffers( VBO& p_vbo, IBO& p_ibo, const MeshCache& p_cache ) {

		glGenBuffers( 1, &p_vbo.vertexBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, p_vbo.vertexBuffer );
		glBufferData( GL_ARRAY_BUFFER, p_cache.vertices.size( ) * sizeof( glm::vec3 ), &p_cache.vertices[0], GL_STATIC_DRAW );

		if ( p_cache.uvs.data( ) ) {
			glGenBuffers( 1, &p_vbo.uvBuffer );
			glBindBuffer( GL_ARRAY_BUFFER, p_vbo.uvBuffer );
			glBufferData( GL_ARRAY_BUFFER, p_cache.uvs.size( ) * sizeof( glm::vec2 ), &p_cache.uvs[0], GL_STATIC_DRAW );
		}

		if ( p_cache.normals.data( ) ) {
			glGenBuffers( 1, &p_vbo.normalBuffer );
			glBindBuffer( GL_ARRAY_BUFFER, p_vbo.normalBuffer );
			glBufferData( GL_ARRAY_BUFFER, p_cache.normals.size( ) * sizeof( glm::vec3 ), &p_cache.normals[0], GL_STATIC_DRAW );
		}

		// Buffer for the indices
		glGenBuffers( 1, &p_ibo.elementBuffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_ibo.elementBuffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, p_cache.indices.size( ) * sizeof( uint ), &p_cache.indices[0], GL_STATIC_DRAW );

		return true;
	}

	GLuint ModelViewer::createDummyTexture( const GLubyte *p_data ) {
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

	GLuint ModelViewer::loadTexture( const uint p_fileId ) {
		auto entryNumber = m_datFile.entryNumFromFileId( p_fileId );
		auto fileData = m_datFile.readEntry( entryNumber );

		// Bail if read failed
		if ( fileData.GetSize( ) == 0 ) {
			return false;
		}

		// Convert to image
		ANetFileType fileType;
		m_datFile.identifyFileType( fileData.GetPointer( ), fileData.GetSize( ), fileType );
		auto reader = FileReader::readerForData( fileData, m_datFile, fileType );

		// Bail if not an image
		auto imgReader = dynamic_cast<ImageReader*>( reader );
		if ( !imgReader ) {
			deletePointer( reader );
			return false;
		}

		// Get image in wxImage
		auto imageData = imgReader->getImage( );

		if ( !imageData.IsOk( ) ) {
			deletePointer( reader );
			return false;
		}

		// wxImage store seperate alpha channel if present
		GLubyte* bitmapData = imageData.GetData( );
		GLubyte* alphaData = imageData.GetAlpha( );

		int imageWidth = imageData.GetWidth( );
		int imageHeight = imageData.GetHeight( );
		int bytesPerPixel = imageData.HasAlpha( ) ? 4 : 3;
		int imageSize = imageWidth * imageHeight * bytesPerPixel;

		Array<GLubyte> image( imageSize );

		// Merge wxImage alpha channel to RGBA
#pragma omp parallel for
		for ( int y = 0; y < imageHeight; y++ ) {
			for ( int x = 0; x < imageWidth; x++ ) {
				image[( x + y * imageWidth ) * bytesPerPixel + 0] = bitmapData[( x + y * imageWidth ) * 3];
				image[( x + y * imageWidth ) * bytesPerPixel + 1] = bitmapData[( x + y * imageWidth ) * 3 + 1];
				image[( x + y * imageWidth ) * bytesPerPixel + 2] = bitmapData[( x + y * imageWidth ) * 3 + 2];

				if ( bytesPerPixel == 4 ) {
					image[( x + y * imageWidth ) * bytesPerPixel + 3] = alphaData[x + y * imageWidth];
				}
			}
		}

		// Generate texture ID
		GLuint TextureID;
		glGenTextures( 1, &TextureID );

		// Assign texture to ID
		glBindTexture( GL_TEXTURE_2D, TextureID );

		// Give the image to OpenGL
		glTexImage2D( GL_TEXTURE_2D,
			0,
			bytesPerPixel,
			imageWidth,
			imageHeight,
			0,
			imageData.HasAlpha( ) ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE,
			image.GetPointer( ) );

		glGenerateMipmap( GL_TEXTURE_2D );

		// Texture parameters

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

		// Trilinear texture filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

		// Anisotropic texture filtering
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8 );

		glBindTexture( GL_TEXTURE_2D, 0 );

		deletePointer( reader );

		return TextureID;
	}

	bool ModelViewer::loadShaders( GLuint& p_programId, const char *p_vertexShaderFilePath, const char *p_fragmentShaderFilePath ) {
		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream( p_vertexShaderFilePath );

		if ( VertexShaderStream.is_open( ) ) {
			std::string Line = "";
			while ( getline( VertexShaderStream, Line ) ) {
				VertexShaderCode += "\n" + Line;
			}
			VertexShaderStream.close( );
		} else {
			wxLogMessage( wxT( "Vertex shader : %s not found." ), p_vertexShaderFilePath );
			return false;
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream( p_fragmentShaderFilePath );
		if ( FragmentShaderStream.is_open( ) ) {
			std::string Line = "";
			while ( getline( FragmentShaderStream, Line ) ) {
				FragmentShaderCode += "\n" + Line;
			}
			FragmentShaderStream.close( );
		} else {
			wxLogMessage( wxT( "Fragment shader : %s not found." ), p_fragmentShaderFilePath );
			return false;
		}

		// Create the shaders
		GLuint VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
		GLuint FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );

		GLint isCompiled = GL_FALSE;
		GLint InfoLogLength = 0;

		// Compile Vertex Shader
		wxLogMessage( wxT( "Compiling vertex shader : %s" ), p_vertexShaderFilePath );
		const GLchar *VertexSourcePointer = VertexShaderCode.c_str( );
		glShaderSource( VertexShaderID, 1, &VertexSourcePointer, NULL );
		glCompileShader( VertexShaderID );

		// Check Vertex Shader
		glGetShaderiv( VertexShaderID, GL_COMPILE_STATUS, &isCompiled );
		if ( isCompiled == GL_FALSE ) {
			glGetShaderiv( VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
			std::vector<GLchar> VertexShaderErrorMessage( glm::max( InfoLogLength, int( 1 ) ) );
			glGetShaderInfoLog( VertexShaderID, InfoLogLength, &InfoLogLength, &VertexShaderErrorMessage[0] );

			glDeleteShader( VertexShaderID );
			wxLogMessage( wxT( "Vertex shader compile error : %s" ), &VertexShaderErrorMessage[0] );
			return false;
		}

		// Compile Fragment Shader
		wxLogMessage( wxT( "Compiling vertex shader : %s" ), p_fragmentShaderFilePath );
		const GLchar *FragmentSourcePointer = FragmentShaderCode.c_str( );
		glShaderSource( FragmentShaderID, 1, &FragmentSourcePointer, NULL );
		glCompileShader( FragmentShaderID );

		// Check Fragment Shader
		glGetShaderiv( FragmentShaderID, GL_COMPILE_STATUS, &isCompiled );
		if ( isCompiled == GL_FALSE ) {
			glGetShaderiv( FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
			std::vector<GLchar> FragmentShaderErrorMessage( glm::max( InfoLogLength, int( 1 ) ) );
			glGetShaderInfoLog( FragmentShaderID, InfoLogLength, &InfoLogLength, &FragmentShaderErrorMessage[0] );

			glDeleteShader( FragmentShaderID );
			wxLogMessage( wxT( "Fragment shader compile error : %s" ), &FragmentShaderErrorMessage[0] );
			return false;
		}

		GLint isLinked = GL_FALSE;

		// Link the program
		wxLogMessage( wxT( "Linking shader program..." ) );
		p_programId = glCreateProgram( );
		glAttachShader( p_programId, VertexShaderID );
		glAttachShader( p_programId, FragmentShaderID );
		glLinkProgram( p_programId );

		// Check the program
		glGetProgramiv( p_programId, GL_LINK_STATUS, &isLinked );
		if ( isLinked == GL_FALSE ) {
			glGetProgramiv( p_programId, GL_INFO_LOG_LENGTH, &InfoLogLength );
			std::vector<GLchar> ProgramErrorMessage( glm::max( InfoLogLength, int( 1 ) ) );
			glGetProgramInfoLog( p_programId, InfoLogLength, NULL, &ProgramErrorMessage[0] );

			glDeleteShader( VertexShaderID );
			glDeleteShader( FragmentShaderID );

			wxLogMessage( wxT( "Shader program linking error : %s" ), &ProgramErrorMessage[0] );
			return false;
		}
		wxLogMessage( wxT( "Done." ) );

		glDeleteShader( VertexShaderID );
		glDeleteShader( FragmentShaderID );

		return true;
	}

	bool ModelViewer::loadFont( std::map<GLchar, Character>& p_characters, const char *p_fontFile, const FT_UInt p_height ) {
		// FreeType
		FT_Library ft;

		// All functions return a value different than 0 whenever an error occurred
		if ( FT_Init_FreeType( &ft ) ) {
			wxLogMessage( wxT( "FreeType: Could not initialize FreeType library." ) );
			return false;
		}

		// Load font as face
		FT_Face face;
		if ( FT_New_Face( ft, p_fontFile, 0, &face ) ) {
			wxLogMessage( wxT( "FreeType: Failed to load font." ) );
			return false;
		}

		// Set size to load glyphs as
		FT_Set_Pixel_Sizes( face, 0, p_height );

		// Disable byte-alignment restriction
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		// Load first 128 characters of ASCII set
		for ( GLubyte c = 0; c < 128; c++ ) {
			// Load character glyph
			if ( FT_Load_Char( face, c, FT_LOAD_RENDER ) ) {
				wxLogMessage( wxT( "FreeType: Failed to load Glyph." ) );
				continue;
			}

			// Generate texture
			GLuint texture;
			glGenTextures( 1, &texture );
			glBindTexture( GL_TEXTURE_2D, texture );
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
				);

			// Set texture options
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			// Now store character for later use
			Character character = {
				texture,
				glm::ivec2( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
				glm::ivec2( face->glyph->bitmap_left, face->glyph->bitmap_top ),
				static_cast<GLuint>( face->glyph->advance.x )
			};
			p_characters.insert( std::pair<GLchar, Character>( c, character ) );
		}
		glBindTexture( GL_TEXTURE_2D, 0 );

		// Destroy FreeType once we're finished
		FT_Done_Face( face );
		FT_Done_FreeType( ft );

		// Configure VAO/VBO for texture quads
		glGenVertexArrays( 1, &textVAO );
		glBindVertexArray( textVAO );

		glGenBuffers( 1, &textVBO );
		glBindBuffer( GL_ARRAY_BUFFER, textVBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * 6 * 4, NULL, GL_DYNAMIC_DRAW );

		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindVertexArray( 0 );

		return true;
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

		//minZ 0.1f
		//maxZ 100.0f

		const wxSize ClientSize = this->GetClientSize( );
		float aspectRatio = ( static_cast<float>( ClientSize.x ) / static_cast<float>( ClientSize.y ) );
		auto fov = ( 5.0f / 12.0f ) * glm::pi<float>( );

		// Projection matrix
		auto ProjMatrix = glm::perspective( fov, aspectRatio, static_cast<float>( minZ ), static_cast<float>( maxZ ) );
		// View matrix
		auto ViewMatrix = m_camera.calculateViewMatrix( );

		// Send our transformation to the currently bound shader
		glUniformMatrix4fv( viewMatrixID, 1, GL_FALSE, glm::value_ptr( ViewMatrix ) );
		glUniformMatrix4fv( projectionMatrixID, 1, GL_FALSE, glm::value_ptr( ProjMatrix ) );
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
		glm::vec3 glmBounds = bounds.center( );
		m_camera.setPivot( glmBounds );

		m_camera.setPivot( bounds.center( ) );
		m_camera.setDistance( distance );
		this->render( );
	}

	void ModelViewer::onMotionEvt( wxMouseEvent& p_event ) {
		if ( m_lastMousePos.x == std::numeric_limits<int>::min( ) &&
			m_lastMousePos.y == std::numeric_limits<int>::min( ) ) {
			m_lastMousePos = p_event.GetPosition( );
		}

		// Yaw/Pitch
		if ( p_event.LeftIsDown( ) ) {
			float rotateSpeed = 0.5f * ( glm::pi<float>( ) / 180.0f );   // 0.5 degrees per pixel
			m_camera.addYaw( rotateSpeed * -( p_event.GetX( ) - m_lastMousePos.x ) );
			m_camera.addPitch( rotateSpeed * ( p_event.GetY( ) - m_lastMousePos.y ) );
			this->render( );
		}

		// Pan
		if ( p_event.RightIsDown( ) ) {
			float xPan = ( p_event.GetX( ) - m_lastMousePos.x );
			float yPan = ( p_event.GetY( ) - m_lastMousePos.y );
			m_camera.pan( xPan, yPan );
			this->render( );
		}

		m_lastMousePos = p_event.GetPosition( );
	}

	void ModelViewer::onMouseWheelEvt( wxMouseEvent& p_event ) {
		float zoomSteps = static_cast<float>( p_event.GetWheelRotation( ) ) / static_cast<float>( p_event.GetWheelDelta( ) );
		m_camera.multiplyDistance( -zoomSteps );
		this->render( );
	}

	void ModelViewer::onKeyDownEvt( wxKeyEvent& p_event ) {
		if ( p_event.GetKeyCode( ) == 'F' ) {
			this->focus( );
		} else if ( p_event.GetKeyCode( ) == '1' ) {
			m_statusText = !m_statusText;
		} else if ( p_event.GetKeyCode( ) == '2' ) {
			m_statusWireframe = !m_statusWireframe;
		} else if ( p_event.GetKeyCode( ) == '3' ) {
			m_statusTextured = !m_statusTextured;
		}
	}

	void ModelViewer::onClose( wxCloseEvent& evt ) {
		m_renderTimer->Stop( );
		evt.Skip( );
	}

}; // namespace gw2b
