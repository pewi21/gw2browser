/* \file       Readers/MapReader.cpp
*  \brief      Contains the definition of the map reader class.
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

#include <gw2formats/pf/MapPackFile.h>

#include "MapReader.h"

namespace gw2b {

    MapReader::MapReader( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType )
        : FileReader( p_data, p_datFile, p_fileType ) {
    }

    MapReader::~MapReader( ) {
    }

    Array<float> MapReader::getMapData( ) const {
        gw2f::pf::MapPackFile mapFile( m_data.GetPointer( ), m_data.GetSize( ) );

        //auto audioChunk = mapFile.chunk<gw2f::pf::MapChunks::Audio>( );
        //auto missionChunk = mapFile.chunk<gw2f::pf::MapChunks::Mission>( );
        //auto paramChunk = mapFile.chunk<gw2f::pf::MapChunks::Param>( );
        //auto shoreChunk = mapFile.chunk<gw2f::pf::MapChunks::Shore>( );
        //auto surfaceChunk = mapFile.chunk<gw2f::pf::MapChunks::Surfaces>( );
        //auto terniChunk = mapFile.chunk<gw2f::pf::MapChunks::TerrainImg>( );
        //auto areaChunk = mapFile.chunk<gw2f::pf::MapChunks::Areas>( );
        //auto collideChunk = mapFile.chunk<gw2f::pf::MapChunks::Collide>( );
        //auto cubemapChunk = mapFile.chunk<gw2f::pf::MapChunks::CubeMap>( );
        //auto decalsChunk = mapFile.chunk<gw2f::pf::MapChunks::Decals>( );
        //auto environmentChunk = mapFile.chunk<gw2f::pf::MapChunks::Environment>( );
        //auto lightChunk = mapFile.chunk<gw2f::pf::MapChunks::Lights>( );
        //auto propertiesChunk = mapFile.chunk<gw2f::pf::MapChunks::Properties>( );
        //auto riverChunk = mapFile.chunk<gw2f::pf::MapChunks::Rivers>( );
        //auto shadowChunk = mapFile.chunk<gw2f::pf::MapChunks::ShadowExt>( );
        //auto zoneChunk = mapFile.chunk<gw2f::pf::MapChunks::Zones>( );

        auto terrainChunk = mapFile.chunk<gw2f::pf::MapChunks::Terrain>( );
        auto& heightMap = terrainChunk->heightMapArray;




        return Array<float>( );
    }

}; // namespace gw2b
