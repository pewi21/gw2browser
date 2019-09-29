/** \file       Readers/ContentReader.cpp
 *  \brief      Contains the definition of the content manifest reader class.
 *  \author     Khral Steelforge
 */

/**
 * Copyright (C) 2018 Khral Steelforge <https://github.com/kytulendu>
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

#include <codecvt>
#include <locale>
#include <gw2formats/pf/ContentManifestPackFile.h>

#include "ContentReader.h"

namespace gw2b {

    ContentReader::ContentReader( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType )
        : FileReader( p_data, p_datFile, p_fileType ) {
    }

    ContentReader::~ContentReader( ) {
    }

    std::unique_ptr<tinyxml2::XMLDocument> ContentReader::getContentData( ) const {
        gw2f::pf::ContentManifestPackFile cntcFile( m_data.GetPointer( ), m_data.GetSize( ) );

        auto cntcChunk = cntcFile.chunk<gw2f::pf::ContentManifestChunks::Main>( );

        // Create xml document
        auto xmlDoc = std::unique_ptr<tinyxml2::XMLDocument>( new tinyxml2::XMLDocument( ) );

        // Add root
        auto pRoot = xmlDoc->NewElement( "Root" );
        xmlDoc->InsertFirstChild( pRoot );

        // Add file format identification
        auto pElement = xmlDoc->NewElement( "FileFormat" );
        pElement->SetText( "cntc" );
        pRoot->InsertEndChild( pElement );

        // -------------------------------------------

        // flags
        pElement = xmlDoc->NewElement( "flags" );
        pElement->SetAttribute( "Type", "dword" );
        pElement->SetText( cntcChunk->flags );
        pRoot->InsertEndChild( pElement );

        // typeInfos
        pElement = xmlDoc->NewElement( "typeInfos" );
        for ( uint i = 0; i < cntcChunk->typeInfos.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

            auto pChildElement = xmlDoc->NewElement( "guidOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->typeInfos[i].guidOffset );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "uidOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->typeInfos[i].uidOffset );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "dataIdOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->typeInfos[i].dataIdOffset );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "nameOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->typeInfos[i].nameOffset );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "trackReferences" );
            pChildElement->SetAttribute( "Type", "byte" );
            pChildElement->SetText( cntcChunk->typeInfos[i].trackReferences );
            pListElement->InsertEndChild( pChildElement );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->typeInfos.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // namespaces
        pElement = xmlDoc->NewElement( "namespaces" );
        for ( uint i = 0; i < cntcChunk->namespaces.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

#if defined(_MSC_VER)
            wxString str( wxString::Format( wxT( "%s" ), cntcChunk->namespaces[i].name.data( ) ) );
#elif defined(__GNUC__) || defined(__GNUG__)
            wxString str;
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
            std::string mbs = conv.to_bytes( cntcChunk->namespaces[i].name.data( ) );
            str = wxString( mbs.c_str( ), wxConvUTF8 );
#endif

            auto pChildElement = xmlDoc->NewElement( "name" );
            pChildElement->SetAttribute( "Type", "wstring" );
            pChildElement->SetText( str.mb_str( ) );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "domain" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->namespaces[i].domain );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "parentIndex" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->namespaces[i].parentIndex );
            pListElement->InsertEndChild( pChildElement );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->namespaces.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // fileRefs
        pElement = xmlDoc->NewElement( "fileRefs" );
        for ( uint i = 0; i < cntcChunk->fileRefs.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

            pListElement->SetText( cntcChunk->fileRefs[i].fileId( ) );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->fileRefs.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // indexEntries
        pElement = xmlDoc->NewElement( "indexEntries" );
        for ( uint i = 0; i < cntcChunk->indexEntries.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

            auto pChildElement = xmlDoc->NewElement( "type" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->indexEntries[i].type );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "offset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->indexEntries[i].offset );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "namespaceIndex" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->indexEntries[i].namespaceIndex );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "rootIndex" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->indexEntries[i].rootIndex );
            pListElement->InsertEndChild( pChildElement );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->indexEntries.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // localOffsets
        pElement = xmlDoc->NewElement( "localOffsets" );
        for ( uint i = 0; i < cntcChunk->localOffsets.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

            auto pChildElement = xmlDoc->NewElement( "relocOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->localOffsets[i].relocOffset );
            pListElement->InsertEndChild( pChildElement );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->localOffsets.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // externalOffsets
        pElement = xmlDoc->NewElement( "externalOffsets" );
        for ( uint i = 0; i < cntcChunk->externalOffsets.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

            auto pChildElement = xmlDoc->NewElement( "relocOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->externalOffsets[i].relocOffset );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "targetFileIndex" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->externalOffsets[i].targetFileIndex );
            pListElement->InsertEndChild( pChildElement );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->externalOffsets.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // fileIndices
        pElement = xmlDoc->NewElement( "fileIndices" );
        for ( uint i = 0; i < cntcChunk->fileIndices.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

            auto pChildElement = xmlDoc->NewElement( "relocOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->fileIndices[i].relocOffset );
            pListElement->InsertEndChild( pChildElement );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->fileIndices.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // stringIndices
        pElement = xmlDoc->NewElement( "stringIndices" );
        for ( uint i = 0; i < cntcChunk->stringIndices.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

            auto pChildElement = xmlDoc->NewElement( "relocOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->stringIndices[i].relocOffset );
            pListElement->InsertEndChild( pChildElement );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->stringIndices.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // trackedReferences
        pElement = xmlDoc->NewElement( "trackedReferences" );
        for ( uint i = 0; i < cntcChunk->trackedReferences.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

            auto pChildElement = xmlDoc->NewElement( "sourceOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->trackedReferences[i].sourceOffset );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "targetFileIndex" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->trackedReferences[i].targetFileIndex );
            pListElement->InsertEndChild( pChildElement );

            pChildElement = xmlDoc->NewElement( "targetOffset" );
            pChildElement->SetAttribute( "Type", "dword" );
            pChildElement->SetText( cntcChunk->trackedReferences[i].targetOffset );
            pListElement->InsertEndChild( pChildElement );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->trackedReferences.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // strings
        pElement = xmlDoc->NewElement( "strings" );
        for ( uint i = 0; i < cntcChunk->strings.size( ); i++ ) {
            auto pListElement = xmlDoc->NewElement( "data" );

#if defined(_MSC_VER)
            wxString str( wxString::Format( wxT( "%s" ), cntcChunk->strings[i].data( ) ) );
#elif defined(__GNUC__) || defined(__GNUG__)
            wxString str;
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
            std::string mbs = conv.to_bytes( cntcChunk->strings[i].data( ) );
            str = wxString( mbs.c_str( ), wxConvUTF8 );
#endif

            pListElement->SetText( str.mb_str( ) );

            pElement->InsertEndChild( pListElement );
        }
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->strings.size( ) ) );
        pRoot->InsertEndChild( pElement );

        // content
        //wxString content;
        //for ( uint i = 0; i < cntcChunk->content.size( ); i++ ) {
        //    content << wxString::Format( wxT( "0x%02x " ), cntcChunk->content[i] );
        //}

        pElement = xmlDoc->NewElement( "content" );
        pElement->SetAttribute( "ItemCount", static_cast<uint>( cntcChunk->content.size( ) ) );

        //auto pChildElement = xmlDoc->NewElement( "data" );
        //pChildElement->SetText( content.mb_str( ) );
        //pElement->InsertEndChild( pChildElement );

        pRoot->InsertEndChild( pElement );

        // -------------------------------------------

        return xmlDoc;
    }

}; // namespace gw2b
