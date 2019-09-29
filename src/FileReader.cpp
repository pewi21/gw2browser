/** \file       FileReader.cpp
 *  \brief      Contains the definition for the base class of readers for the
 *              various file types.
 *  \author     Rhoot
 */

/**
 * Copyright (C) 2014-2019 Khral Steelforge <https://github.com/kytulendu>
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

#include "stdafx.h"

#include "Readers/StringReader.h"
#include "Readers/ImageReader.h"
#include "Readers/ModelReader.h"
#include "Readers/PackedSoundReader.h"
#include "Readers/asndMP3Reader.h"
#include "Readers/SoundBankReader.h"
#include "Readers/EulaReader.h"
#include "Readers/TextReader.h"
#include "Readers/MapReader.h"
#include "Readers/ContentReader.h"
#include "Readers/AFNTReader.h"

#include "FileReader.h"

namespace gw2b {

    FileReader::FileReader( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType )
        : m_data( p_data )
        , m_datFile( p_datFile )
        , m_fileType( p_fileType ) {
    }

    FileReader::~FileReader() {
    }

    void FileReader::clean() {
        m_data.Clear();
        m_fileType = ANFT_Unknown;
    }

    Array<byte> FileReader::rawData( ) const {
        return m_data;
    }

    FileReader* FileReader::readerForData( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType ) {
        switch ( p_fileType ) {
        case ANFT_ATEX:
        case ANFT_ATTX:
        case ANFT_ATEC:
        case ANFT_ATEP:
        case ANFT_ATEU:
        case ANFT_ATET:
        case ANFT_DDS:
        case ANFT_JPEG:
        case ANFT_WEBP:
        case ANFT_PNG:
            if ( ImageReader::isValidHeader( p_data.GetPointer(), p_data.GetSize() ) ) {
                return new ImageReader( p_data, p_datFile, p_fileType );
            }
            break;
        case ANFT_Model:
            return new ModelReader( p_data, p_datFile, p_fileType );
            break;
        case ANFT_StringFile:
            if ( StringReader::isValidHeader( p_data.GetPointer( ) ) ) {
                return new StringReader( p_data, p_datFile, p_fileType );
            }
            break;
        case ANFT_PackedMP3:
        case ANFT_PackedOgg:
            return new PackedSoundReader( p_data, p_datFile, p_fileType );
            break;
        case ANFT_asndMP3:
            return new asndMP3Reader( p_data, p_datFile, p_fileType );
            break;
        case ANFT_Bank:
            return new SoundBankReader( p_data, p_datFile, p_fileType );
            break;
        case ANFT_EULA:
            return new EulaReader( p_data, p_datFile, p_fileType );
            break;
        case ANFT_TEXT:
        case ANFT_UTF8:
            return new TextReader( p_data, p_datFile, p_fileType );
            break;
        case ANFT_GameContent:
            return new ContentReader( p_data, p_datFile, p_fileType );
            break;
//        case ANFT_MapParam:
//            return new MapReader( p_data, p_datFile, p_fileType );
//            break;
        case ANFT_BitmapFontFile:
            return new AFNTReader( p_data, p_datFile, p_fileType );
            break;
        default:
            break;
        }

        return new FileReader( p_data, p_datFile, p_fileType );
    }

}; // namespace gw2b
