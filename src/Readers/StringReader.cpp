/* \file       Readers/StringReader.cpp
*  \brief      Contains the definition of the String reader class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2014-2015 Khral Steelforge <https://github.com/kytulendu>

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

#include <wx/string.h>
#include <wx/sstream.h>
#include <sstream>

#include <gw2formats/StringsFile.h>

#include "StringReader.h"

namespace gw2b {

	StringReader::StringReader( const Array<byte>& p_data, ANetFileType p_fileType )
		: FileReader( p_data, p_fileType ) {
	}

	StringReader::~StringReader( ) {
	}

	wxString StringReader::getString( ) const {
		wxString string;

		// Table header.
		string << L"Entry;String" << L"\r\n";

		gw2f::StringsFile stringFile( m_data.GetPointer( ), m_data.GetSize( ) );

#ifdef _DEBUG
		switch ( stringFile.language( ) ) {
		case gw2f::language::English:
			wxLogDebug( wxT( "English String." ) );
			break;
		case gw2f::language::French:
			wxLogDebug( wxT( "French String." ) );
			break;
		case gw2f::language::German:
			wxLogDebug( wxT( "German String." ) );
			break;
		case gw2f::language::Korean:
			wxLogDebug( wxT( "Korean String." ) );
			break;
		case gw2f::language::Spanish:
			wxLogDebug( wxT( "Spanish String." ) );
			break;
		}
#endif

		for ( size_t i = 0; i < stringFile.entryCount( ); i++ ) {
			auto& entry = stringFile.entry( i );

			wxString str;
			if ( entry.isEncrypted( ) ) {
				str = L"Encrypted string";
			} else {
				str = entry.get( );
				if ( str.IsEmpty( ) ) {
					str = L"Empty string";
				}
			}
			string << i << L";\"" << str << L"\"\r\n";
		}
		return string;
	}

	Array<byte> StringReader::convertData( ) const {
		auto string = this->getString( );

		wxString output = string.utf8_str( );

		// Convert string to byte array
		Array<byte> outputData( output.length( ) );
		::memcpy( outputData.GetPointer( ), output.utf8_str( ), output.length( ) );

		return outputData;
	}

	bool StringReader::isValidHeader( const byte* p_data ) {
		auto fourcc = *reinterpret_cast<const uint32*>( p_data );

		if ( fourcc == FCC_strs ) {
			return true;
		}
		return false;
	}

}; // namespace gw2b
