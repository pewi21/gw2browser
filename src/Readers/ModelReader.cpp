/* \file       Readers/ModelReader.cpp
*  \brief      Contains the definition of the model reader class.
*  \author     Rhoot
*/

/*
Copyright (C) 2014-2015 Khral Steelforge <https://github.com/kytulendu>
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

#include <wx/sstream.h>
#include <wx/txtstrm.h>
#include <sstream>
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
		materialData.assign( p_other.materialData.begin( ), p_other.materialData.end( ) );
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

	uint Model::numMaterialData( ) const {
		return m_data->materialData.size( );
	}

	MaterialData& Model::materialData( uint p_index ) {
		Assert( p_index < this->numMaterialData( ) );
		return m_data->materialData[p_index];
	}

	const MaterialData& Model::materialData( uint p_index ) const {
		Assert( p_index < this->numMaterialData( ) );
		return m_data->materialData[p_index];
	}

	MaterialData* Model::addMaterialData( uint p_amount ) {
		this->unShare( );

		MaterialData emptyData;
		::memset( &emptyData, 0, sizeof( emptyData ) );

		uint oldSize = m_data->materialData.size( );
		m_data->materialData.resize( oldSize + p_amount, emptyData );

		return &( m_data->materialData[oldSize] );
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

	Array<byte> ModelReader::convertData( ) const {
		Model model = this->getModel( );
		std::ostringstream stream;

		// Note: wxWidgets only does locale-specific number formatting. This does
		// not work well with obj-files.
		stream.imbue( std::locale( "C" ) );
		stream << "# " << model.numMeshes( ) << " meshes" << std::endl;

		uint indexBase = 1;
		for ( uint i = 0; i < model.numMeshes( ); i++ ) {
			const Mesh& mesh = model.mesh( i );

			// Write header
			stream << std::endl << "# Mesh " << i << ": " << mesh.vertices.GetSize( ) << " vertices, " << mesh.triangles.GetSize( ) << " triangles" << std::endl;
			stream << "g mesh" << i << std::endl;
			stream << "usemtl " << mesh.materialName.c_str( ) << std::endl;

			// Write positions
			for ( uint j = 0; j < mesh.vertices.GetSize( ); j++ ) {
				stream << "v " << mesh.vertices[j].position.x << ' ' << mesh.vertices[j].position.y << ' ' << mesh.vertices[j].position.z << std::endl;
			}

			// Write UVs
			if ( mesh.hasUV ) {
				for ( uint j = 0; j < mesh.vertices.GetSize( ); j++ ) {
					stream << "vt " << mesh.vertices[j].uv.x << ' ' << mesh.vertices[j].uv.y << std::endl;
				}
			}

			// Write normals
			if ( mesh.hasNormal ) {
				for ( uint j = 0; j < mesh.vertices.GetSize( ); j++ ) {
					stream << "vn " << mesh.vertices[j].normal.x << ' ' << mesh.vertices[j].normal.y << ' ' << mesh.vertices[j].normal.z << std::endl;
				}
			}

			// Write faces
			for ( uint j = 0; j < mesh.triangles.GetSize( ); j++ ) {
				const Triangle& triangle = mesh.triangles[j];

				stream << 'f';
				for ( uint k = 0; k < 3; k++ ) {
					uint index = triangle.indices[k] + indexBase;
					stream << ' ' << index;

					// UV reference
					if ( mesh.hasUV ) {
						stream << '/' << index;
					} else if ( mesh.hasNormal ) {
						stream << '/';
					}

					// Normal reference
					if ( mesh.hasNormal ) {
						stream << '/' << index;
					}
				}
				stream << std::endl;
			}

			// newline before next mesh!
			stream << std::endl;
			indexBase += mesh.vertices.GetSize( );
		}

		// Close stream
		stream.flush( );
		std::string output = stream.str( );
		stream.clear( );

		// Convert string to byte array
		Array<byte> outputData( output.length( ) );
		::memcpy( outputData.GetPointer( ), output.c_str( ), output.length( ) );

		return outputData;
	}

	Model ModelReader::getModel( ) const {
		Model newModel;

		// Bail if there is no data to read
		if ( m_data.GetSize( ) == 0 ) {
			return newModel;
		}

		gw2f::pf::ModelPackFile modelPackFile( m_data.GetPointer( ), m_data.GetSize( ) );

		//auto animationChunk = model.chunk<gw2f::pf::ModelChunks::Animation>( );
		//auto propertiesChunk = model.chunk<gw2f::pf::ModelChunks::Properties>( );
		//auto rootmotionChunk = model.chunk<gw2f::pf::ModelChunks::RootMotion>( );
		//auto skeletonChunk = model.chunk<gw2f::pf::ModelChunks::Skeleton>( );

		this->readGeometry( newModel, modelPackFile );
		this->readMaterialData( newModel, modelPackFile );

		return newModel;
	}

	void ModelReader::readGeometry( Model& p_model, gw2f::pf::ModelPackFile& p_modelPackFile ) const {
		auto geometryChunk = p_modelPackFile.chunk<gw2f::pf::ModelChunks::Geometry>( );

		// Bail if no data
		if ( !geometryChunk->meshes.data( ) ) {
			return;
		}

		uint32 meshCount = geometryChunk->meshes.size( );

		// Bail if no meshes to read
		if ( !meshCount ) {
			return;
		}

		// Create storage for submeshes now, so we can parallelize the loop
		Mesh* meshes = p_model.addMeshes( meshCount );

#pragma omp parallel for shared( meshes )
		for ( int i = 0; i < static_cast<int>( meshCount ); i++ ) {
			// Fetch mesh info
			auto meshInfo = geometryChunk->meshes[i];

			// Fetch buffer info
			auto vertexInfo = meshInfo.geometry->verts;
			auto indicesInfo = meshInfo.geometry->indices;

			// Add new mesh
			Mesh& mesh = meshes[i];

			// Material data
			mesh.materialIndex = meshInfo.materialIndex;
			mesh.materialName = wxString::FromUTF8( meshInfo.materialName.data( ) );

			// Vertex data
			if ( vertexInfo.vertexCount ) {
				this->readVertexBuffer( mesh, vertexInfo.mesh.vertices.data( ), vertexInfo.vertexCount, static_cast<ANetFlexibleVertexFormat>( vertexInfo.mesh.fvf ) );
			}

			auto indiceCount = indicesInfo.indices.size( );
			// Index data
			if ( indiceCount ) {
				this->readIndiceBuffer( mesh, reinterpret_cast<const byte*>( indicesInfo.indices.data( ) ), indiceCount );
			}

		}
	}

	void ModelReader::readVertexBuffer( Mesh& p_mesh, const byte* p_data, uint p_vertexCount, ANetFlexibleVertexFormat p_vertexFormat ) const {
		p_mesh.vertices.SetSize( p_vertexCount );
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
				::memcpy( &vertex.position, pos, sizeof( vertex.position ) );
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
				vertex.position.x = *reinterpret_cast<const half*>( pos + 0 * sizeof( half ) );
				vertex.position.y = *reinterpret_cast<const half*>( pos + 1 * sizeof( half ) );
				vertex.position.z = *reinterpret_cast<const half*>( pos + 2 * sizeof( half ) );
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

	void ModelReader::readIndiceBuffer( Mesh& p_mesh, const byte* p_data, uint p_indiceCount ) const {
		p_mesh.triangles.SetSize( p_indiceCount / 3 );
		::memcpy( p_mesh.triangles.GetPointer( ), p_data, p_mesh.triangles.GetSize( ) * sizeof( Triangle ) );

		// Calculate bounds
		float floatMin = std::numeric_limits<float>::min( );
		float floatMax = std::numeric_limits<float>::max( );
		p_mesh.bounds.min = glm::vec3( floatMax, floatMax, floatMax );
		p_mesh.bounds.max = glm::vec3( floatMin, floatMin, floatMin );

		glm::vec3 min = p_mesh.bounds.min;
		glm::vec3 max = p_mesh.bounds.max;

		auto indices = reinterpret_cast<const uint16*>( p_data );
		for ( uint i = 0; i < p_indiceCount; i++ ) {
			auto& vertex = p_mesh.vertices[indices[i]];
			glm::vec3 position = vertex.position;
			min = glm::min( min, position );
			max = glm::max( max, position );
		}

		p_mesh.bounds.min = min;
		p_mesh.bounds.max = max;
	}

	void ModelReader::readMaterialData( Model& p_model, gw2f::pf::ModelPackFile& p_modelPackFile ) const {
		auto modelChunk = p_modelPackFile.chunk<gw2f::pf::ModelChunks::Model>( );

		// Bail if no data
		if ( !modelChunk->permutations.data( ) ) {
			return;
		}

		// Read some necessary data
		uint32 numMaterialInfo = modelChunk->permutations.size( );
		auto materialInfoArray = modelChunk->permutations;

		// Count materials
		uint materialCount = 0;
		for ( uint i = 0; i < numMaterialInfo; i++ ) {
			materialCount = wxMax( materialCount, materialInfoArray[i].materials.size( ) );
		}

		// Bail if no materials
		if ( !materialCount ) {
			return;
		}

		// Prepare parallel loop
		std::vector<omp_lock_t> locks( materialCount );
		for ( auto iter = std::begin( locks ); iter != std::end( locks ); iter++ ) {
			omp_init_lock( &( *iter ) );
		}

		MaterialData* materialData = p_model.addMaterialData( materialCount );

		// Loop through each material info
#pragma omp parallel for shared(locks, materialData)
		for ( int i = 0; i < static_cast<int>( numMaterialInfo ); i++ ) {
			// Bail if no material data
			if ( !materialInfoArray[i].materials.data( ) ) {
				continue;
			}

			auto materialsArray = materialInfoArray[i].materials;

			// Loop through each material in these material infos
			for ( uint j = 0; j < materialsArray.size( ); j++ ) {
				auto& data = materialData[j];

				// Only one thread must access this material at a time
				omp_set_lock( &locks[j] );

				// Bail if this material index already has data
				if ( data.materialFlags && data.diffuseMap && data.normalMap && data.lightMap ) {
					omp_unset_lock( &locks[j] );
					continue;
				}

				// Read material info
				auto materialInfo = materialsArray[i];

				// We are (almost) *only* interested in textures
				data.materialFlags = materialInfo.materialFlags;
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
					if ( ( textures[t].token >> 32 ) == 0x67531924 ) {
						data.diffuseMap = ( DatFile::fileIdFromFileReference( *fileReference ) + 1 );
					}

					// Normal?
					else if ( ( ( textures[t].token >> 32 ) == 0x1816c9ee )
						|| ( ( textures[t].token >> 32 ) == 0x8b0bbd87 )
						|| ( ( textures[t].token >> 32 ) == 0xa55a48b0 ) ) {
						data.normalMap = ( DatFile::fileIdFromFileReference( *fileReference ) + 1 );
					}

					// Light Map ?
					else if ( ( textures[t].token >> 32 ) == 0x680bbd87 ) {
						data.lightMap = ( DatFile::fileIdFromFileReference( *fileReference ) + 1 );
						break;
					}
				}

				// Let other threads access this material now that we're done
				omp_unset_lock( &locks[j] );
			}
		}

		// Destroy locks
		for ( auto iter = std::begin( locks ); iter != std::end( locks ); iter++ ) {
			omp_destroy_lock( &( *iter ) );
		}
	}

}; // namespace gw2b
