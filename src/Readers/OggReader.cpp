/* \file       Readers/OggReader.cpp
*  \brief      Contains the definition of the Ogg reader class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2014 Khral Steelforge <https://github.com/kytulendu>

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

#include <gw2formats\pf\AudioPackFile.h>

#include "OggReader.h"

namespace gw2b {

	OggReader::OggReader( const Array<byte>& p_data, ANetFileType p_fileType )
		: FileReader( p_data, p_fileType ) {
	}

	OggReader::~OggReader( ) {
	}

	Array<byte> OggReader::getOgg( ) const {

		gw2f::pf::AudioPackFile asnd( m_data.GetPointer( ), m_data.GetSize( ) );

		auto chunk = asnd.chunk<gw2f::pf::AudioChunks::Waveform>( );

		Array<byte> output( chunk->audioData.size( ) );

		int size = chunk->audioData.size( );

#pragma omp parallel for
		for ( int i = 0; i < size; i++ ) {
			output[i] = chunk->audioData[i];
		}

		return output;
	}

	Array<byte> OggReader::convertData( ) const {
		auto outputData = this->getOgg( );

		return outputData;
	}

}; // namespace gw2b
