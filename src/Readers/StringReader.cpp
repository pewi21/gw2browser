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

#include <codecvt>
#include <locale>

#include <wx/mstream.h>

#include "StringReader.h"

namespace gw2b {

#pragma pack(push, 1)

	struct StringReader::entryHeader {
		uint16 size;
		uint16 decryptionOffset;
		uint16 bitsPerSymbol;
	};

#pragma pack(pop)

	StringReader::StringReader( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType )
		: FileReader( p_data, p_datFile, p_fileType ) {
	}

	StringReader::~StringReader( ) {
	}

	std::vector<StringStruct> StringReader::getString( ) const {
		wxMemoryInputStream stream( m_data.GetPointer( ), m_data.GetSize( ) );

		auto end = stream.GetLength( ) - 2;
		//auto language = static_cast<language::Type>( *end );

		// skip past fcc
		stream.SeekI( 4 );
		auto pos = stream.TellI( );

		std::vector<StringStruct> string;

		// for track each entry in string file, usually have 1024 entry
		auto entryIndex = 0;

		while ( pos < end ) {
			if ( end - pos < 6 ) {
				wxLogMessage( wxT( "String file size is less than 6 bytes." ) );
				return string;
			}

			entryHeader entry;

			// Set stream position to begining of entry header
			stream.SeekI( pos );
			// Read the entry header
			stream.Read( reinterpret_cast<entryHeader*>( &entry ), sizeof( entryHeader ) );

			if ( entry.size > 0 ) {
				StringStruct str;

				// Set stream position to begining of entry data
				stream.SeekI( pos + 6 );

				auto size = ( entry.size - 6 ) >> 1;	// ( entry.size - 6 ) / 2

				auto isEncrypted = entry.decryptionOffset != 0 || entry.bitsPerSymbol != 0x10;
				if ( !isEncrypted ) {
					auto retval = allocate<char16>( size );

					// Read UTF-16 data
					stream.Read( retval, sizeof( char16 ) * size );

					std::basic_string<char16> rawEntryString;
					rawEntryString.assign( retval, size );
#if defined(_MSC_VER)
					str.string = wxString::Format( wxT( "%s" ), rawEntryString.c_str( ) );
#elif defined(__GNUC__) || defined(__GNUG__)
					std::wstring_convert<std::codecvt_utf8_utf16<char16>, char16> temp;
					std::string mbs = temp.to_bytes( rawEntryString );
					str.string = wxString( mbs.c_str( ) );
#endif
					freePointer( retval );

					if ( str.string.IsEmpty( ) ) {
						//str.string = wxT( "Empty string" );

						// Comment this out if enable above command
						pos = pos + entry.size;
						entryIndex++;
						continue;
					}
				} else {
					//str.string = wxT( "Encrypted string" );

					// Comment this out if enable above command
					pos = pos + entry.size;
					entryIndex++;
					continue;
				}

				str.id = entryIndex;
				string.push_back( str );

				pos = pos + entry.size;
				entryIndex++;
			}
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
