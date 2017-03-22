/* \file       Viewers/ModelViewer/Model.cpp
*  \brief      Contains the declaration of the model class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2017 Khral Steelforge <https://github.com/kytulendu>

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

#include "Exception.h"

#include "Model.h"

namespace gw2b {

	Model::Model( const GW2Model& p_model, DatFile& p_datFile )
		: m_datFile( p_datFile )
		, m_numMeshes( 0 )
		, m_numVertices( 0 )
		, m_numTriangles( 0 )
		, m_bounds( p_model.bounds( ) ) {
		this->loadModel( p_model );
		this->loadMaterial( p_model );
	}

	Model::~Model( ) {
		this->clearBuffer( );

		m_vertexBuffer.clear( );
		m_indexBuffer.clear( );
		m_textureList.clear( );
		m_textureMap.clear( );
		m_meshCache.clear( );
	}

	void Model::draw( Shader* p_shader ) {
		for ( uint i = 0; i < static_cast<uint>( m_numMeshes ); i++ ) {
			this->drawMesh( p_shader, i );
		}
	}

	size_t Model::getNumMeshes( ) const {
		return m_numMeshes;
	}

	size_t Model::getVertSize( ) const {
		return m_numVertices;
	}

	size_t Model::getTriSize( ) const {
		return m_numTriangles;
	}

	Bounds Model::getBounds( ) const {
		return m_bounds;
	}

	void Model::clearBuffer( ) {
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

		// Clean VAO
		for ( auto& it : m_vertexArray ) {
			if ( it.vertexArray ) {
				glDeleteVertexArrays( 1, &it.vertexArray );
			}
		}
	}

	void Model::drawMesh( Shader* p_shader, const uint p_meshIndex ) {
		auto& vao = m_vertexArray[p_meshIndex];
		auto& cache = m_meshCache[p_meshIndex];

		auto materialIndex = m_meshCache[p_meshIndex].materialIndex;

		// Texture Maping
		if ( !m_textureList.empty( ) ) {
			// Use Texture Unit 0
			glActiveTexture( GL_TEXTURE0 );

			if ( materialIndex >= 0 && m_textureList[materialIndex].diffuseMap ) {
				auto texture = m_textureMap.find( m_textureList[materialIndex].diffuseMap );
				if ( texture != m_textureMap.end( ) ) {
					texture->second->bind( );
				}
			}

			// Set our "diffuseMap" sampler to user Texture Unit 0
			glUniform1i( glGetUniformLocation( p_shader->getProgramId( ), "material.diffuseMap" ), 0 );

			// Bind our normal texture in Texture Unit 1
			glActiveTexture( GL_TEXTURE1 );

			if ( materialIndex >= 0 && m_textureList[materialIndex].normalMap ) {
				auto texture = m_textureMap.find( m_textureList[materialIndex].normalMap );
				if ( texture != m_textureMap.end( ) ) {
					texture->second->bind( );
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

	}

	void Model::loadModel( const GW2Model& p_model ) {
		m_numMeshes = p_model.numMeshes( );

		// Create mesh cache
		m_meshCache.resize( m_numMeshes );

		// Load mesh to mesh cache
#pragma omp parallel for
		for ( int i = 0; i < static_cast<int>( m_numMeshes ); i++ ) {
			auto& mesh = p_model.mesh( i );
			auto& cache = m_meshCache[i];

			m_numVertices += mesh.vertices.size( );
			m_numTriangles += mesh.triangles.size( );

			cache.materialIndex = mesh.materialIndex;

			this->loadMesh( cache, mesh );
		}

		// Create Vertex Array Object, Vertex Buffer Object and Index Buffer Object
		m_vertexArray.resize( m_numMeshes );
		m_vertexBuffer.resize( m_numMeshes );
		m_indexBuffer.resize( m_numMeshes );

		// Populate Buffer Object
		for ( uint i = 0; i < m_meshCache.size( ); i++ ) {
			auto& cache = m_meshCache[i];
			auto& vao = m_vertexArray[i];
			auto& vbo = m_vertexBuffer[i];
			auto& ibo = m_indexBuffer[i];

			this->populateBuffers( vao, vbo, ibo, cache );
		}

	}

	void Model::loadMesh( MeshCache& p_cache, const GW2Mesh& p_mesh ) {
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

	void Model::computeTangent( MeshCache& p_mesh ) {
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

	bool Model::getSimilarVertexIndex( PackedVertex& p_packed, std::map<PackedVertex, uint>& p_vertexToOutIndex, uint& p_result ) {
		auto it = p_vertexToOutIndex.find( p_packed );
		if ( it == p_vertexToOutIndex.end( ) ) {
			return false;
		} else {
			p_result = it->second;
			return true;
		}
	}

	void Model::indexVBO( const MeshCache& p_inMesh, MeshCache& p_outMesh ) {
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

	void Model::populateBuffers( VAO& p_vao, VBO& p_vbo, IBO& p_ibo, const MeshCache& p_cache ) {
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

	void Model::loadMaterial( const GW2Model& p_model ) {
		auto& texture = m_textureMap;
		auto& material = p_model.material( );
		auto numMaterial = p_model.numMaterial( );

		// Load textures to texture map
		for ( auto& mat : material ) {
			std::unordered_map<uint32, texture_ptr>::iterator it;

			// Load diffuse texture
			if ( mat.diffuseMap ) {
				it = texture.find( mat.diffuseMap );
				if ( it == texture.end( ) ) {
					try {
						texture.insert( std::pair<uint32, texture_ptr>( mat.diffuseMap, texture_ptr( new Texture2D( m_datFile, mat.diffuseMap ) ) ) );
					} catch ( const exception::Exception& err ) {
						wxLogMessage( wxT( "Failed to load texture %d : %s" ), mat.diffuseMap, wxString( err.what( ) ) );
					}
				}
			}

			if ( mat.normalMap ) {
				it = texture.find( mat.normalMap );
				if ( it == texture.end( ) ) {
					try {
						texture.insert( std::pair<uint32, texture_ptr>( mat.normalMap, texture_ptr( new Texture2D( m_datFile, mat.normalMap ) ) ) );
					} catch ( const exception::Exception& err ) {
						wxLogMessage( wxT( "Failed to load texture %d : %s" ), mat.normalMap, wxString( err.what( ) ) );
					}
				}
			}

			if ( mat.lightMap ) {
				it = texture.find( mat.lightMap );
				if ( it == texture.end( ) ) {
					try {
						texture.insert( std::pair<uint32, texture_ptr>( mat.lightMap, texture_ptr( new Texture2D( m_datFile, mat.lightMap ) ) ) );
					} catch ( const exception::Exception& err ) {
						wxLogMessage( wxT( "Failed to load texture %d : %s" ), mat.lightMap, wxString( err.what( ) ) );
					}
				}
			}
		}

		// Create Texture Buffer Object
		m_textureList.resize( numMaterial );

		// Copy texture information from material of GW2Model to m_textureList
		for ( size_t i = 0; i < numMaterial; ++i ) {
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

}; // namespace gw2b
