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
//#include <wx/txtstrm.h>
#include <wx/wfstream.h>


#include "DatFile.h"
#include "DatIndex.h"
#include "FileReader.h"
//#include "ExtractFilesWindow.h"
#include "Readers/ImageReader.h"
#include "Readers/StringReader.h"
#include "Readers/ModelReader.h"
#include "Readers/PackedSoundReader.h"
#include "Readers/asndMP3Reader.h"

#include "Exporter.h"

namespace gw2b {

	Exporter::Exporter( const Array<const DatIndexEntry*>& p_entries, DatFile& p_datFile, ExtractionMode p_mode )
		: wxFrame( nullptr, wxID_ANY, wxT( "ExporterWindow" ) )
		, m_datFile( p_datFile )
		, m_entries( p_entries )
		, m_mode( p_mode )
		, m_fileType( ANFT_Unknown ) {

		// If it's just one file, we could handle it here
		if ( p_entries.GetSize( ) == 1 ) {
			auto entry = p_entries[0];
			// Convert and export file
			this->extractFile( *entry );

		// More than one files
		} else {
			wxMessageBox( wxT( "Not implemented!" ), wxT( "" ), wxOK | wxICON_WARNING );

			// Convert and export file
			//this->extractFiles( p_entries, dialog.GetPath( ) );
			/*
			wxDirDialog dialog( this, wxT( "Select output folder" ) );
			if ( dialog.ShowModal( ) == wxID_OK ) {
				if ( p_mode ) {
					new ExtractFilesWindow( entries, m_datFile, dialog.GetPath( ), ExtractFilesWindow::EM_Converted );
				} else {
					new ExtractFilesWindow( entries, m_datFile, dialog.GetPath( ), ExtractFilesWindow::EM_Raw );
				}
			}*/
		}
	}

	Exporter::~Exporter( ) {

	}

	const wxChar* Exporter::GetExtension( const ExtractionMode p_mode ) const {
		if ( p_mode == EM_Converted ) {
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
				return wxT( ".png" );
				break;
			case ANFT_Model:
				return wxT( ".obj" );
				break;
			case ANFT_StringFile:
				return wxT( ".csv" );
				break;
			case ANFT_PackedOgg:
			case ANFT_Ogg:
				return wxT( ".ogg" );
				break;
			case ANFT_PackedMP3:
			case ANFT_asndMP3:
			case ANFT_MP3:
				return wxT( ".mp3" );
				break;
			default:
				return wxT( ".raw" ); // todo
				break;
			}
		} else {
			switch ( this->m_fileType ) {
				// Texture
			case ANFT_ATEX:
				return wxT( ".atex" );
				break;
			case ANFT_ATTX:
				return wxT( ".attx" );
				break;
			case ANFT_ATEC:
				return wxT( ".atec" );
				break;
			case ANFT_ATEP:
				return wxT( ".atep" );
				break;
			case ANFT_ATEU:
				return wxT( ".ateu" );
				break;
			case ANFT_ATET:
				return wxT( ".atet" );
				break;
			case ANFT_DDS:
				return wxT( ".dds" );
				break;
			case ANFT_JPEG:
				return wxT( ".jpg" );
				break;
			case ANFT_WEBP:
				return wxT( ".webp" );
				break;

				// String files
			case ANFT_StringFile:
				return wxT( ".strs" );
				break;
			case ANFT_EULA:
				return wxT( ".eula" );
				break;

				// Maps stuff
			case ANFT_MapContent:
				return wxT( ".cntc" );
				break;
			case ANFT_MapParam:
				return wxT( ".parm" );
				break;
			case ANFT_MapShadow:
				return wxT( ".mpsd" );
				break;
			case ANFT_PagedImageTable:
				return wxT( ".pimg" );
				break;

			case ANFT_Material:
				return wxT( ".amat" );
				break;
			case ANFT_Composite:
				return wxT( ".cmpc" );
				break;
			case ANFT_HavokCollision:
				return wxT( ".havk" );
				break;
			case ANFT_AnimSequences:
				return wxT( ".seqn" );
				break;
			case ANFT_EmoteAnimation:
				return wxT( ".emoc" );
				break;

			case ANFT_Cinematic:
				return wxT( ".cinp" );
				break;

			case ANFT_PortalManifest:
				return wxT( ".prlt" );
				break;

			case ANFT_TextPackManifest:
				return wxT( ".txtm" );
				break;
			case ANFT_TextPackVariant:
				return wxT( ".txtvar" );
				break;
			case ANFT_TextPackVoices:
				return wxT( ".txtv" );
				break;

				// Sound
			case ANFT_Sound:
			case ANFT_asndMP3:
			case ANFT_asndOgg:
				return wxT( ".sound" );
				break;
			case ANFT_PackedMP3:
			case ANFT_PackedOgg:
				return wxT( ".asnd" );
				break;
			case ANFT_MP3:
				return wxT( ".mp3" );
				break;
			case ANFT_Ogg:
				return wxT( ".ogg" );
				break;

				// Audio script
			case ANFT_AudioScript:
				return wxT( ".amsp" );
				break;

				// Font file
			case ANFT_FontFile:
				return wxT( ".eot" );	// Embedded OpenType
				break;

				// Bink2 Video file
			case ANFT_Bink2Video:
				return wxT( ".bk2" );
				break;

			case ANFT_ShaderCache:
				return wxT( ".cdhs" );
				break;

			case ANFT_Model:
				return wxT( ".modl" );
				break;

				// Binary
			case ANFT_DLL:
				return wxT( ".dll" );
				break;
			case ANFT_EXE:
				return wxT( ".exe" );
				break;

			case ANFT_Config:
				return wxT( ".locl" );
				break;

			case ANFT_ARAP:
				return wxT( ".arap" );
				break;

			default:
				return wxT( ".raw" );
				break;
			}
		}
	}

	const char* Exporter::getWildCard( const ExtractionMode p_mode ) const {
		if ( p_mode == EM_Converted ) {
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
				return "Portable Network Graphics (*.png)|*.png";
				break;
			case ANFT_Model:
				return "WaveFront (*.obj)|*.obj";
				break;
			case ANFT_StringFile:
				return "Comma-separated values (*.csv)|*.csv";
				break;
			case ANFT_PackedOgg:
			case ANFT_Ogg:
				return "Ogg Vorbis (*.ogg)|*.ogg";
				break;
			case ANFT_PackedMP3:
			case ANFT_asndMP3:
			case ANFT_MP3:
				return "MP3 (*.mp3)|*.mp3";
				break;
			default:
				return "Raw Data (*.raw)|*.raw"; // todo
				break;
			}
		} else {
			return wxFileSelectorDefaultWildcardStr;
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

		auto ext = wxEmptyString;
		ext = this->GetExtension( m_mode );

		// Ask for location
		wxFileDialog dialog( this,
			wxString::Format( wxT( "Extract %s%s..." ), p_entry.name( ), ext ),
			wxEmptyString,
			p_entry.name( ) + ext,
			getWildCard( m_mode ),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

		// Open file for writing
		if ( dialog.ShowModal( ) == wxID_CANCEL ) {
			return;
		}

		auto reader = FileReader::readerForData( entryData, m_fileType );

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
				this->exportImage( reader, dialog.GetDirectory( ), p_entry.name( ) );
				break;
			case ANFT_PackedMP3:
			case ANFT_PackedOgg:
			case ANFT_asndMP3:
				this->exportSound( reader, dialog.GetPath( ) );
				break;
			case ANFT_Model:
				this->exportModel( reader, dialog.GetDirectory( ), p_entry.name( ) );
				break;
			case ANFT_StringFile:
				this->exportString( reader, dialog.GetPath( ) );
				break;
			default:
				entryData = reader->rawData( );
				this->writeFile( entryData, dialog.GetPath( ) );
				break;
			}
		} else {
			entryData = reader->rawData( );
			this->writeFile( entryData, dialog.GetPath( ) );
		}
		deletePointer( reader );
	}

	void extractFiles( const Array<const DatIndexEntry*>& p_entries, const wxString& p_path ) {

		/*
		wxFileName filename( m_path, p_entry.name( ) );
		this->appendPaths( filename, *p_entry.category( ) );

		// Create directory if in-existant
		if ( !filename.DirExists( ) ) {
		filename.Mkdir( 511, wxPATH_MKDIR_FULL );
		}

		// Read file contents
		auto contents = m_datFile.readFile( p_entry.mftEntry( ) );
		if ( !contents.GetSize( ) ) {
		return;
		}

		// Open file for writing
		wxFile file( filename.GetFullPath( ), wxFile::write );
		if ( file.IsOpened( ) ) {
		file.Write( contents.GetPointer( ), contents.GetSize( ) );
		}
		file.Close( );
		*/
	}

	void Exporter::exportImage( FileReader* p_reader, const wxString& p_directory, const wxString& p_fileId ) {
		// Bail if not an image
		auto imgReader = dynamic_cast<ImageReader*>( p_reader );
		if ( !imgReader ) {
			wxString message;
			message << p_fileId << " is not an image.";
			wxMessageBox( message, wxT( "Error" ), wxOK | wxICON_ERROR );
			return;
		}

		// Get image in wxImage
		auto imageData = imgReader->getImage( );

		if ( !imageData.IsOk( ) ) {
			wxString message;
			message << "Can not get " << p_fileId << " wxImage.";
			wxMessageBox( message, wxT( "Error" ), wxOK | wxICON_ERROR );
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

		// Get file name
		wxString filename;
		filename << p_directory << "\\" << p_fileId << this->GetExtension( m_mode );

		// Write to file
		this->writeFile( data, filename );
	}

	void Exporter::exportString( FileReader* p_reader, const wxString& p_filePath ) {
		// Bail if not a string
		auto strReader = dynamic_cast<StringReader*>( p_reader );
		if ( !strReader ) {
			wxMessageBox( wxT( "Not string file." ), wxT( "Error" ), wxOK | wxICON_ERROR );
			return;
		}

		// Get string
		auto string = strReader->getString( );

		if ( string.IsEmpty( ) ) {
			wxMessageBox( wxT( "Empty String." ), wxT( "Error" ), wxOK | wxICON_ERROR );
			return;
		}

		// Convert string to byte array
		Array<byte> data( string.length( ) );
		::memcpy( data.GetPointer( ), string.utf8_str( ), string.length( ) );

		// Write to file
		this->writeFile( data, p_filePath );
	}

	void Exporter::exportSound( FileReader* p_reader, const wxString& p_filePath ) {
		Array<byte> data;

		if ( m_fileType == ANFT_asndMP3 ) {

			auto asndMP3 = dynamic_cast<asndMP3Reader*>( p_reader );

			if ( !asndMP3 ) {
				wxMessageBox( wxT( "Not sound file." ), wxT( "Error" ), wxOK | wxICON_ERROR );
				return;
			}
			// Get sound data
			data = asndMP3->getMP3( );

		} else if (
			( m_fileType == ANFT_PackedMP3 ) ||
			( m_fileType == ANFT_PackedOgg )
			) {
			auto packedOgg = dynamic_cast<PackedSoundReader*>( p_reader );

			if ( !packedOgg ) {
				wxMessageBox( wxT( "Not sound file." ), wxT( "Error" ), wxOK | wxICON_ERROR );
				return;
			}
			// Get sound data
			data = packedOgg->getSound( );

		}

		// Write to file
		this->writeFile( data, p_filePath );
	}

	void Exporter::exportModel( FileReader* p_reader, const wxString& p_directory, const wxString& p_fileId ) {
		// Bail if not a model
		auto modlReader = dynamic_cast<ModelReader*>( p_reader );
		if ( !modlReader ) {
			wxMessageBox( wxT( "Not string file." ), wxT( "Error" ), wxOK | wxICON_ERROR );
			return;
		}

		// Get model data
		auto model = modlReader->getModel( );

		// -------------
		// Export meshes
		// -------------

		// Note: wxWidgets only does locale-specific number formatting. This does
		// not work well with obj-files.
		std::ostringstream modlStream;

		modlStream.imbue( std::locale( "C" ) );

		modlStream << "# Mesh Count : " << model.numMeshes( ) << std::endl;
		modlStream << "mtllib " << p_fileId << ".mtl" << std::endl;

		uint indexBase = 1;
		for ( uint i = 0; i < model.numMeshes( ); i++ ) {
			const Mesh& mesh = model.mesh( i );

			// Write header
			modlStream << std::endl << "# Mesh " << i + 1 << ": " << mesh.vertices.size( ) << " vertices, " << mesh.triangles.size( ) << " triangles" << std::endl;
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

			modlStream << "usemtl " << mesh.materialName.c_str( ) << std::endl;

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

		// Get file name
		wxString objFile;
		objFile << p_directory << "\\" << p_fileId << this->GetExtension( m_mode );

		// Write to file
		this->writeFile( meshData, objFile );
	}

	bool Exporter::writeFile( const Array<byte>& p_data, const wxString& p_filePath ) {
		wxFileOutputStream stream( p_filePath );
		if ( !stream.Write( p_data.GetPointer( ), p_data.GetSize( ) ) ) {
			wxMessageBox( wxT( "Failed to open the file for writing." ), wxT( "Error" ), wxOK | wxICON_ERROR );
			return false;
		}
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
