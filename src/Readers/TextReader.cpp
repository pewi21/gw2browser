/* \file       Readers/TextReader.cpp
*  \brief      Contains the definition of the text reader class.
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

#include <codecvt>
#include <locale>

#include "TextReader.h"

namespace gw2b {

	TextReader::TextReader( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType )
		: FileReader( p_data, p_datFile, p_fileType ) {
	}

	TextReader::~TextReader( ) {
	}

	wxString TextReader::getString( ) const {
		auto data = m_data.GetPointer( );
		auto size = m_data.GetSize( );

		wxString str;
		for ( uint i = 0; i < size; i++ ) {
            if ( isprint( data[i] ) || iscntrl( data[i] ) ) {
                str << data[i];
            }
		}

		return str;
	}

}; // namespace gw2b
