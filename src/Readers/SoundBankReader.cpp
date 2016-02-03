/* \file       Readers/SoundBankReader.cpp
*  \brief      Contains the definition of the sound bank reader class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2016 Khral Steelforge <https://github.com/kytulendu>

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

#include <gw2formats\pf\BankPackFile.h>
#include <gw2formats\pf\AudioPackFile.h>

#include "SoundBankReader.h"

namespace gw2b {

	SoundBankReader::SoundBankReader( const Array<byte>& p_data, ANetFileType p_fileType )
		: FileReader( p_data, p_fileType ) {
	}

	SoundBankReader::~SoundBankReader( ) {
	}

	std::vector<SoundBank> SoundBankReader::getSoundData( ) const {
		gw2f::pf::BankPackFile bankFile( m_data.GetPointer( ), m_data.GetSize( ) );
		auto bankChunk = bankFile.chunk<gw2f::pf::BankChunks::BankFile>( );

		int totalBank = bankChunk->asndFile.size( );

		std::vector<SoundBank> outputBuffer;

		for ( int i = 0; i < totalBank; i++ ) {
			auto asnd = bankChunk->asndFile[i];
			auto asndSize = asnd.audioData.size( );

			if ( asndSize ) {
				gw2f::pf::AudioPackFile asndFile( asnd.audioData.data( ), asndSize );

				auto asndChunk = asndFile.chunk<gw2f::pf::AudioChunks::Waveform>( );
				auto audioDataSize = asndChunk->audioData.size( );

				Array<byte> soundData( audioDataSize );
				::memcpy( soundData.GetPointer( ), asndChunk->audioData.data( ), audioDataSize );

				SoundBank output;
				output.voiceId = asnd.voiceId;
				output.data = soundData;

				outputBuffer.push_back( output );
			}
		}

		return outputBuffer;
	}

}; // namespace gw2b
