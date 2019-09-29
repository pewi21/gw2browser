/** \file       Readers/ModelReader.h
 *  \brief      Contains the declaration of the model reader class.
 *  \author     Rhoot
 */

/**
 * Copyright (C) 2014-2017 Khral Steelforge <https://github.com/kytulendu>
 * Copyright (C) 2012 Rhoot <https://github.com/rhoot>
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

#pragma once

#ifndef READERS_MODELREADER_H_INCLUDED
#define READERS_MODELREADER_H_INCLUDED

#include <vector>

#include <gw2formats/pf/ModelPackFile.h>

#include "FileReader.h"
#include "ANetStructs.h"

namespace gw2b {
    class PackFile;

#pragma pack(push, 1)

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    union Triangle {
        struct {
            uint16 index1;
            uint16 index2;
            uint16 index3;
        };
        uint16 indices[3];
    };

    struct Bounds {
        glm::vec3 min;
        glm::vec3 max;

        Bounds& operator+=( const Bounds& p_other ) {
            min = glm::min( min, p_other.min );
            max = glm::max( max, p_other.max );
            return *this;
        }

        glm::vec3 center( ) const {
            glm::vec3 min = this->min;
            glm::vec3 max = this->max;
            glm::vec3 center = glm::lerp( min, max, 0.5f );
            glm::vec3 retval = center;
            return retval;
        }

        glm::vec3 size( ) const {
            glm::vec3 min = this->min;
            glm::vec3 max = this->max;
            glm::vec3 size = ( max - min );
            glm::vec3 retval = size;
            return retval;
        }
    };

#pragma pack(pop)

    struct GW2Mesh {
        std::vector<Vertex>   vertices;
        std::vector<Triangle> triangles;
        wxString        materialName;
        int             materialIndex;
        uint32          flags;
        Bounds          bounds;
        byte            hasNormal : 1;
        byte            hasUV : 1;
    };

    struct GW2Material {
        uint32 materialId;          // Unknown
        uint32 materialFlags;       // Unknown
        uint32 materialFile;        // File Id for material file
        uint32 diffuseMap;
        uint32 normalMap;
        uint32 specularMap;
        uint32 lightMap;
        uint32 dye;
    };

    class GW2ModelData : public wxRefCounter {
    public:
        std::vector<GW2Mesh>        meshes;
        std::vector<GW2Material>    material;
    public:
        GW2ModelData( );
        GW2ModelData( const GW2ModelData& p_other );
        virtual ~GW2ModelData( );
    };

    class GW2Model {
        wxObjectDataPtr<GW2ModelData>  m_data;
    public:
        GW2Model( );
        GW2Model( const GW2Model& p_other );
        ~GW2Model( );

        GW2Model& operator=( const GW2Model& p_other );

        // Submeshes
        uint numMeshes( ) const;
        const GW2Mesh& mesh( uint p_index ) const;
        const std::vector<GW2Mesh>& mesh( ) const;
        GW2Mesh* addMeshes( uint p_amount );

        // Material data
        uint numMaterial( ) const;
        const GW2Material& material( uint p_index ) const;
        const std::vector<GW2Material>& material( ) const;
        GW2Material* addMaterial( uint p_amount );

        // helpers
        Bounds bounds( ) const;
    private:
        void unShare( );
    };

    class ModelReader : public FileReader {
    public:
        /** Constructor.
        *  \param[in]  pData        Data to be handled by this reader.
        *  \param[in]  p_datFile    Reference to an instance of DatFile.
        *  \param[in]  pFileType    File type of the given data. */
        ModelReader( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType );
        /** Destructor. Clears all data. */
        virtual ~ModelReader( );

        /** Gets the type of data contained in this file. Not to be confused with
        *  file type.
        *  \return DataType         type of data. */
        virtual DataType dataType( ) const override {
            return DT_Model;
        }
        /** Gets the model represented by this data.
        *  \return GW2Model         model. */
        GW2Model getModel( ) const;

    private:
        void readGeometry( GW2Model& p_model, gw2f::pf::ModelPackFile& p_modelPackFile ) const;
        void readVertexBuffer( GW2Mesh& p_mesh, const byte* p_data, uint p_vertexCount, ANetFlexibleVertexFormat p_vertexFormat ) const;
        uint vertexSize( ANetFlexibleVertexFormat p_vertexFormat ) const;
        void computeBond( GW2Mesh& p_mesh, const byte* p_data, uint p_indiceCount ) const;
        void readIndexBuffer( GW2Mesh& p_mesh, const byte* p_data, uint p_indiceCount ) const;
        void normalizeNormals( GW2Mesh& p_mesh ) const;
        void computeVertexNormals( GW2Mesh& p_mesh ) const;
        /** Rotate given mesh in ZY and invert Z. */
        void rotZYinvZ( GW2Mesh& p_mesh ) const;
        void readMaterial( GW2Model& p_model, gw2f::pf::ModelPackFile& p_modelPackFile ) const;
        void readMaterialPF( GW2Model& p_model, gw2f::pf::ModelPackFile& p_modelPackFile ) const;

    }; // class ModelReader

}; // namespace gw2b

#endif // READERS_MODELREADER_H_INCLUDED
