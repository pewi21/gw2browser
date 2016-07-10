/* \file       ScanDatTask.cpp
*  \brief      Contains declaration of the ScanDatTask class.
*  \author     Rhoot
*/

/*
Copyright (C) 2014-2016 Khral Steelforge <https://github.com/kytulendu>
Copyright (C) 2012 Rhoot <https://github.com/rhoot>

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

#include "ScanDatTask.h"

#include "DatFile.h"
#include "DatIndex.h"
#include "FileReader.h"

namespace gw2b {

	ScanDatTask::ScanDatTask( const std::shared_ptr<DatIndex>& p_index, DatFile& p_datFile )
		: m_index( p_index )
		, m_datFile( p_datFile ) {
		Ensure::notNull( p_index.get( ) );
		Ensure::notNull( &p_datFile );
	}

	ScanDatTask::~ScanDatTask( ) {
	}

	bool ScanDatTask::init( ) {
		this->setMaxProgress( m_datFile.numFiles( ) );
		this->setCurrentProgress( m_index->highestMftEntry( ) + 1 );

		uint filesLeft = m_datFile.numFiles( ) - ( m_index->highestMftEntry( ) + 1 );
		m_index->reserveEntries( filesLeft );

		return true;
	}

	void ScanDatTask::perform( ) {
		uint bytetoread = 32;
		// Make sure the output buffer is big enough
		this->ensureBufferSize( bytetoread );

		// Read file
		uint32 entryNumber = this->currentProgress( );
		uint size = m_datFile.peekFile( entryNumber, bytetoread, m_outputBuffer.GetPointer( ) );

		// Skip if empty
		if ( !size ) {
			this->setCurrentProgress( entryNumber + 1 );
			return;
		}

		// Get the file type
		ANetFileType fileType;
		auto results = m_datFile.identifyFileType( m_outputBuffer.GetPointer( ), size, fileType );

		// Enough data to identify the file type?
		uint lastRequestedSize = bytetoread;
		while ( results == DatFile::IR_NotEnoughData ) {
			uint sizeRequired = this->requiredIdentificationSize( m_outputBuffer.GetPointer( ), size, fileType );

			// Prevent infinite loops
			if ( sizeRequired == lastRequestedSize ) {
				break;
			}
			lastRequestedSize = sizeRequired;

			// Re-read with the newly asked-for size
			this->ensureBufferSize( sizeRequired );
			size = m_datFile.peekFile( entryNumber, sizeRequired, m_outputBuffer.GetPointer( ) );
			results = m_datFile.identifyFileType( m_outputBuffer.GetPointer( ), size, fileType );
		}

		// Need another check, since the file might have been reloaded a couple of times
		if ( !size ) {
			this->setCurrentProgress( entryNumber + 1 );
			return;
		}

		// Categorize the entry
		auto category = this->categorize( fileType, m_outputBuffer.GetPointer( ), size );

		// Add to index
		uint baseId = m_datFile.baseIdFromFileNum( entryNumber );
		auto& newEntry = m_index->addIndexEntry( )
			->setBaseId( baseId )
			.setFileId( m_datFile.fileIdFromFileNum( entryNumber ) )
			.setFileType( fileType )
			.setMftEntry( entryNumber )
			.setName( wxString::Format( wxT( "%d" ), baseId ) );
		// Found a file with no baseId...
		if ( baseId == 0 ) {
			newEntry.setName( wxString::Format( wxT( "ID-less_%d" ), entryNumber ) );
		}
		// Finalize the add
		category->addEntry( &newEntry );
		newEntry.finalizeAdd( );

		// Delete the reader and proceed to the next file
		this->setText( wxString::Format( wxT( "Scanning .dat: %d/%d" ), entryNumber, this->maxProgress( ) ) );
		this->setCurrentProgress( entryNumber + 1 );
	}

	uint ScanDatTask::requiredIdentificationSize( const byte* p_data, size_t p_size, ANetFileType p_fileType ) {
		switch ( p_fileType ) {
		case ANFT_Binary:
			if ( p_size >= 0x40 ) {
				return *reinterpret_cast<const uint32*>( p_data + 0x3c ) + 0x18;
			} else {
				return 0x140;   // Seems true for most of them
			}
		case ANFT_Sound:
			return 0x80;		// 128 byte
		default:
			return 0x20;
		}
	}

#define MakeCategory(x)     { category = m_index->findOrAddCategory(x); }
#define MakeSubCategory(x)  { category = category->findOrAddSubCategory(x); }
	DatIndexCategory* ScanDatTask::categorize( ANetFileType p_fileType, const byte* p_data, size_t p_size ) {
		DatIndexCategory* category = nullptr;

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
			MakeCategory( wxT( "Textures" ) );

			switch ( p_fileType ) {
			case ANFT_ATEX:
				MakeSubCategory( wxT( "Generic Textures" ) );
				break;
			case ANFT_ATTX:
				MakeSubCategory( wxT( "Terrain Textures" ) );
				break;
			case ANFT_ATEC:
				MakeSubCategory( wxT( "ATEC" ) );
				break;
			case ANFT_ATEP:
				MakeSubCategory( wxT( "Map Textures" ) );
				break;
			case ANFT_ATEU:
				MakeSubCategory( wxT( "UI Textures" ) );
				break;
			case ANFT_ATET:
				MakeSubCategory( wxT( "ATET" ) );
				break;
			case ANFT_DDS:
				MakeSubCategory( wxT( "DDS" ) );
				break;
			case ANFT_JPEG:
				MakeSubCategory( wxT( "JPEG" ) );
				break;
			case ANFT_WEBP:
				MakeSubCategory( wxT( "WebP" ) );
				break;
			case ANFT_PNG:
				MakeSubCategory( wxT( "PNG" ) );
				break;
			}

			if ( ( p_fileType == ANFT_ATEX || p_fileType == ANFT_ATTX || p_fileType == ANFT_ATEC ||
				p_fileType == ANFT_ATEP || p_fileType == ANFT_ATEU || p_fileType == ANFT_ATET ) ) {
				uint16 width = *reinterpret_cast<const uint16*>( p_data + 8 );
				uint16 height = *reinterpret_cast<const uint16*>( p_data + 10 );
				MakeSubCategory( wxString::Format( wxT( "%ux%u" ), width, height ) );
			} else if ( p_fileType == ANFT_DDS && p_size >= 20 ) {
				uint32 width = *reinterpret_cast<const uint32*>( p_data + 16 );
				uint32 height = *reinterpret_cast<const uint32*>( p_data + 12 );
				MakeSubCategory( wxString::Format( wxT( "%ux%u" ), width, height ) );
			}

			break;
		case ANFT_Sound:
		case ANFT_MP3:
		case ANFT_Ogg:
		case ANFT_PackedMP3:
		case ANFT_PackedOgg:
		case ANFT_asndMP3:
		case ANFT_asndOgg:
			MakeCategory( wxT( "Sounds" ) );

			switch ( p_fileType ) {
			case ANFT_MP3:
				MakeSubCategory( wxT( "MP3" ) );
				break;
			case ANFT_Ogg:
				MakeSubCategory( wxT( "Ogg" ) );
				break;
			case ANFT_asndMP3:
				MakeSubCategory( wxT( "asndMP3" ) );
				break;
			case ANFT_asndOgg:
				MakeSubCategory( wxT( "asndOgg" ) );
				break;
			case ANFT_PackedMP3:
				MakeSubCategory( wxT( "PackedMP3" ) );
				break;
			case ANFT_PackedOgg:
				MakeSubCategory( wxT( "PackedOgg" ) );
				break;
			}
			break;

		case ANFT_Binary:
		case ANFT_EXE:
		case ANFT_DLL:
			MakeCategory( wxT( "Binaries" ) );
			break;

		case ANFT_StringFile:
		{
			MakeCategory( wxT( "Strings" ) );

			uint32 entryNumber = this->currentProgress( );

			auto buffer = allocate<byte>( m_datFile.fileSize( entryNumber ) );
			auto size = m_datFile.readFile( entryNumber, buffer );

			// strs file format that have to read near end of file to know what language is
			auto pos = buffer + 4;
			auto end = buffer + size - 2;
			auto language = static_cast<uint32>( *end );

			switch ( language ) {
			case 0:
				MakeSubCategory( wxT( "English" ) );
				break;
			case 1:
				MakeSubCategory( wxT( "Korean" ) );
				break;
			case 2:
				MakeSubCategory( wxT( "French" ) );
				break;
			case 3:
				MakeSubCategory( wxT( "German" ) );
				break;
			case 4:
				MakeSubCategory( wxT( "Spanish" ) );
				break;
			case 5:
				MakeSubCategory( wxT( "Chinese" ) );
				break;
			default:
				MakeSubCategory( wxT( "Unknown" ) );
				MakeSubCategory( wxString::Format( wxT( "%u" ), language ) );
			}
			freePointer( buffer );
			break;
		}
		case ANFT_Manifest:
			MakeCategory( wxT( "Manifests" ) );
			break;
		case ANFT_PortalManifest:
			MakeCategory( wxT( "Portal Manifests" ) );
			break;
		case ANFT_TextPackManifest:
			MakeCategory( wxT( "TextPack Manifests" ) );
			break;
		case ANFT_TextPackVariant:
			MakeCategory( wxT( "TextPack Variant" ) );
			break;
		case ANFT_TextPackVoices:
			MakeCategory( wxT( "TextPack Voices" ) );
			break;
		case ANFT_Bank:
			MakeCategory( wxT( "Soundbank" ) );
			break;
		case ANFT_BankIndex:
			MakeCategory( wxT( "Soundbank Index" ) );
			break;
		case ANFT_AudioScript:
			MakeCategory( wxT( "Audio Scripts" ) );
			break;
		case ANFT_Model:
			MakeCategory( wxT( "Models" ) );
			break;
		case ANFT_DependencyTable:
			MakeCategory( wxT( "Dependency Tables" ) );
			break;
		case ANFT_EULA:
			MakeCategory( wxT( "EULA" ) );
			break;
		case ANFT_Cinematic:
			MakeCategory( wxT( "Cinematics" ) );
			break;
		case ANFT_HavokCollision:
			MakeCategory( wxT( "Havok Collision" ) );
			break;
		case ANFT_MapContent:
			MakeCategory( wxT( "Map Content" ) );
			break;
		case ANFT_MapParam:
			MakeCategory( wxT( "Map Parameter" ) );
			break;
		case ANFT_MapShadow:
			MakeCategory( wxT( "Map Shadow" ) );
			break;
		case ANFT_PagedImageTable:
			MakeCategory( wxT( "Paged Image Table" ) );
			break;
		case ANFT_Material:
			MakeCategory( wxT( "Materials" ) );
			break;
		case ANFT_Composite:
			MakeCategory( wxT( "Composite Data" ) );
			break;
		case ANFT_AnimSequences:
			MakeCategory( wxT( "Animation Sequences" ) );
			break;
		case ANFT_EmoteAnimation:
			MakeCategory( wxT( "Emote Animations" ) );
			break;
		case ANFT_FontFile:
			MakeCategory( wxT( "Font" ) );
			break;
		case ANFT_Bink2Video:
			MakeCategory( wxT( "Bink Videos" ) );
			break;
		case ANFT_ShaderCache:
			MakeCategory( wxT( "Shader Cache" ) );
			break;
		case ANFT_Config:
			MakeCategory( wxT( "Configuration" ) );
			break;
		case ANFT_PF:
		case ANFT_ARAP:
			MakeCategory( wxT( "Misc" ) );

			if ( p_fileType == ANFT_PF && p_size >= 12 ) {
				MakeSubCategory( wxString( reinterpret_cast<const char*>( p_data + 8 ), 4 ) );
			}
			break;

		default: // unknown stuff
			MakeCategory( wxT( "Unknown" ) );

			// to do: printable character detection in files for detect text files.
			//MakeSubCategory( wxString::Format( wxT( "%x" ), *reinterpret_cast<const uint32*>( p_data ) ) );

			break;
		}
		return category;
	}

	void ScanDatTask::ensureBufferSize( size_t p_size ) {
		if ( m_outputBuffer.GetSize( ) < p_size ) {
			m_outputBuffer.SetSize( p_size );
		}
	}

}; // namespace gw2b
