/* \file       Readers/MP3Reader.cpp
*  \brief      Contains the definition of the MP3 reader class.
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

#include "MP3Reader.h"

namespace gw2b {

	MP3Reader::MP3Reader( const Array<byte>& p_data, ANetFileType p_fileType )
		: FileReader( p_data, p_fileType ) {
	}

	MP3Reader::~MP3Reader( ) {
	}

	Array<byte> MP3Reader::getMP3( ) const {
		gw2f::pf::AudioPackFile asnd( m_data.GetPointer( ), m_data.GetSize( ) );

		auto audioChunk = asnd.chunk<gw2f::pf::AudioChunks::Waveform>( );

		int size = audioChunk->audioData.size( );

		Array<byte> outputArray( size );

#pragma omp parallel for
		for ( int i = 0; i < size; i++ ) {
			outputArray[i] = audioChunk->audioData[i];
		}

		return outputArray;
	}

	Array<byte> MP3Reader::convertData( ) const {
		return( this->getMP3( ) );
	}

	bool MP3Reader::isValidHeader( const byte* p_data, size_t p_size ) {
		if ( p_size < 0x10 ) {
			return false;
		}
		auto fourcc = *reinterpret_cast<const uint32*>( p_data );

		if ( ( fourcc & 0xffff ) == FCC_PF ) {
			return true;
		}

		return false;
	}

}; // namespace gw2b
