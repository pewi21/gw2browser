/* \file       Readers/StringReader.cpp
*  \brief      Contains the definition of the String reader class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2014-2016 Khral Steelforge <https://github.com/kytulendu>

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

#include <gw2formats/StringsFile.h>

#include "StringReader.h"

namespace gw2b {

	StringReader::StringReader( const Array<byte>& p_data, ANetFileType p_fileType )
		: FileReader( p_data, p_fileType ) {
	}

	StringReader::~StringReader( ) {
	}

	std::vector<StringStruct> StringReader::getString( ) const {
		gw2f::StringsFile stringFile( m_data.GetPointer( ), m_data.GetSize( ) );

		std::vector<StringStruct> string;

		for ( size_t i = 0; i < stringFile.entryCount( ); i++ ) {
			auto& entry = stringFile.entry( i );

			StringStruct str;
			if ( entry.isEncrypted( ) ) {
				//str.string = wxT( "Encrypted string" );
				continue;
			} else {
				str.string = wxString::Format( wxT( "%s" ), entry.get( ).c_str( ) );
				if ( str.string.IsEmpty( ) ) {
					//str.string = wxT( "Empty string" );
					continue;
				}
			}
			str.id = i;
			string.push_back( str );
		}

		return string;
	}

	bool StringReader::isValidHeader( const byte* p_data ) {
		auto fourcc = *reinterpret_cast<const uint32*>( p_data );

		if ( fourcc == FCC_strs ) {
			return true;
		}
		return false;
	}

}; // namespace gw2b
