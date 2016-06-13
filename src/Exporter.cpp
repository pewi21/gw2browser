/* \file       Exporter.cpp
*  \brief      Contains the definition for the base class of exporters for the
*              various file types.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2016 Khral Steelforge <https://github.com/kytulendu>
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

#include <sstream>
#include <wx/sstream.h>
#include <wx/wfstream.h>

#include "DatFile.h"
#include "DatIndex.h"
#include "FileReader.h"
#include "Readers/ImageReader.h"
#include "Readers/StringReader.h"
#include "Readers/ModelReader.h"
#include "Readers/PackedSoundReader.h"
#include "Readers/asndMP3Reader.h"
#include "Readers/SoundBankReader.h"
#include "Readers/EulaReader.h"

#include "Exporter.h"

namespace gw2b {

	Exporter::Exporter( const Array<const DatIndexEntry*>& p_entries, DatFile& p_datFile, ExtractionMode p_mode )
		: m_datFile( p_datFile )
		, m_entries( p_entries )
		, m_progress( nullptr )
		, m_currentProgress( 0 )
		, m_mode( p_mode )
		, m_fileType( ANFT_Unknown ) {

		// If it's just one file, we could handle it here
		if ( m_entries.GetSize( ) == 1 ) {
			auto& entry = m_entries[0];

			// Ask for location
			wxFileDialog dialog( this,
				wxString::Format( wxT( "Extract %s..." ), entry->name( ) ),
				wxEmptyString,
				entry->name( ),
				wxFileSelectorDefaultWildcardStr,
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			// Open file for writing
			if ( dialog.ShowModal( ) == wxID_CANCEL ) {
				return;
			}

			// Set file path
			m_filename.SetPath( dialog.GetDirectory( ) );
			// Set file name
			m_filename.SetName( dialog.GetFilename( ) );

			// Convert and export file
			this->extractFile( *entry );

		// More than one files
		} else {
			// Ask for location
			wxDirDialog dialog( this, wxT( "Select output folder" ) );
			if ( dialog.ShowModal( ) == wxID_OK ) {

				m_path = dialog.GetPath( );

				uint numFile = static_cast<uint>( p_entries.GetSize( ) );

				auto title = wxString::Format( wxT( "Extracting %d %s..." ), numFile, ( p_entries.GetSize( ) == 1 ? wxT( "file" ) : wxT( "files" ) ) );
				m_progress = new wxProgressDialog( title, wxT( "Preparing to extract..." ), p_entries.GetSize( ), this, wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME );
				m_progress->Show( );

				// Loop through each files and update progress bar
				for ( uint i = 0; i < m_entries.GetSize( ); i++ ) {
					// DONE
					if ( m_currentProgress >= m_entries.GetSize( ) ) {
						break;
					}

					// Extract current file
					this->extractFiles( *m_entries[m_currentProgress] );

					bool shouldContinue = m_progress->Update( m_currentProgress, wxString::Format( wxT( "Extracting file %d/%d..." ), m_currentProgress, numFile ) );

					if ( shouldContinue ) {
						m_currentProgress++;
					} else {
						m_currentProgress = m_entries.GetSize( );
					}
				}
				deletePointer( m_progress );
			}
		}
	}

	const wxChar* Exporter::GetExtension( ) const {
		if ( m_mode == EM_Converted ) {
			switch ( this->m_fileType ) {
			case ANFT_ATEX:
			case ANFT_ATTX:
			case ANFT_ATEC:
			case ANFT_ATEP:
			case ANFT_ATEU:
			case ANFT_ATET:
			case ANFT_DDS:
			case ANFT_JPEG:
			case ANFT_WEBP:
				return wxT( "png" );
				break;
			case ANFT_Model:
				return wxT( "obj" );
				break;
			case ANFT_StringFile:
				return wxT( "csv" );
				break;
			case ANFT_PackedOgg:
			case ANFT_Ogg:
				return wxT( "ogg" );
				break;
			case ANFT_PackedMP3:
			case ANFT_asndMP3:
			case ANFT_MP3:
			case ANFT_Bank:
				return wxT( "mp3" );
				break;
			case ANFT_EULA:
				return wxT( "txt" );
				break;
			case ANFT_Bink2Video:
				return wxT( "bk2" );
				break;
			case ANFT_DLL:
				return wxT( "dll" );
				break;
			case ANFT_EXE:
				return wxT( "exe" );
				break;
			default:
				return wxT( "raw" ); // todo
				break;
			}
		} else {
			switch ( this->m_fileType ) {
				// Texture
			case ANFT_ATEX:
				return wxT( "atex" );
				break;
			case ANFT_ATTX:
				return wxT( "attx" );
				break;
			case ANFT_ATEC:
				return wxT( "atec" );
				break;
			case ANFT_ATEP:
				return wxT( "atep" );
				break;
			case ANFT_ATEU:
				return wxT( "ateu" );
				break;
			case ANFT_ATET:
				return wxT( "atet" );
				break;
			case ANFT_DDS:
				return wxT( "dds" );
				break;
			case ANFT_JPEG:
				return wxT( "jpg" );
				break;
			case ANFT_WEBP:
				return wxT( "webp" );
				break;

				// String files
			case ANFT_StringFile:
				return wxT( "strs" );
				break;
			case ANFT_EULA:
				return wxT( "eula" );
				break;

				// Maps stuff
			case ANFT_MapContent:
				return wxT( "cntc" );
				break;
			case ANFT_MapParam:
				return wxT( "parm" );
				break;
			case ANFT_MapShadow:
				return wxT( "mpsd" );
				break;
			case ANFT_PagedImageTable:
				return wxT( "pimg" );
				break;

			case ANFT_Material:
				return wxT( "amat" );
				break;
			case ANFT_Composite:
				return wxT( "cmpc" );
				break;
			case ANFT_HavokCollision:
				return wxT( "havk" );
				break;
			case ANFT_AnimSequences:
				return wxT( "seqn" );
				break;
			case ANFT_EmoteAnimation:
				return wxT( "emoc" );
				break;

			case ANFT_Cinematic:
				return wxT( "cinp" );
				break;

			case ANFT_PortalManifest:
				return wxT( "prlt" );
				break;

			case ANFT_TextPackManifest:
				return wxT( "txtm" );
				break;
			case ANFT_TextPackVariant:
				return wxT( "txtvar" );
				break;
			case ANFT_TextPackVoices:
				return wxT( "txtv" );
				break;

				// Sound
			case ANFT_Sound:
			case ANFT_asndMP3:
			case ANFT_asndOgg:
				return wxT( "sound" );
				break;
			case ANFT_PackedMP3:
			case ANFT_PackedOgg:
				return wxT( "asnd" );
				break;
			case ANFT_MP3:
				return wxT( "mp3" );
				break;
			case ANFT_Ogg:
				return wxT( "ogg" );
				break;
			case ANFT_Bank:
				return wxT( "abnk" );
				break;

				// Audio script
			case ANFT_AudioScript:
				return wxT( "amsp" );
				break;

				// Font file
			case ANFT_FontFile:
				return wxT( "eot" );	// Embedded OpenType
				break;

				// Bink2 Video file
			case ANFT_Bink2Video:
				return wxT( "bk2" );
				break;

			case ANFT_ShaderCache:
				return wxT( "cdhs" );
				break;

			case ANFT_Model:
				return wxT( "modl" );
				break;

				// Binary
			case ANFT_DLL:
				return wxT( "dll" );
				break;
			case ANFT_EXE:
				return wxT( "exe" );
				break;

			case ANFT_Config:
				return wxT( "locl" );
				break;

			case ANFT_ARAP:
				return wxT( "arap" );
				break;

			default:
				return wxT( "raw" );
				break;
			}
		}
	}

	void Exporter::extractFile( const DatIndexEntry& p_entry ) {
		auto entryData = m_datFile.readFile( p_entry.mftEntry( ) );
		// Valid data?
		if ( !entryData.GetSize( ) ) {
			wxMessageBox( wxT( "Failed to extract the file, most likely due to a decompression error." ), wxT( "Error" ), wxOK | wxICON_ERROR );
			return;
		}

		// Identify file type
		m_datFile.identifyFileType( entryData.GetPointer( ), entryData.GetSize( ), m_fileType );

		auto reader = FileReader::readerForData( entryData, m_datFile, m_fileType );

		if ( reader ) {
			// Set file extension
			m_filename.SetExt( wxString( this->GetExtension( ) ) );

			// Should we convert the file?
			if ( m_mode == EM_Converted ) {
				switch ( m_fileType ) {
				case ANFT_ATEX:
				case ANFT_ATTX:
				case ANFT_ATEC:
				case ANFT_ATEP:
				case ANFT_ATEU:
				case ANFT_ATET:
				case ANFT_DDS:
				case ANFT_JPEG:
				case ANFT_WEBP:
					this->exportImage( reader, p_entry.name( ) );
					break;
				case ANFT_StringFile:
					this->exportString( reader, p_entry.name( ) );
					break;
				case ANFT_EULA:
					this->exportEula( reader, p_entry.name( ) );
					break;
				case ANFT_PackedMP3:
				case ANFT_PackedOgg:
				case ANFT_asndMP3:
					this->exportSound( reader, p_entry.name( ) );
					break;
				case ANFT_Bank:
					this->exportSoundBank( reader, p_entry.name( ) );
					break;
				case ANFT_Model:
					this->exportModel( reader, p_entry.name( ) );
					break;
				default:
					//entryData = reader->rawData( );
					this->writeFile( entryData );
					break;
				}
			} else {
				//entryData = reader->rawData( );
				this->writeFile( entryData );
			}

			deletePointer( reader );

		} else {
			this->writeFile( entryData );
		}
	}

	void Exporter::extractFiles( const DatIndexEntry& p_entry ) {
		// Set file path
		m_filename.SetPath( m_path );
		// Set file name
		m_filename.SetName( p_entry.name( ) );

		// Appen category name as path
		this->appendPaths( m_filename, *p_entry.category( ) );

		// Create directory if not exist
		if ( !m_filename.DirExists( ) ) {
			m_filename.Mkdir( 511, wxPATH_MKDIR_FULL );
		}

		// Extract file
		this->extractFile( p_entry );
	}

	void Exporter::exportImage( FileReader* p_reader, const wxString& p_entryname ) {
		// Bail if not an image
		auto imgReader = dynamic_cast<ImageReader*>( p_reader );
		if ( !imgReader ) {
			wxLogMessage( wxString::Format( wxT( "Entry %s is not an image." ), p_entryname ) );
			return;
		}

		// Get image in wxImage
		auto imageData = imgReader->getImage( );

		if ( !imageData.IsOk( ) ) {
			wxMessageBox( wxT("imgReader->getImage( ) error."), wxT( "Error" ), wxOK | wxICON_ERROR );
			return;
		}

		// Write the png to memory
		wxMemoryOutputStream stream;
		if ( !imageData.SaveFile( stream, wxBITMAP_TYPE_PNG ) ) {
			wxMessageBox( wxT( "Failed to write png to memory." ), wxT( "Error" ), wxOK | wxICON_ERROR );
			return;
		}

		// Reset the position of the stream
		auto buffer = stream.GetOutputStreamBuffer( );
		buffer->Seek( 0, wxFromStart );

		// Read the data from the stream and into a buffer
		Array<byte> data( buffer->GetBytesLeft( ) );
		buffer->Read( data.GetPointer( ), data.GetSize( ) );

		// Write to file
		this->writeFile( data );
	}

	void Exporter::exportString( FileReader* p_reader, const wxString& p_entryname ) {
		// Bail if not a string
		auto strReader = dynamic_cast<StringReader*>( p_reader );
		if ( !strReader ) {
			wxLogMessage( wxString::Format( wxT( "Entry %s is not a string file." ), p_entryname ) );
			return;
		}

		// Get string
		auto string = strReader->getString( );
		if ( string.empty( ) ) {
			wxLogMessage( wxString::Format( wxT( "Entry %s is an empty string file." ), p_entryname ) );
			return;
		}

		wxString StringOut;
		for ( auto& it : string ) {
			StringOut << wxT( "\"" ) << it.id << wxT( "\";\"" ) << it.string << L"\"\r\n";
		}

		// Convert string to byte array
		Array<byte> data( StringOut.length( ) );
		::memcpy( data.GetPointer( ), StringOut.utf8_str( ), StringOut.length( ) );

		// Write to file
		this->writeFile( data );
	}

	void Exporter::exportEula( FileReader* p_reader, const wxString& p_entryname ) {
		auto eulaReader = dynamic_cast<EulaReader*>( p_reader );
		if ( !eulaReader ) {
			wxLogMessage( wxString::Format( wxT( "Entry %s is not a eula file." ), p_entryname ) );
			return;
		}

		// Get eula
		auto eula = eulaReader->getString( );
		// Get file name
		auto filename = m_filename.GetName( );
		// Write each record
		for ( uint i = 0; i < eula.size( ); i++ ) {
			wxString StringOut;
			StringOut << eula[i];

			// Convert string to byte array
			Array<byte> data( StringOut.length( ) );
			::memcpy( data.GetPointer( ), StringOut.utf8_str( ), StringOut.length( ) );

			m_filename.SetName( wxString::Format( wxT( "%s_%d" ), filename, i ) );
			// Write to file
			this->writeFile( data );
		}
	}

	void Exporter::exportSound( FileReader* p_reader, const wxString& p_entryname ) {
		Array<byte> data;
		if ( m_fileType == ANFT_asndMP3 ) {
			auto asndMP3 = dynamic_cast<asndMP3Reader*>( p_reader );
			if ( !asndMP3 ) {
				wxLogMessage( wxString::Format( wxT( "Entry %s is not a sound file." ), p_entryname ) );
				return;
			}
			// Get sound data
			data = asndMP3->getMP3Data( );

		} else if (
			( m_fileType == ANFT_PackedMP3 ) ||
			( m_fileType == ANFT_PackedOgg )
			) {
			auto packedSound = dynamic_cast<PackedSoundReader*>( p_reader );

			if ( !packedSound ) {
				wxLogMessage( wxString::Format( wxT( "Entry %s is not a sound file." ), p_entryname ) );
				return;
			}
			// Get sound data
			data = packedSound->getSoundData( );
		}
		// Write to file
		this->writeFile( data );
	}

	void Exporter::exportSoundBank( FileReader* p_reader, const wxString& p_entryname ) {
		auto sound = dynamic_cast<SoundBankReader*>( p_reader );
		if ( !sound ) {
			wxLogMessage( wxString::Format( wxT( "Entry %s is not a sound bank file." ), p_entryname ) );
			return;
		}

		auto data = sound->getSoundData( );
		auto filename = m_filename.GetName( );

		for ( auto& it : data ) {
			m_filename.SetName( wxString::Format( wxT( "%s_%d" ), filename, it.voiceId ) );

			//uint16 format = *reinterpret_cast<const uint16*>( it.data.GetPointer( ) );
			// the data is either compressed or encrypted or not mp3 format
			//if ( format != FCC_MP3 ) {
			//	//m_filename.SetExt( wxT( "raw" ) );
			//	continue;
			//}

			// Write to file
			this->writeFile( it.data );
		}
	}

	void Exporter::exportModel( FileReader* p_reader, const wxString& p_entryname ) {
		// Bail if not a model
		auto modlReader = dynamic_cast<ModelReader*>( p_reader );
		if ( !modlReader ) {
			wxMessageBox( wxString::Format( wxT( "Entry %s is in wrong format." ), p_entryname ), wxT( "" ), wxOK | wxICON_WARNING );
			return;
		}

		// Get model data
		auto model = modlReader->getModel( );

		// -------------
		// Export meshes
		// -------------

		// Export to Wavefront .obj file
		// https://en.wikipedia.org/wiki/Wavefront_.obj_file

		wxLogMessage( wxString::Format( wxT( "Writing %s OBJ file..." ), m_filename.GetName( ) ) );

		// Note: wxWidgets only does locale-specific number formatting. This does
		// not work well with obj-files.
		std::ostringstream modlStream;

		modlStream.imbue( std::locale( "C" ) );

		modlStream << "# Gw2Browser OBJ File: \'" << m_filename.GetName( ) << "\'" << std::endl;
		modlStream << "# Mesh Count : " << model.numMeshes( ) << std::endl;
		modlStream << "mtllib " << m_filename.GetName( ) << ".mtl" << std::endl;
		modlStream << std::endl;

		uint indexBase = 1;
		for ( uint i = 0; i < model.numMeshes( ); i++ ) {
			const Mesh& mesh = model.mesh( i );

			// Write header
			modlStream << "# Mesh " << i + 1 << ": " << mesh.vertices.size( ) << " vertices, " << mesh.triangles.size( ) << " triangles" << std::endl;
			modlStream << "g mesh" << i + 1 << std::endl;

			// Write positions
			for ( uint j = 0; j < mesh.vertices.size( ); j++ ) {
				modlStream << "v " << mesh.vertices[j].position.x << ' ' << mesh.vertices[j].position.y << ' ' << mesh.vertices[j].position.z << std::endl;
			}

			// Write UVs
			if ( mesh.hasUV ) {
				for ( uint j = 0; j < mesh.vertices.size( ); j++ ) {
					// Flip UV coordinate
					auto u = mesh.vertices[j].uv.x;
					auto v = 1.0f - mesh.vertices[j].uv.y;
					modlStream << "vt " << u << ' ' << v << std::endl;
				}
			}

			// Write normals
			if ( mesh.hasNormal ) {
				for ( uint j = 0; j < mesh.vertices.size( ); j++ ) {
					modlStream << "vn " << mesh.vertices[j].normal.x << ' ' << mesh.vertices[j].normal.y << ' ' << mesh.vertices[j].normal.z << std::endl;
				}
			}

			// If no material, use None material
			if ( mesh.materialName.empty( ) ) {
				modlStream << "usemtl None" << std::endl;
			} else {
				modlStream << "usemtl " << mesh.materialName.c_str( ) << std::endl;
			}

			// Enable/disable smooth shading
			//modlStream << "s off" << std::endl;
			//modlStream << "s 1" << std::endl;

			// Write faces
			for ( uint j = 0; j < mesh.triangles.size( ); j++ ) {
				const Triangle& triangle = mesh.triangles[j];

				modlStream << 'f';
				for ( uint k = 0; k < 3; k++ ) {
					uint index = triangle.indices[k] + indexBase;
					modlStream << ' ' << index;

					// UV reference
					if ( mesh.hasUV ) {
						modlStream << '/' << index;
					}

					// Normal reference
					if ( mesh.hasNormal ) {
						if ( !mesh.hasUV ) {
							modlStream << '/';
						}
						modlStream << '/' << index;
					}
				}
				modlStream << std::endl;
			}

			// newline before next mesh!
			modlStream << std::endl;
			indexBase += mesh.vertices.size( );
		}

		// Close stream
		modlStream.flush( );
		wxString meshString( modlStream.str( ) );
		modlStream.clear( );

		// Convert string to byte array
		Array<byte> meshData( meshString.length( ) );
		::memcpy( meshData.GetPointer( ), meshString.c_str( ), meshString.length( ) );

		// Write to file
		this->writeFile( meshData );

		// ----------------
		// Export Materials
		// ----------------

		wxLogMessage( wxString::Format( wxT( "Writing %s MTL file..." ), m_filename.GetName( ) ) );

		std::ostringstream matStream;

		matStream.imbue( std::locale( "C" ) );

		matStream << "# Gw2Browser MTL File: \'" << m_filename.GetName( ) << "\'" << std::endl;
		matStream << "# Material Count : " << model.numMaterial( ) << std::endl;
		matStream << std::endl;

		//for ( uint i = 0; i < model.numMeshes( ); i++ ) {
		for ( uint i = 0; i < model.numMaterial( ); i++ ) {
			//const Mesh& mesh = model.mesh( i );
			//auto materialIndex = mesh.materialIndex;
			//const Material& material = model.material( materialIndex );
			const Material& material = model.material( i );

			matStream << "# Material " << i + 1 << std::endl;
			matStream << "# Material ID : " << material.materialId << std::endl;
			matStream << "# Material flags : " << material.materialFlags << std::endl;
			matStream << "# Material file : " << material.materialFile << std::endl;
			if ( material.diffuseMap ) {
				matStream << "# Diffuse map texture : " << material.diffuseMap << std::endl;
			}
			if ( material.normalMap ) {
				matStream << "# Normal map texture : " << material.normalMap << std::endl;
			}
			if ( material.lightMap ) {
				matStream << "# Light map texture : " << material.lightMap << std::endl;
			}

			// Define new material
			//matStream << "newmtl " << mesh.materialName.c_str( ) << std::endl;
			matStream << "newmtl " << "mat" << i + 1 << std::endl;

			// Ambient color
			matStream << "Ka 1.000 1.000 1.000" << std::endl;
			// Diffuse color
			matStream << "Kd 1.000 1.000 1.000" << std::endl;
			// Specular color
			matStream << "Ks 0.000 0.000 0.000" << std::endl;
			// Specular exponent (ranges between 0 and 1000)
			matStream << "Ns 0" << std::endl;
			// Transparent
			matStream << "d 1" << std::endl;
			// Illumination model
			matStream << "illum 2" << std::endl;

			// Load diffuse texture
			if ( material.diffuseMap ) {
				// Ambient texture map
				matStream << "map_Ka " << material.diffuseMap << ".png" << std::endl;
				// Diffuse texture map ( most of the time, it will be the same as the ambient texture map )
				matStream << "map_Kd " << material.diffuseMap << ".png" << std::endl;
				// Specular color texture map
				matStream << "map_Ks " << material.diffuseMap << ".png" << std::endl;
			}

			// newline before next material!
			matStream << std::endl;
		}

		// Material None
		matStream << "newmtl " << "None" << std::endl;
		matStream << "Ka 1.000 1.000 1.000" << std::endl;
		matStream << "Kd 1.000 1.000 1.000" << std::endl;
		matStream << "Ks 0.000 0.000 0.000" << std::endl;
		matStream << "Ns 0" << std::endl;
		matStream << "d 1" << std::endl;
		matStream << "illum 2" << std::endl;

		// Close stream
		matStream.flush( );
		wxString matString( matStream.str( ) );
		matStream.clear( );

		// Convert string to byte array
		Array<byte> matData( matString.length( ) );
		::memcpy( matData.GetPointer( ), matString.c_str( ), matString.length( ) );

		// Set file extension to .mtl
		m_filename.SetExt( wxT( "mtl" ) );

		// Write to file
		this->writeFile( matData );

		// ---------------
		// Export Textures
		// ---------------

		// Exported texture(s) is in the same directory as exported model.

		wxLogMessage( wxString::Format( wxT( "Exporting %s's textures..." ), m_filename.GetName( ) ) );

		std::vector<uint32> textureFileList;

		for ( uint i = 0; i < model.numMaterial( ); i++ ) {
			auto material = model.material( i );
			if ( material.diffuseMap ) {
				textureFileList.push_back( material.diffuseMap );
			}
			if ( material.normalMap ) {
				textureFileList.push_back( material.normalMap );
			}
			if ( material.lightMap ) {
				textureFileList.push_back( material.lightMap );
			}
		}

		std::vector<uint32>::iterator temp;

		std::sort( textureFileList.begin( ), textureFileList.end( ) );
		temp = std::unique( textureFileList.begin( ), textureFileList.end( ) );
		textureFileList.resize( std::distance( textureFileList.begin( ), temp ) );

		// Create directory if not exist
		if ( !m_filename.DirExists( ) ) {
			m_filename.Mkdir( 511, wxPATH_MKDIR_FULL );
		}

		// Extract textures
		for ( auto& it : textureFileList ) {
			this->exportModelTexture( it );
		}

		wxLogMessage( wxString::Format( wxT( "Finish export model %s." ), m_filename.GetName( ) ) );
	}

	void Exporter::exportModelTexture( uint32 p_fileid ) {
		auto entryNumber = m_datFile.entryNumFromFileId( p_fileid );
		auto fileData = m_datFile.readEntry( entryNumber );

		// Bail if read failed
		if ( !fileData.GetSize( ) ) {
			wxLogMessage( wxString::Format( wxT( "File id %d is empty or not exist." ), p_fileid ) );
			return;
		}

		m_filename.SetName( wxString::Format( wxT( "%d" ), p_fileid ) );
		m_filename.SetExt( wxT( "png" ) );

		if ( m_filename.FileExists( ) ) {
			wxLogMessage( wxString::Format( wxT( "File %s is already exists, skiping." ), m_filename.GetFullPath( ) ) );
			return;
		}

		// Convert to image
		ANetFileType fileType;
		m_datFile.identifyFileType( fileData.GetPointer( ), fileData.GetSize( ), fileType );
		auto reader = FileReader::readerForData( fileData, m_datFile, fileType );

		wxLogMessage( wxString::Format( wxT( "Writing texture file %s." ), m_filename.GetFullPath( ) ) );

		this->exportImage( reader, wxT( "Dummy" ) );

		deletePointer( reader );
	}

	bool Exporter::writeFile( const Array<byte>& p_data ) {
		// Open file for writing
		wxFile file( m_filename.GetFullPath( ), wxFile::write );
		if ( file.IsOpened( ) ) {
			file.Write( p_data.GetPointer( ), p_data.GetSize( ) );
		} else {
			wxMessageBox( wxString::Format( wxT( "Failed to open the file %s for writing." ), m_filename.GetFullPath( ) ),
				wxT( "Error" ),
				wxOK | wxICON_ERROR );
			wxLogMessage( wxString::Format( wxT( "Failed to open the file %s for writing." ), m_filename.GetFullPath( ) ) );
			return false;
		}
		file.Close( );
		return true;
	}

	void Exporter::appendPaths( wxFileName& p_path, const DatIndexCategory& p_category ) {
		auto parent = p_category.parent( );
		if ( parent ) {
			this->appendPaths( p_path, *parent );
		}
		p_path.AppendDir( p_category.name( ) );
	}

}; // namespace gw2b
