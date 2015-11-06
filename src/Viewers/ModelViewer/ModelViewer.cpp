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

//#include <iostream>
#include <fstream>
#include <string>
//#include <algorithm>
//#include <vector>

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

		const int attrib[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, WX_GL_SAMPLE_BUFFERS, 1, WX_GL_SAMPLES, 4, 0 };
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
		// Cleanup VBO and shader
		glDeleteBuffers( 1, &vertexBuffer );
		glDeleteBuffers( 1, &uvBuffer );
		glDeleteProgram( programID );
		glDeleteTextures( 1, &TextureID );
		glDeleteVertexArrays( 1, &VertexArrayID );

		delete m_renderTimer;
		delete m_glContext;

		/*
		for ( uint i = 0; i < m_meshCache.GetSize( ); i++ ) {
			if ( m_meshCache[i].indexBuffer ) {
				m_meshCache[i].indexBuffer->Release( );
			}
			if ( m_meshCache[i].vertexBuffer ) {
				m_meshCache[i].vertexBuffer->Release( );
			}
		}
		for ( uint i = 0; i < m_textureCache.GetSize( ); i++ ) {
			if ( m_textureCache[i].diffuseMap ) {
				m_textureCache[i].diffuseMap->Release( );
			}
			//if ( m_textureCache[i].normalMap ) {
			//	m_textureCache[i].normalMap->Release( );
			//}
		}
		*/
	}

	void ModelViewer::clear( ) {
		/*
		for ( uint i = 0; i < m_meshCache.GetSize( ); i++ ) {
			if ( m_meshCache[i].indexBuffer ) {
				m_meshCache[i].indexBuffer->Release( );
			}
			if ( m_meshCache[i].vertexBuffer ) {
				m_meshCache[i].vertexBuffer->Release( );
			}
		}
		for ( uint i = 0; i < m_textureCache.GetSize( ); i++ ) {
			if ( m_textureCache[i].diffuseMap ) {
				m_textureCache[i].diffuseMap->Release( );
			}
			//if ( m_textureCache[i].normalMap ) {
			//	m_textureCache[i].normalMap->Release( );
			//}
		}
		*/
		//m_textureCache.Clear( );
		//m_meshCache.Clear( );
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
		m_meshCache.SetSize( m_model.numMeshes( ) );

		// Tempoarary buffer
		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;

		uint indexBase = 1;

		// Load meshes
		for ( uint i = 0; i < m_model.numMeshes( ); i++ ) {
			auto& mesh = m_model.mesh( i );
			auto& cache = m_meshCache[i];

			//mesh.materialName.c_str( );

			// Read positions
			for ( uint j = 0; j < mesh.vertices.GetSize( ); j++ ) {
				glm::vec3 tempVertices;
				tempVertices.x = mesh.vertices[j].position.x;
				tempVertices.y = mesh.vertices[j].position.y;
				tempVertices.z = mesh.vertices[j].position.z;
				temp_vertices.push_back( tempVertices );
			}

			// Read UVs
			if ( mesh.hasUV ) {
				for ( uint j = 0; j < mesh.vertices.GetSize( ); j++ ) {
					glm::vec2 tempUvs;
					tempUvs.x = mesh.vertices[j].uv.x;
					tempUvs.y = mesh.vertices[j].uv.y;
					temp_uvs.push_back( tempUvs );
				}
			}

			// Read normals
			if ( mesh.hasNormal ) {
				for ( uint j = 0; j < mesh.vertices.GetSize( ); j++ ) {
					temp_normals.push_back( mesh.vertices[j].normal );
				}
			}

			// Write faces
			// Index ?
			for ( uint j = 0; j < mesh.triangles.GetSize( ); j++ ) {
				const Triangle& triangle = mesh.triangles[j];

				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

				for ( uint k = 0; k < 3; k++ ) {
					uint index = triangle.indices[k] + indexBase;;

					vertexIndex[k] = index;
					// UV reference
					if ( mesh.hasUV ) {
						uvIndex[k] = index;
					}

					// Normal reference
					if ( mesh.hasNormal ) {
						normalIndex[k] = index;
					}
				}

				vertexIndices.push_back( vertexIndex[0] );
				vertexIndices.push_back( vertexIndex[1] );
				vertexIndices.push_back( vertexIndex[2] );
				uvIndices.push_back( uvIndex[0] );
				uvIndices.push_back( uvIndex[1] );
				uvIndices.push_back( uvIndex[2] );
				normalIndices.push_back( normalIndex[0] );
				normalIndices.push_back( normalIndex[1] );
				normalIndices.push_back( normalIndex[2] );
			}
			indexBase += mesh.vertices.GetSize( );
		}

		std::vector<glm::vec3> out_vertices;
		std::vector<glm::vec2> out_uvs;
		std::vector<glm::vec3> out_normals;

		// For each vertex of each triangle
		for ( unsigned int i = 0; i < vertexIndices.size( ); i++ ) {

			// Get the indices of its attributes
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int uvIndex = uvIndices[i];
			unsigned int normalIndex = normalIndices[i];

			// Get the attributes thanks to the index
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			glm::vec2 uv = temp_uvs[uvIndex - 1];
			glm::vec3 normal;
			// Normal reference
			if ( m_model.mesh(1).hasNormal ) {
				glm::vec3 normal = temp_normals[normalIndex - 1];
			}

			// Put the attributes in buffers
			out_vertices.push_back( vertex );
			out_uvs.push_back( uv );
			out_normals.push_back( normal );
		}


		vertsize = out_vertices.size( );

		glGenBuffers( 1, &vertexBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
		// Give our vertices to OpenGL.
		glBufferData( GL_ARRAY_BUFFER, out_vertices.size( ) * sizeof( glm::vec3 ), &out_vertices[0], GL_STATIC_DRAW );

		glGenBuffers( 1, &uvBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, uvBuffer );
		glBufferData( GL_ARRAY_BUFFER, out_uvs.size( ) * sizeof( glm::vec2 ), &out_uvs[0], GL_STATIC_DRAW );

		glGenBuffers( 1, &normalBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, normalBuffer );
		glBufferData( GL_ARRAY_BUFFER, out_normals.size( ) * sizeof( glm::vec3 ), &out_normals[0], GL_STATIC_DRAW );

		// Create DX texture cache
		//m_textureCache.SetSize( m_model.numMaterialData( ) );

		// Load textures
		Texture = loadTexture( 13845 );
		/*
		for ( uint i = 0; i < m_model.numMaterialData( ); i++ ) {
			auto& material = m_model.materialData( i );
			auto& cache = m_textureCache[i];

			// Load diffuse texture
			if ( material.diffuseMap ) {
				cache.diffuseMap = this->loadTexture( material.diffuseMap );
			} else {
				cache.diffuseMap = nullptr;
			}

			// Load normal map
			//if ( material.normalMap ) {
			//	cache.normalMap = this->loadTexture( material.normalMap );
			//} else {
			//	cache.normalMap = nullptr;
			//}
		}
		*/
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

		// Set clear background color to blue
		glClearColor( 0.3f, 0.4f, 0.6f, 1.0f );

		// Enable depth test
		glEnable( GL_DEPTH_TEST );

		// Accept fragment if it closer to the camera than the former one
		//glDepthFunc( GL_LESS );

		// Cull triangles which normal is not towards the camera
		//glEnable( GL_CULL_FACE );

		// Generate VAO
		glGenVertexArrays( 1, &VertexArrayID );
		glBindVertexArray( VertexArrayID );

		// Todo : write shader manager
		// Create and compile our GLSL program from the shaders
		programID = loadShaders( "..//data//shader.vert", "..//data//shader.frag" );
		//wxMessageBox( Error 0x%08x while loading shader: %s" )

		// Load font

		// Get a handle for our "MVP" uniform
		MatrixID = glGetUniformLocation( programID, "MVP" );

		// Get a handle for our "myTextureSampler" uniform
		TextureID = glGetUniformLocation( programID, "myTextureSampler" );

		return true;
	}

	//createBuffers
	/*
	bool ModelViewer::createBuffers( MeshCache& p_cache, uint p_vertexCount, uint p_vertexSize, uint p_indexCount, uint p_indexSize ) {
		p_cache.indexBuffer = nullptr;
		p_cache.vertexBuffer = nullptr;

		// 0 indices or 0 vertices, either is an empty mesh
		if ( !p_vertexCount || !p_indexCount ) {
			return false;
		}

		// Allocate vertex buffer and bail if it fails
		if ( FAILED( m_device->CreateVertexBuffer( p_vertexCount * p_vertexSize, D3DUSAGE_WRITEONLY, c_vertexFVF,
			D3DPOOL_DEFAULT, &p_cache.vertexBuffer, nullptr ) ) ) {
			return false;
		}

		// Allocate index buffer and bail if it fails
		if ( FAILED( m_device->CreateIndexBuffer( p_indexCount * p_indexSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
			D3DPOOL_DEFAULT, &p_cache.indexBuffer, nullptr ) ) ) {
			p_cache.vertexBuffer->Release( );
			p_cache.vertexBuffer = nullptr;
			p_cache.indexBuffer = nullptr;
			return false;
		}

		return true;
	}
	*/
	//populateBuffers
	/*
	bool ModelViewer::populateBuffers( const Mesh& p_mesh, MeshCache& p_cache ) {
		uint vertexCount = p_mesh.vertices.GetSize( );
		uint vertexSize = sizeof( Vertex );
		uint indexCount = p_mesh.triangles.GetSize( ) * 3;
		uint indexSize = sizeof( uint16 );

		// Lock vertex buffer
		Vertex* vertices;
		if ( FAILED( p_cache.vertexBuffer->Lock( 0, vertexCount * vertexSize, reinterpret_cast<void**>( &vertices ), 0 ) ) ) {
			return false;
		}

		// Populate vertex buffer
		std::copy_n( &p_mesh.vertices[0], vertexCount, &vertices[0] );

		// Unlock vertex buffer
		Assert( SUCCEEDED( p_cache.vertexBuffer->Unlock( ) ) );

		// Lock index buffer
		uint16* indices;
		if ( FAILED( p_cache.indexBuffer->Lock( 0, indexCount * indexSize, reinterpret_cast<void**>( &indices ), 0 ) ) ) {
			return false;
		}
		// Copy index buffer
		std::copy_n( &p_mesh.triangles[0].index1, indexCount, &indices[0] );
		Assert( SUCCEEDED( p_cache.indexBuffer->Unlock( ) ) );

		return true;
	}
	*/

	void ModelViewer::render( ) {
		// Set the OpenGL viewport according to the client size of wxGLCanvas.
		const wxSize ClientSize = this->GetClientSize( );
		glViewport( 0, 0, ClientSize.x, ClientSize.y );

		// All models are located at 0,0,0 with no rotation, so no world matrix is needed
		// Calculate minZ/maxZ
		auto bounds = m_model.bounds( );
		auto size = bounds.size( );
		auto distance = m_camera.distance( );
		auto extents = glm::vec3( size.x * 0.5f, size.y * 0.5f, size.z * 0.5f );

		auto maxSize = ::sqrt( extents.x * extents.x + extents.y * extents.y + extents.z * extents.z );
		auto maxZ = ( maxSize + distance ) * 1.1f;
		auto minZ = maxZ * 0.0001f;

		// View matrix
		auto viewMatrix = m_camera.calculateViewMatrix( );

		// Projection matrix
		float aspectRatio = ( static_cast<float>( ClientSize.x ) / static_cast<float>( ClientSize.y ) );
		// Initial Field of View
		auto fov = ( 5.0f / 12.0f ) * glm::pi<float>( );
		auto projMatrix = glm::perspective( fov, aspectRatio, minZ, maxZ );

		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 ModelMatrix = glm::mat4( 1.0f );
		//glm::mat4 ModelMatrix = glm::scale( glm::mat4( 1.0f ), glm::vec3( 0.5f ) );

		// ModelViewProjection : multiplication of our 3 matrices
		MVP = projMatrix * viewMatrix;//* ModelMatrix;





		// Clear background to blue
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Use the shader
		glUseProgram( programID );

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv( MatrixID, 1, GL_FALSE, &MVP[0][0] );

		// Bind our texture in Texture Unit 0
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, Texture );
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i( TextureID, 0 );

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			( void* ) 0         // array buffer offset
			);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray( 1 );
		glBindBuffer( GL_ARRAY_BUFFER, uvBuffer );
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			( void* ) 0                          // array buffer offset
			);

		if ( m_statusWireframe == true ) {
			glDrawArrays( GL_LINES, 0, vertsize );
		} else {
			// Draw the triangle !
			glDrawArrays( GL_TRIANGLES, 0, vertsize ); // change this
		}

		glDisableVertexAttribArray( 0 );
		glDisableVertexAttribArray( 1 );

		SwapBuffers( );

		/*
		this->updateMatrices( );

		uint vertexCount = 0;
		uint triangleCount = 0;

		this->beginFrame( 0x353535 );
		for ( uint i = 0; i < m_model.numMeshes( ); i++ ) {
			this->drawMesh( i );
			vertexCount += m_model.mesh( i ).vertices.GetSize( );
			triangleCount += m_model.mesh( i ).triangles.GetSize( );
		}

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

		this->endFrame( );
		*/
	}

	void ModelViewer::paintNow( wxPaintEvent& p_event ) {
		wxPaintDC dc( this );

		this->render( );
	}

	void ModelViewer::onPaintEvt( wxPaintEvent& p_event ) {
		wxPaintDC dc( this );

		this->render( );
	}

	void ModelViewer::onClose( wxCloseEvent& evt ) {
		m_renderTimer->Stop( );
		evt.Skip( );
	}
	/*
	void ModelViewer::drawMesh( uint p_meshIndex ) {
		auto& mesh = m_meshCache[p_meshIndex];

		// No mesh to draw?
		if ( mesh.indexBuffer == nullptr || mesh.vertexBuffer == nullptr ) {
			return;
		}

		// Count vertices / primitives
		uint vertexCount = m_model.mesh( p_meshIndex ).vertices.GetSize( );
		uint primitiveCount = m_model.mesh( p_meshIndex ).triangles.GetSize( );
		int  materialIndex = m_model.mesh( p_meshIndex ).materialIndex;

		// Set buffers
		if ( FAILED( m_device->SetFVF( c_vertexFVF ) ) ) {
			return;
		}
		if ( FAILED( m_device->SetStreamSource( 0, mesh.vertexBuffer, 0, sizeof( Vertex ) ) ) ) {
			return;
		}
		if ( FAILED( m_device->SetIndices( mesh.indexBuffer ) ) ) {
			return;
		}

		// Alpha blending, for alpha support
		if ( doesUseAlpha( m_model.mesh( p_meshIndex ).materialName ) ) {
			m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			m_device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			// Alpha testing, so we can still render behind transparent pixels
			D3DCAPS9 caps;
			m_device->GetDeviceCaps( &caps );
			if ( caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL ) {
				m_device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				m_device->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				m_device->SetRenderState( D3DRS_ALPHAREF, 0x7f );
			}
		} else {
			m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			m_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		}

		// Begin drawing
		uint numPasses;
		m_effect->SetTechnique( "RenderScene" );
		m_effect->Begin( &numPasses, 0 );

		// Update texture
		if ( materialIndex >= 0 && m_textureCache[materialIndex].diffuseMap ) {
			m_effect->SetTexture( "g_DiffuseTex", m_textureCache[materialIndex].diffuseMap );
		}

		// Draw each shader pass
		for ( uint i = 0; i < numPasses; i++ ) {
			m_effect->BeginPass( i );
			m_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, primitiveCount );
			m_effect->EndPass( );
		}

		// End
		m_effect->End( );
	}
	*/

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

	GLuint ModelViewer::loadShaders( const char *vertex_file_path, const char *fragment_file_path ) {
		// Create the shaders
		GLuint VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
		GLuint FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream( vertex_file_path );

		if ( VertexShaderStream.is_open( ) ) {
			std::string Line = "";
			while ( getline( VertexShaderStream, Line ) ) {
				VertexShaderCode += "\n" + Line;
			}
			VertexShaderStream.close( );
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream( fragment_file_path );
		if ( FragmentShaderStream.is_open( ) ) {
			std::string Line = "";
			while ( getline( FragmentShaderStream, Line ) ) {
				FragmentShaderCode += "\n" + Line;
			}
			FragmentShaderStream.close( );
		}

		GLint isCompiled = GL_FALSE;
		GLint InfoLogLength = 0;

		// Compile Vertex Shader
		wxString vertexPath( vertex_file_path );
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
			//return;
		}

		// Compile Fragment Shader
		wxString fragmentPath( fragment_file_path );
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
			//return;
		}

		GLint isLinked = GL_FALSE;

		// Link the program
		wxLogDebug( wxT( "Linking program" ) );
		GLuint ProgramID = glCreateProgram( );
		glAttachShader( ProgramID, VertexShaderID );
		glAttachShader( ProgramID, FragmentShaderID );
		glLinkProgram( ProgramID );

		// Check the program
		glGetProgramiv( ProgramID, GL_LINK_STATUS, &isLinked );
		if ( isLinked == GL_FALSE ) {
			glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength );
			std::vector<GLchar> ProgramErrorMessage( glm::max( InfoLogLength, int( 1 ) ) );
			glGetProgramInfoLog( ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0] );

			glDeleteShader( VertexShaderID );
			glDeleteShader( FragmentShaderID );

			wxLogDebug( wxT( "%s" ), &ProgramErrorMessage[0] );
			//return;
		}
		wxLogDebug( wxT( "Done" ) );

		glDeleteShader( VertexShaderID );
		glDeleteShader( FragmentShaderID );

		return ProgramID;
	}

	GLuint ModelViewer::loadTexture( uint p_fileId ) {
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

		// Load texture to OpenGL

		// Create one OpenGL texture
		GLuint textureID;
		glGenTextures( 1, &textureID );

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture( GL_TEXTURE_2D, textureID );

		// Give the image to OpenGL
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imageData.GetWidth( ), imageData.GetHeight( ), 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.GetData( ) );

		// Poor filtering, or ...
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Trilinear filtering.
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glGenerateMipmap( GL_TEXTURE_2D );

		deletePointer( reader );

		return textureID;
	}

	void ModelViewer::focus( ) {
		float fov = ( 5.0f / 12.0f ) * glm::pi<float>( );
		uint meshCount = m_model.numMeshes( );

		if ( !meshCount ) {
			return;
		}

		// Calculate complete bounds
		Bounds bounds = m_model.bounds( );
		float height = bounds.max.z - bounds.min.z;
		if ( height <= 0 ) {
			return;
		}

		float distance = bounds.min.y - ( ( height * 0.5f ) / ::tanf( fov * 0.5f ) );
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
			float xPan = -( p_event.GetX( ) - m_lastMousePos.x );
			float yPan = -( p_event.GetY( ) - m_lastMousePos.y );
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
		}
	}

}; // namespace gw2b
