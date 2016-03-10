/* \file       Readers/ModelReader.cpp
*  \brief      Contains the definition of the model reader class.
*  \author     Rhoot
*/

/*
Copyright (C) 2014-2016 Khral Steelforge <https://github.com/kytulendu>
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

#include <new>
#include <vector>

#include <gw2formats/pf/ModelPackFile.h>

#include "ModelReader.h"

#include "DatFile.h"
#include "PackFile.h"

namespace gw2b {

	//----------------------------------------------------------------------------
	//      ModelData
	//----------------------------------------------------------------------------

	ModelData::ModelData( ) {
	}

	ModelData::ModelData( const ModelData& p_other ) {
		meshes.assign( p_other.meshes.begin( ), p_other.meshes.end( ) );
		material.assign( p_other.material.begin( ), p_other.material.end( ) );
	}

	ModelData::~ModelData( ) {
	}

	//----------------------------------------------------------------------------
	//      Model
	//----------------------------------------------------------------------------

	Model::Model( )
		: m_data( new ModelData( ) ) {
	}

	Model::Model( const Model& p_other )
		: m_data( p_other.m_data ) {
	}

	Model::~Model( ) {
	}

	Model& Model::operator=( const Model& p_other ) {
		m_data = p_other.m_data;
		return *this;
	}

	uint Model::numMeshes( ) const {
		return m_data->meshes.size( );
	}

	const Mesh& Model::mesh( uint p_index ) const {
		Assert( p_index < this->numMeshes( ) );
		return m_data->meshes[p_index];
	}

	Mesh* Model::addMeshes( uint p_amount ) {
		this->unShare( );

		uint oldSize = m_data->meshes.size( );
		m_data->meshes.resize( oldSize + p_amount );

		return &( m_data->meshes[oldSize] );
	}

	uint Model::numMaterial( ) const {
		return m_data->material.size( );
	}

	Material& Model::material( uint p_index ) {
		Assert( p_index < this->numMaterial( ) );
		return m_data->material[p_index];
	}

	const Material& Model::material( uint p_index ) const {
		Assert( p_index < this->numMaterial( ) );
		return m_data->material[p_index];
	}

	Material* Model::addMaterial( uint p_amount ) {
		this->unShare( );

		Material emptyData;
		::memset( &emptyData, 0, sizeof( emptyData ) );

		uint oldSize = m_data->material.size( );
		m_data->material.resize( oldSize + p_amount, emptyData );

		return &( m_data->material[oldSize] );
	}

	void Model::unShare( ) {
		if ( m_data->GetRefCount( ) == 1 ) {
			return;
		}
		m_data = new ModelData( *m_data );
	}

	Bounds Model::bounds( ) const {
		// If this model contains meshes, return proper bounds
		if ( m_data->meshes.size( ) ) {
			Bounds retval;
			for ( uint i = 0; i < m_data->meshes.size( ); i++ ) {
				if ( i == 0 ) {
					retval = m_data->meshes[i].bounds;
				} else {
					retval += m_data->meshes[i].bounds;
				}
			}
			return retval;
		}

		// If not, return empty bounds
		Bounds retval;
		::memset( &retval, 0, sizeof( retval ) );
		return retval;
	}


	//----------------------------------------------------------------------------
	//      ModelReader
	//----------------------------------------------------------------------------

	ModelReader::ModelReader( const Array<byte>& p_data, ANetFileType p_fileType )
		: FileReader( p_data, p_fileType ) {
	}

	ModelReader::~ModelReader( ) {
	}

	Model ModelReader::getModel( ) const {
		Model newModel;

		// Bail if there is no data to read
		if ( m_data.GetSize( ) == 0 ) {
			wxLogMessage( wxT( "No data." ) );
			return newModel;
		}

		wxLogMessage( wxT( "Reading model file..." ) );

		gw2f::pf::ModelPackFile modelPackFile( m_data.GetPointer( ), m_data.GetSize( ) );

		//auto animationChunk = model.chunk<gw2f::pf::ModelChunks::Animation>( );
		//auto propertiesChunk = model.chunk<gw2f::pf::ModelChunks::Properties>( );
		//auto rootmotionChunk = model.chunk<gw2f::pf::ModelChunks::RootMotion>( );
		//auto skeletonChunk = model.chunk<gw2f::pf::ModelChunks::Skeleton>( );

		this->readGeometry( newModel, modelPackFile );
		this->readMaterialData( newModel, modelPackFile );

		wxLogMessage( wxT( "Finished reading model file." ) );

		return newModel;
	}

	void ModelReader::readGeometry( Model& p_model, gw2f::pf::ModelPackFile& p_modelPackFile ) const {
		wxLogMessage( wxT( "Reading GOEM chunk..." ) );
		auto geometryChunk = p_modelPackFile.chunk<gw2f::pf::ModelChunks::Geometry>( );

		// Bail if no geometry data
		if ( !geometryChunk ) {
			wxLogMessage( wxT( "No data." ) );
			return;
		}

		uint32 meshCount = geometryChunk->meshes.size( );

		// Bail if no meshes to read
		if ( !meshCount ) {
			wxLogMessage( wxT( "No mesh." ) );
			return;
		}

		wxLogMessage( wxT( "%d mesh(es)." ), meshCount );

		// Create storage for submeshes now, so we can parallelize the loop
		Mesh* meshes = p_model.addMeshes( meshCount );

		auto& meshInfoArray = geometryChunk->meshes;

		uint verticesCount = 0;
		uint trianglesCount = 0;

		clock_t begin = clock( );

#pragma omp parallel for shared( meshes )
		for ( int i = 0; i < static_cast< int >( meshCount ); i++ ) {
			// Fetch mesh info
			auto meshInfo = meshInfoArray[i];

			// Fetch buffer info
			auto vertexInfo = meshInfo.geometry->verts;
			auto indicesInfo = meshInfo.geometry->indices;
			auto vertexCount = vertexInfo.vertexCount;
			auto indiceCount = indicesInfo.indices.size( );

			// Add new mesh
			Mesh& mesh = meshes[i];

			// Material data
			mesh.materialIndex = meshInfo.materialIndex;
			mesh.materialName = wxString::FromUTF8( meshInfo.materialName.data( ) );

			// Vertex data
			if ( vertexCount ) {
				this->readVertexBuffer( mesh, vertexInfo.mesh.vertices.data( ), vertexCount, static_cast< ANetFlexibleVertexFormat >( vertexInfo.mesh.fvf ) );
			}

			// Index data
			if ( indiceCount ) {
				this->readIndexBuffer( mesh, reinterpret_cast< const byte* >( indicesInfo.indices.data( ) ), indiceCount );
				this->computeBond( mesh, reinterpret_cast< const byte* >( indicesInfo.indices.data( ) ), indiceCount );
			}

			if ( mesh.hasNormal ) {
				// Normalize normal
				// Todo: may need to convert to OpenGL coordinate
				//this->normalizeNormals( mesh );
			} else {
				// Calculate the vertex normal if it not exist
				this->computeVertexNormals( mesh );
				mesh.hasNormal = true;
			}

			// Count the vertices and triangles
			verticesCount += vertexCount;
			trianglesCount += indiceCount;
		}
		clock_t end = clock( );
		wxLogMessage( wxT( "Read %d mesh(es) in %f seconds." ), meshCount, ( double( end - begin ) ) / CLOCKS_PER_SEC );
		wxLogMessage( wxT( "%d vertices." ), verticesCount );
		wxLogMessage( wxT( "%d triangles." ), trianglesCount );
	}

	void ModelReader::readVertexBuffer( Mesh& p_mesh, const byte* p_data, uint p_vertexCount, ANetFlexibleVertexFormat p_vertexFormat ) const {
		p_mesh.vertices.resize( p_vertexCount );
		uint vertexSize = this->vertexSize( p_vertexFormat );

		p_mesh.hasNormal = ( ( p_vertexFormat & ANFVF_Normal ) ? 1 : 0 );
		p_mesh.hasUV = ( ( p_vertexFormat & ( ANFVF_UV32Mask | ANFVF_UV16Mask ) ) ? 1 : 0 );

#pragma omp parallel for
		for ( int i = 0; i < static_cast<int>( p_vertexCount ); i++ ) {
			auto pos = &p_data[i * vertexSize];
			Vertex& vertex = p_mesh.vertices[i];
			uint uvIndex = 0;

			// Bit 0: Position
			if ( p_vertexFormat & ANFVF_Position ) {
				glm::vec3 vertices;
				::memcpy( &vertices, pos, sizeof( vertices ) );
				// Convert DirectX coordinate to OpenGL
				vertex.position.x = vertices.x;
				vertex.position.z = 1.0f - vertices.y;
				vertex.position.y = 1.0f - vertices.z;
				pos += sizeof( vertex.position );
			}
			// Bit 1: Weights
			if ( p_vertexFormat & ANFVF_Weights ) {
				pos += 4;
			}
			// Bit 2: Group
			if ( p_vertexFormat & ANFVF_Group ) {
				pos += 4;
			}
			// Bit 3: Normal
			if ( p_vertexFormat & ANFVF_Normal ) {
				// need to test this
				::memcpy( &vertex.normal, pos, sizeof( vertex.normal ) );
				pos += sizeof( vertex.normal );
			}
			// Bit 4: Color
			if ( p_vertexFormat & ANFVF_Color ) {
				pos += sizeof( uint32 );
			}
			// Bit 5: Tangent
			if ( p_vertexFormat & ANFVF_Tangent ) {
				pos += sizeof( glm::vec3 );
			}
			// Bit 6: Bitangent
			if ( p_vertexFormat & ANFVF_Bitangent ) {
				pos += sizeof( glm::vec3 );
			}
			// Bit 7: Tangent frame
			if ( p_vertexFormat & ANFVF_TangentFrame ) {
				pos += sizeof( glm::vec3 );
			}
			// Bit 8-15: 32-bit UV
			uint uvFlag = ( p_vertexFormat & ANFVF_UV32Mask ) >> 8;
			if ( uvFlag ) {
				for ( uint i = 0; i < 7; i++ ) {
					if ( ( ( uvFlag >> i ) & 1 ) == 0 ) {
						continue;
					}
					if ( uvIndex < 1 ) {
						::memcpy( &vertex.uv, pos, sizeof( vertex.uv ) );
					}
					pos += sizeof( vertex.uv );
				}
			}
			// Bit 16-23: 16-bit UV
			uvFlag = ( p_vertexFormat & ANFVF_UV16Mask ) >> 16;
			if ( uvFlag ) {
				for ( uint i = 0; i < 7; i++ ) {
					if ( ( ( uvFlag >> i ) & 1 ) == 0 ) {
						continue;
					}
					if ( uvIndex < 1 ) {
						auto uv = reinterpret_cast<const half*>( pos );
						vertex.uv.x = uv[0];
						vertex.uv.y = uv[1];
						uvIndex++;
					}
					pos += sizeof( half ) * 2;
				}
			}
			// Bit 24: Unknown 48-byte value
			if ( p_vertexFormat & ANFVF_Unknown1 ) {
				pos += 48;
			}
			// Bit 25: Unknown 4-byte value
			if ( p_vertexFormat & ANFVF_Unknown2 ) {
				pos += 4;
			}
			// Bit 26: Unknown 4-byte value
			if ( p_vertexFormat & ANFVF_Unknown3 ) {
				pos += 4;
			}
			// Bit 27: Unknown 16-byte value
			if ( p_vertexFormat & ANFVF_Unknown4 ) {
				pos += 16;
			}
			// Bit 28: Compressed position
			if ( p_vertexFormat & ANFVF_PositionCompressed ) {
				glm::vec3 vertices;
				vertices.x = *reinterpret_cast<const half*>( pos + 0 * sizeof( half ) );
				vertices.y = *reinterpret_cast<const half*>( pos + 1 * sizeof( half ) );
				vertices.z = *reinterpret_cast<const half*>( pos + 2 * sizeof( half ) );
				// Convert DirectX coordinate to OpenGL
				vertex.position.x = vertices.x;
				vertex.position.z = 1.0f - vertices.y;
				vertex.position.y = 1.0f - vertices.z;
				pos += 3 * sizeof( half );
			}
			// Bit 29: Unknown 12-byte value
			if ( p_vertexFormat & ANFVF_Unknown5 ) {
				pos += 12;
			}
		}
	}

	uint ModelReader::vertexSize( ANetFlexibleVertexFormat p_vertexFormat ) const {
		uint uvCount = 0;
		uint uvField = ( p_vertexFormat & ANFVF_UV32Mask ) >> 0x08;
		for ( uint i = 0; i < 7; i++ ) {
			if ( ( ( uvField >> i ) & 1 ) != 0 )
				uvCount++;
		}

		uvField = ( p_vertexFormat & ANFVF_UV16Mask ) >> 0x10;
		uint uv16Count = 0;
		for ( uint i = 0; i < 7; i++ ) {
			if ( ( ( uvField >> i ) & 1 ) != 0 )
				uv16Count++;
		}

		return ( ( p_vertexFormat & ANFVF_Position ) * 12 )
			+ ( ( p_vertexFormat & ANFVF_Weights ) * 2 )
			+ ( ( p_vertexFormat & ANFVF_Group ) )
			+ ( ( ( p_vertexFormat & ANFVF_Normal ) >> 3 ) * 12 )
			+ ( ( p_vertexFormat & ANFVF_Color ) >> 2 )
			+ ( ( ( p_vertexFormat & ANFVF_Tangent ) >> 5 ) * 12 )
			+ ( ( ( p_vertexFormat & ANFVF_Bitangent ) >> 6 ) * 12 )
			+ ( ( ( p_vertexFormat & ANFVF_TangentFrame ) >> 7 ) * 12 )
			+ ( uvCount * 8 )
			+ ( uv16Count * 4 )
			+ ( ( ( p_vertexFormat & ANFVF_Unknown1 ) >> 24 ) * 48 )
			+ ( ( ( p_vertexFormat & ANFVF_Unknown2 ) >> 25 ) * 4 )
			+ ( ( ( p_vertexFormat & ANFVF_Unknown3 ) >> 26 ) * 4 )
			+ ( ( ( p_vertexFormat & ANFVF_Unknown4 ) >> 27 ) * 16 )
			+ ( ( ( p_vertexFormat & ANFVF_PositionCompressed ) >> 28 ) * 6 )
			+ ( ( ( p_vertexFormat & ANFVF_Unknown5 ) >> 29 ) * 12 );

	}

	void ModelReader::computeBond( Mesh& p_mesh, const byte* p_data, uint p_indiceCount ) const {
		// Calculate bounds
		float floatMin = std::numeric_limits<float>::min( );
		float floatMax = std::numeric_limits<float>::max( );
		p_mesh.bounds.min = glm::vec3( floatMax, floatMax, floatMax );
		p_mesh.bounds.max = glm::vec3( floatMin, floatMin, floatMin );

		glm::vec3 min = p_mesh.bounds.min;
		glm::vec3 max = p_mesh.bounds.max;

		auto indices = reinterpret_cast<const uint16*>( p_data );
#pragma omp parallel for
		for ( int i = 0; i < static_cast<int>( p_indiceCount ); i++ ) {
			auto& vertex = p_mesh.vertices[indices[i]];
			glm::vec3 position = vertex.position;
			min = glm::min( min, position );
			max = glm::max( max, position );
		}
		p_mesh.bounds.min = min;
		p_mesh.bounds.max = max;
	}

	void ModelReader::readIndexBuffer( Mesh& p_mesh, const byte* p_data, uint p_indiceCount ) const {
		p_mesh.triangles.resize( p_indiceCount / 3 );
		for ( uint i = 0; i < p_mesh.triangles.size( ); i++ ) {
			auto pos = &p_data[i * sizeof( Triangle )];
			Triangle& triangle = p_mesh.triangles[i];
			// Flip the order of the faces of the triangle
			triangle.index1 = *reinterpret_cast<const uint16*>( pos + 0 * sizeof( uint16 ) );
			triangle.index2 = *reinterpret_cast<const uint16*>( pos + 2 * sizeof( uint16 ) );
			triangle.index3 = *reinterpret_cast<const uint16*>( pos + 1 * sizeof( uint16 ) );
		}
	}

	void ModelReader::computeVertexNormals( Mesh& p_mesh ) const {

		// Claculate vertex normals
		// http://www.iquilezles.org/www/articles/normals/normals.htm

		auto& vert = p_mesh.vertices;
		auto& face = p_mesh.triangles;

		// vert[i].normal is already initialized with zero

		for ( uint i = 0; i < p_mesh.triangles.size( ); i++ ) {
			// Re-flip the order of the faces of the triangle to original order
			const int ia = face[i].index3;
			const int ib = face[i].index2;
			const int ic = face[i].index1;

			const glm::vec3 e1 = vert[ia].position - vert[ib].position;
			const glm::vec3 e2 = vert[ic].position - vert[ib].position;
			const glm::vec3 no = glm::cross( e1, e2 );

			vert[ia].normal += no;
			vert[ib].normal += no;
			vert[ic].normal += no;
		}

		for ( uint i = 0; i < p_mesh.vertices.size( ); i++ ) {
			// Check if the normals is zero to avoid divide by zero problem.
			if ( ( vert[i].normal.x == 0.0f ) || ( vert[i].normal.y == 0.0f ) || ( vert[i].normal.z == 0.0f ) ) {
				if ( vert[i].normal.x ) {
					vert[i].normal.x = glm::normalize( vert[i].normal.x );
				}
				if ( vert[i].normal.y ) {
					vert[i].normal.y = glm::normalize( vert[i].normal.y );
				}
				if ( vert[i].normal.z ) {
					vert[i].normal.z = glm::normalize( vert[i].normal.z );
				}
			} else {
				vert[i].normal = glm::normalize( vert[i].normal );
			}
		}

	}

	void ModelReader::readMaterialData( Model& p_model, gw2f::pf::ModelPackFile& p_modelPackFile ) const {
		wxLogMessage( wxT( "Reading MODL chunk..." ) );
		auto modelChunk = p_modelPackFile.chunk<gw2f::pf::ModelChunks::Model>( );

		// Bail if no data
		if ( !modelChunk ) {
			wxLogMessage( wxT( "No data." ) );
			return;
		}

		// Bail if no data
		if ( !modelChunk->permutations.data( ) ) {
			wxLogMessage( wxT( "No permutations data." ) );
			return;
		}

		// Read some necessary data
		auto materialInfoArray = modelChunk->permutations[0];
		auto materialsArray = materialInfoArray.materials;
		// Count materials
		auto materialCount = materialsArray.size( );

		// Bail if no materials
		if ( !materialCount ) {
			wxLogMessage( wxT( "No material to read." ) );
			return;
		}
		wxLogMessage( wxT( "Have %d material(s)." ), materialCount );

		Material* material = p_model.addMaterial( materialCount );

		// Prepare parallel loop
		std::vector<omp_lock_t> locks( materialCount );
		for ( auto& iter : locks) {
			omp_init_lock( &iter );
		}

		// Loop through each material
#pragma omp parallel for shared(locks, material)
		for ( int j = 0; j < static_cast<int>( materialCount ); j++ ) {
			auto& data = material[j];

			// Only one thread must access this material at a time
			omp_set_lock( &locks[j] );

			// Bail if this material index already has data
			if ( data.materialId && data.materialFlags && data.diffuseMap && data.normalMap && data.lightMap && data.materialFile ) {
				omp_unset_lock( &locks[j] );
				continue;
			}

			// Read material info
			auto materialInfo = materialsArray[j];

			// Copy the data
			// We are (almost) *only* interested in textures
			data.materialId = materialInfo.materialId;
			data.materialFlags = materialInfo.materialFlags;

			auto ref = reinterpret_cast<const ANetFileReference*>( &materialInfo.filename );

			// Get material filename from each materials
			data.materialFile = DatFile::fileIdFromFileReference( *ref );

			if ( materialInfo.textures.size( ) == 0 ) {
				omp_unset_lock( &locks[j] );
				continue;
			}

			auto textures = materialInfo.textures;

			// Out of these, we only care about the diffuse maps
			for ( uint t = 0; t < textures.size( ); t++ ) {
				// Get file reference
				auto fileReference = reinterpret_cast<const ANetFileReference*>( &textures[t].filename );

				// todo: figure out this
				// Diffuse?
				if ( ( textures[t].token & 0xffffffff ) == 0x67531924 ) {
					data.diffuseMap = ( DatFile::fileIdFromFileReference( *fileReference ) + 1 );
				}

				// Normal?
				else if ( ( ( textures[t].token & 0xffffffff ) == 0x1816c9ee )
					/*|| ( ( textures[t].token& 0xffffffff ) == 0x8b0bbd87 )
					|| ( ( textures[t].token& 0xffffffff ) == 0xa55a48b0 )*/ ) {
					data.normalMap = ( DatFile::fileIdFromFileReference( *fileReference ) + 1 );
				}

				// Light Map ?
				else if ( ( textures[t].token & 0xffffffff ) == 0x680bbd87 ) {
					data.lightMap = ( DatFile::fileIdFromFileReference( *fileReference ) + 1 );
					break;
				}
			}

			// Let other threads access this material now that we're done
			omp_unset_lock( &locks[j] );
		}

		// Destroy locks
		for ( auto& iter : locks ) {
			omp_destroy_lock( &iter );
		}
		wxLogMessage( wxT( "Finished reading MODL chunk." ) );
	}

}; // namespace gw2b
