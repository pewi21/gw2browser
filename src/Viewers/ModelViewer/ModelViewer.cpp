/* \file       Viewers/ModelViewer.cpp
*  \brief      Contains the definition of the model viewer class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2015 Khral Steelforge <https://github.com/kytulendu>
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

#include "ModelViewer.h"

#include "Readers/ImageReader.h"
#include "DatFile.h"
#include "Data.h"

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

	namespace {

		const int attrib[] = {
			WX_GL_RGBA,
			WX_GL_DOUBLEBUFFER,
			WX_GL_DEPTH_SIZE, 16,
			WX_GL_SAMPLE_BUFFERS, 1,
			WX_GL_SAMPLES, 4,
			0
		};

		const long style = ( wxCLIP_CHILDREN | wxFULL_REPAINT_ON_RESIZE );

		bool doesUseAlpha( const wxString& p_shaderName ) {
			if ( p_shaderName.StartsWith( wxT( "AmatShader" ) ) ) {
				auto ordinal = p_shaderName.Mid( ::strlen( "AmatShader" ) );
				return ( ordinal != "15" ) &&
					( ordinal != "4N" ) &&
					( ordinal != "2" );
			}
			return false;
		}

	};

	ModelViewer::ModelViewer( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
		: ViewerGLCanvas( p_parent, attrib, p_pos, p_size, style )
		, m_lastMousePos( std::numeric_limits<int>::min( ), std::numeric_limits<int>::min( ) ) {

		// Initialize OpenGL
		if ( !m_glInitialized ) {
			if ( !this->initGL( ) ) {
				wxMessageBox( wxT( "initGL( ) Error." ), wxT( "" ), wxICON_ERROR );
				return;
			}
			m_glInitialized = true;
		}

		m_renderTimer = new RenderTimer( this );
		m_renderTimer->start( );

		// Hook up events
		this->Connect( wxEVT_PAINT, wxPaintEventHandler( ModelViewer::onPaintEvt ) );
		this->Connect( wxEVT_MOTION, wxMouseEventHandler( ModelViewer::onMotionEvt ) );
		this->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( ModelViewer::onMouseWheelEvt ) );
		this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ModelViewer::onKeyDownEvt ) );
		this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ModelViewer::onClose ) );
	}

	ModelViewer::~ModelViewer( ) {
		// Clean VBO
		for ( std::vector<VBO>::iterator it = m_vertexBuffer.begin( ); it != m_vertexBuffer.end( ); ++it ) {
			if ( &it->vertexBuffer ) {
				glDeleteBuffers( 1, &it->vertexBuffer );
			}
			if ( &it->uvBuffer ) {
				glDeleteBuffers( 1, &it->uvBuffer );
			}
			if ( &it->normalBuffer ) {
				glDeleteBuffers( 1, &it->normalBuffer );
			}
		}

		// Clean IBO
		for ( std::vector<IBO>::iterator it = m_indexBuffer.begin( ); it != m_indexBuffer.end( ); ++it ) {
			if ( &it->elementBuffer ) {
				glDeleteBuffers( 1, &it->elementBuffer );
			}
		}

		// Clean TBO
		for ( std::vector<TBO>::iterator it = m_textureBuffer.begin( ); it != m_textureBuffer.end( ); ++it ) {
			if ( &it->diffuseMap ) {
				glDeleteBuffers( 1, &it->diffuseMap );
			}
			/*if ( &it->normalMap ) {
				glDeleteBuffers( 1, &it->normalMap );
			}
			if ( &it->lightMap ) {
				glDeleteBuffers( 1, &it->lightMap );
			}*/
		}

		glDeleteBuffers( 1, &dummyBlackTexture );
		glDeleteBuffers( 1, &dummyWhiteTexture );

		glDeleteProgram( programID );
		glDeleteVertexArrays( 1, &VertexArrayID );

		delete m_renderTimer;
		delete m_glContext;
	}

	void ModelViewer::clear( ) {
		// Clean VBO
		for ( std::vector<VBO>::iterator it = m_vertexBuffer.begin( ); it != m_vertexBuffer.end( ); ++it ) {
			if ( &it->vertexBuffer ) {
				glDeleteBuffers( 1, &it->vertexBuffer );
			}
			if ( &it->uvBuffer ) {
				glDeleteBuffers( 1, &it->uvBuffer );
			}
			if ( &it->normalBuffer ) {
				glDeleteBuffers( 1, &it->normalBuffer );
			}
		}

		// Clean IBO
		for ( std::vector<IBO>::iterator it = m_indexBuffer.begin( ); it != m_indexBuffer.end( ); ++it ) {
			if ( &it->elementBuffer ) {
				glDeleteBuffers( 1, &it->elementBuffer );
			}
		}

		// Clean TBO
		for ( std::vector<TBO>::iterator it = m_textureBuffer.begin( ); it != m_textureBuffer.end( ); ++it ) {
			if ( &it->diffuseMap ) {
				glDeleteBuffers( 1, &it->diffuseMap );
			}
			/*if ( &it->normalMap ) {
				glDeleteBuffers( 1, &it->normalMap );
			}
			if ( &it->lightMap ) {
				glDeleteBuffers( 1, &it->lightMap );
			}*/
		}

		m_vertexBuffer.clear( );
		m_indexBuffer.clear( );
		m_textureBuffer.clear( );
		m_meshCache.clear( );
		m_model = Model( );
		ViewerGLCanvas::clear( );
	}

	void ModelViewer::setReader( FileReader* p_reader ) {
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

		// Create VBO and IBO
		m_vertexBuffer.resize( m_model.numMeshes( ) );
		m_indexBuffer.resize( m_model.numMeshes( ) );

		// Populate BO
		for ( int i = 0; i < static_cast<int>( m_meshCache.size( ) ); i++ ) {
			auto& cache = m_meshCache[i];
			auto& vbo = m_vertexBuffer[i];
			auto& ibo = m_indexBuffer[i];

			// Load mesh to mesh cache
			if ( !this->populateBuffers( vbo, ibo, cache ) ) {
				continue;
			}
		}

		// Create TBO
		m_textureBuffer.resize( m_model.numMaterialData( ) );

		// Load textures
		for ( int i = 0; i < static_cast<int>( m_model.numMaterialData( ) ); i++ ) {
			auto& material = m_model.materialData( i );
			auto& cache = m_textureBuffer[i];

			// Load diffuse texture
			if ( material.diffuseMap ) {
				cache.diffuseMap = this->loadTexture( material.diffuseMap );
			} else {
				cache.diffuseMap = 0;
			}
			/*
			if ( material.normalMap ) {
				cache.normalMap = this->loadTexture( material.normalMap );
			} else {
				cache.normalMap = 0;
			}

			if ( material.lightMap ) {
				cache.lightMap = this->loadTexture( material.lightMap );
			} else {
				cache.lightMap = 0;
			}*/
		}

		// Re-focus and re-render
		this->focus( );
		this->render( );
	}

	int ModelViewer::initGL( ) {
		// Create OpenGL context
		m_glContext = new wxGLContext( this );

		SetCurrent( *m_glContext );

		glewExperimental = true;
		GLenum err = glewInit( );
		if ( GLEW_OK != err ) {
			wxString message;
			message << "glewInit( ) failed.\n" << "Error: " << glewGetErrorString( err );
			wxMessageBox( message, wxT( "" ), wxICON_ERROR );
			return false;
		}

		if ( !GLEW_VERSION_3_3 ) {
			wxMessageBox( wxT( "The modelviewer required OpenGL 3.3 support." ), wxT( "" ), wxICON_ERROR );
			return false;
		}

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

		// Generate VAO
		glGenVertexArrays( 1, &VertexArrayID );
		glBindVertexArray( VertexArrayID );

		// Create and compile our GLSL program from the shaders
		if ( !loadShaders( programID, "..//data//shader.vert", "..//data//shader.frag" ) ) {
			wxMessageBox( wxT( "Fail to load shaders." ), wxT( "" ), wxICON_ERROR );
			return false;
		}

		// Load font
		// todo

		// Get a handle for our "MVP" uniform
		MatrixID = glGetUniformLocation( programID, "MVP" );

		// Get a handle for our "myTexture" uniform
		TextureArrayID = glGetUniformLocation( programID, "myTexture" );

		// Create dummy texture
		GLubyte blackTextureData[] = { 0, 0, 0, 255 };
		dummyBlackTexture = createDummyTexture( blackTextureData );

		GLubyte whiteTextureData[] = { 255, 255, 255, 255 };
		dummyWhiteTexture = createDummyTexture( whiteTextureData );

		return true;
	}

	void ModelViewer::paintNow( wxPaintEvent& p_event ) {
		wxPaintDC dc( this );

		this->render( );
	}

	void ModelViewer::onPaintEvt( wxPaintEvent& p_event ) {
		wxPaintDC dc( this );

		this->render( );
	}

	void ModelViewer::render( ) {
		// Set the OpenGL viewport according to the client size of wxGLCanvas.
		const wxSize ClientSize = this->GetClientSize( );
		glViewport( 0, 0, ClientSize.x, ClientSize.y );

		// Clear background
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Use the shader
		glUseProgram( programID );

		this->updateMatrices( );

		if ( m_statusWireframe == true ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		//uint vertexCount = 0;
		//uint triangleCount = 0;

		for ( uint i = 0; i < m_model.numMeshes( ); i++ ) {
			this->drawMesh( i );

			//vertexCount += m_model.mesh( i ).vertices.size( );
			//triangleCount += m_model.mesh( i ).triangles.size( );
		}

		//drawtext

		SwapBuffers( );

		/*
		this->drawText( 0, 0, wxString::Format( wxT( "Meshes: %d" ), m_model.numMeshes( ) ) );
		this->drawText( 0, 0x14, wxString::Format( wxT( "Vertices: %d" ), vertexCount ) );
		this->drawText( 0, 0x28, wxString::Format( wxT( "Triangles: %d" ), triangleCount ) );

		wxSize clientSize = this->GetClientSize( );
		this->drawText( 0, clientSize.y - 0x64, wxT( "Toggle Wireframe: W" ) );
		this->drawText( 0, clientSize.y - 0x50, wxT( "Focus: F button" ) );
		this->drawText( 0, clientSize.y - 0x3c, wxT( "Pan: Middle mouse button" ) );
		this->drawText( 0, clientSize.y - 0x28, wxT( "Rotate: Left mouse button" ) );
		this->drawText( 0, clientSize.y - 0x14, wxT( "Zoom: Scroll wheel" ) );

		wxString diffuseMap = wxString( wxT( "diffuseMap:\n" ) );
		wxString normalMap = wxString( wxT( "normalMap:\n" ) );
		wxString lightMap = wxString( wxT( "lightMap:\n" ) );

		// TODO : Move this else where, no need to update file list each render
		// Texture That need to extract manualy.
		std::vector<uint32> diffuseMapFileList;
		std::vector<uint32> normalMapFileList;
		std::vector<uint32> lightMapFileList;

		for ( uint i = 0; i < m_model.numMaterialData( ); i++ ) {
			auto& material = m_model.materialData( i );

			if ( material.diffuseMap ) {
				diffuseMapFileList.push_back( material.diffuseMap );
			}
			if ( material.normalMap ) {
				normalMapFileList.push_back( material.normalMap );
			}
			if ( material.lightMap ) {
				lightMapFileList.push_back( material.lightMap );
			}
		}

		std::vector<uint32>::iterator temp;

		std::sort( diffuseMapFileList.begin( ), diffuseMapFileList.end( ) );
		temp = std::unique( diffuseMapFileList.begin( ), diffuseMapFileList.end( ) );
		diffuseMapFileList.resize( std::distance( diffuseMapFileList.begin( ), temp ) );

		std::sort( normalMapFileList.begin( ), normalMapFileList.end( ) );
		temp = std::unique( normalMapFileList.begin( ), normalMapFileList.end( ) );
		normalMapFileList.resize( std::distance( normalMapFileList.begin( ), temp ) );

		std::sort( lightMapFileList.begin( ), lightMapFileList.end( ) );
		temp = std::unique( lightMapFileList.begin( ), lightMapFileList.end( ) );
		lightMapFileList.resize( std::distance( lightMapFileList.begin( ), temp ) );

		for ( std::vector<uint32>::iterator it = diffuseMapFileList.begin( ); it != diffuseMapFileList.end( ); ++it ) {
			diffuseMap << *it << "\n";
		}
		for ( std::vector<uint32>::iterator it = normalMapFileList.begin( ); it != normalMapFileList.end( ); ++it ) {
			normalMap << *it << "\n";
		}
		for ( std::vector<uint32>::iterator it = lightMapFileList.begin( ); it != lightMapFileList.end( ); ++it ) {
			lightMap << *it << "\n";
		}

		this->drawText( clientSize.x - 0x11c, 0, wxT( "Textures" ) );

		this->drawText( clientSize.x - 0x4f, 0, wxString( diffuseMap ) );
		this->drawText( clientSize.x - 0x9e, 0, wxString( normalMap ) );
		this->drawText( clientSize.x - 0xdd, 0, wxString( lightMap ) );
		*/
	}

	void ModelViewer::drawMesh( const uint p_meshIndex ) {
		auto& vbo = m_vertexBuffer[p_meshIndex];
		auto& ibo = m_indexBuffer[p_meshIndex];
		auto& cache = m_meshCache[p_meshIndex];

		auto materialIndex = m_model.mesh( p_meshIndex ).materialIndex;

		// Update texture

		// Bind texture to Texture Unit 0
		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D );

		if ( m_statusTextured && !m_textureBuffer.empty( ) ) {
			if ( m_statusWireframe ) {
				// Black texture, for wireframe view
				glBindTexture( GL_TEXTURE_2D, dummyBlackTexture );
			} else if ( materialIndex >= 0 && m_textureBuffer[materialIndex].diffuseMap ) {
				// "Bind" the texture : all future texture functions will modify this texture
				glBindTexture( GL_TEXTURE_2D, m_textureBuffer[materialIndex].diffuseMap );
			}
		} else {
			if ( m_statusWireframe ) {
				// Black texture, for wireframe view
				glBindTexture( GL_TEXTURE_2D, dummyBlackTexture );
			} else {
				// White texture, no texture
				glBindTexture( GL_TEXTURE_2D, dummyWhiteTexture );
			}
		}

		// Set our "myTexture" sampler to user Texture Unit 0
		glUniform1i( TextureArrayID, 0 );

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, vbo.vertexBuffer );
		glVertexAttribPointer(
			0,								// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,								// size
			GL_FLOAT,						// type
			GL_FALSE,						// normalized?
			0,								// stride
			( void* ) 0						// array buffer offset
			);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray( 1 );
		glBindBuffer( GL_ARRAY_BUFFER, vbo.uvBuffer );
		glVertexAttribPointer(
			1,								// attribute. No particular reason for 1, but must match the layout in the shader.
			2,								// size
			GL_FLOAT,						// type
			GL_FALSE,						// normalized?
			0,								// stride
			( void* ) 0						// array buffer offset
			);

		// 3rd attribute buffer : normals
		/*glEnableVertexAttribArray( 2 );
		glBindBuffer( GL_ARRAY_BUFFER, vbo.normalBuffer );
		glVertexAttribPointer(
			2,								// attribute
			3,								// size
			GL_FLOAT,						// type
			GL_FALSE,						// normalized?
			0,								// stride
			( void* ) 0						// array buffer offset
			);*/

		// Index buffer
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo.elementBuffer );

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,					// mode
			cache.indices.size( ),			// indice count
			GL_UNSIGNED_INT,				// type
			( void* ) 0						// element array buffer offset
			);

		// Unbind texture from Texture Unit 0
		glBindTexture( GL_TEXTURE_2D, 0 );

		glDisableVertexAttribArray( 0 );
		glDisableVertexAttribArray( 1 );
		//glDisableVertexAttribArray( 2 );
	}

	// draw text function
	/*
	void ModelViewer::drawText( uint p_x, uint p_y, const wxString& p_text ) {
		if ( m_font.get( ) ) {
			RECT outRect;
			::SetRect( &outRect, p_x, p_y, p_x + 0x200, p_y + 0x14 );
			m_font->DrawTextW( NULL, p_text.wchar_str( ), -1, &outRect, DT_LEFT | DT_NOCLIP, 0xFFFFFFFF );
		}
	}
	*/

	bool ModelViewer::loadMeshes( MeshCache& p_cache, const Mesh& p_mesh, uint p_indexBase ) {
		// Tempoarary buffer
		std::vector<uint> vertexIndices, uvIndices, normalIndices;
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

	bool ModelViewer::getSimilarVertexIndex( PackedVertex & packed, std::map<PackedVertex, uint>& VertexToOutIndex, uint& result ) {
		std::map<PackedVertex, uint>::iterator it = VertexToOutIndex.find( packed );
		if ( it == VertexToOutIndex.end( ) ) {
			return false;
		} else {
			result = it->second;
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

			PackedVertex packed = { in_vertices[i], in_uvs[i]/*, in_normals[i]*/ };

			// Try to find a similar vertex in out_XXXX
			uint index;
			bool found = getSimilarVertexIndex( packed, VertexToOutIndex, index );

			if ( found ) { // A similar vertex is already in the VBO, use it instead !
				out_indices.push_back( index );
			} else { // If not, it needs to be added in the output data.
				out_vertices.push_back( in_vertices[i] );
				out_uvs.push_back( in_uvs[i] );
				//out_normals.push_back( in_normals[i] );
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

		return Texture;
	}

	GLuint ModelViewer::loadTexture( const uint p_fileId ) {
		auto entryNumber = this->datFile( )->entryNumFromFileId( p_fileId );
		auto fileData = this->datFile( )->readEntry( entryNumber );

		// Bail if read failed
		if ( fileData.GetSize( ) == 0 ) {
			return false;
		}

		// Convert to image
		ANetFileType fileType;
		this->datFile( )->identifyFileType( fileData.GetPointer( ), fileData.GetSize( ), fileType );
		auto reader = FileReader::readerForData( fileData, fileType );

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

		// Create one OpenGL texture
		GLuint Texture;
		glGenTextures( 1, &Texture );

		// Trilinear texture filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

		// Anisotropic texture filtering
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8 );

		// "Bind" the newly created texture
		glBindTexture( GL_TEXTURE_2D, Texture );

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

		deletePointer( reader );

		return Texture;
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
			wxString message;
			message << "Vertex shader : " << p_vertexShaderFilePath << " not found.";
			wxMessageBox( message, wxT( "" ), wxICON_ERROR );
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
			wxString message;
			message << "Fragment shader : " << p_fragmentShaderFilePath << " not found.";
			wxMessageBox( message, wxT( "" ), wxICON_ERROR );
			return false;
		}

		// Create the shaders
		GLuint VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
		GLuint FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );

		GLint isCompiled = GL_FALSE;
		GLint InfoLogLength = 0;

		// Compile Vertex Shader
		wxString vertexPath( p_vertexShaderFilePath );
		wxLogDebug( wxT( "Compiling shader : %s" ), vertexPath.utf8_str( ) );
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

			wxLogDebug( wxT( "%s" ), &VertexShaderErrorMessage[0] );
			return false;
		}

		// Compile Fragment Shader
		wxString fragmentPath( p_fragmentShaderFilePath );
		wxLogDebug( wxT( "Compiling shader : %s" ), fragmentPath.utf8_str( ) );
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

			wxLogDebug( wxT( "%s" ), &FragmentShaderErrorMessage[0] );
			return false;
		}

		GLint isLinked = GL_FALSE;

		// Link the program
		wxLogDebug( wxT( "Linking program" ) );
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

			wxLogDebug( wxT( "%s" ), &ProgramErrorMessage[0] );
			return false;
		}
		wxLogDebug( wxT( "Done." ) );

		glDeleteShader( VertexShaderID );
		glDeleteShader( FragmentShaderID );

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

		// Compute the MVP matrix
		const wxSize ClientSize = this->GetClientSize( );
		float aspectRatio = ( static_cast<float>( ClientSize.x ) / static_cast<float>( ClientSize.y ) );
		auto fov = ( 5.0f / 12.0f ) * glm::pi<float>( );

		// Projection matrix
		auto projMatrix = glm::perspective( fov, aspectRatio, minZ, maxZ );

		// View matrix
		auto viewMatrix = m_camera.calculateViewMatrix( );

		// Model matrix
		auto ModelMatrix = glm::mat4( 1.0f );

		MVP = projMatrix * viewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv( MatrixID, 1, GL_FALSE, &MVP[0][0] );
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
		} else if ( p_event.GetKeyCode( ) == 'W' ) {
			m_statusWireframe = !m_statusWireframe;
		} else if ( p_event.GetKeyCode( ) == 'T' ) {
			m_statusTextured = !m_statusTextured;
		}
	}

	void ModelViewer::onClose( wxCloseEvent& evt ) {
		m_renderTimer->Stop( );
		evt.Skip( );
	}

}; // namespace gw2b
