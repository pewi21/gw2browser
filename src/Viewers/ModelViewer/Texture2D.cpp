/* \file       Viewers/ModelViewer/Texture2D.cpp
*  \brief      Contains the declaration of the Texture2D class.
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

#include "Exception.h"
#include "Readers/ImageReader.h"

#include "Texture2D.h"

namespace gw2b {

	Texture2D::Texture2D( DatFile& p_datFile, const uint p_fileId, const GLenum p_textureType, const GLint p_wrapS, const GLint p_wrapT, const bool p_anisotropic )
		: m_fileId( p_fileId )
		, m_textureType( p_textureType ) {
		auto entryNumber = p_datFile.entryNumFromFileId( p_fileId );
		auto fileData = p_datFile.readEntry( entryNumber );

		// Bail if read failed
		if ( fileData.GetSize( ) == 0 ) {
			throw exception::Exception( "The file has no data." );
		}

		// Convert to image
		ANetFileType fileType;
		p_datFile.identifyFileType( fileData.GetPointer( ), fileData.GetSize( ), fileType );
		auto reader = FileReader::readerForData( fileData, p_datFile, fileType );

		// Bail if not an image
		auto imgReader = dynamic_cast<ImageReader*>( reader );
		if ( !imgReader ) {
			deletePointer( reader );
			throw exception::Exception( "The file is not an image." );
		}

		// Get image in wxImage
		auto imageData = imgReader->getImage( );
		if ( !imageData.IsOk( ) ) {
			deletePointer( reader );
			throw exception::Exception( "Failed to get image in wxImage." );
		}

		int imageWidth = imageData.GetWidth( );
		int imageHeight = imageData.GetHeight( );

		// Get color data
		GLubyte* bitmapData = imageData.GetData( );

		if ( imageData.HasAlpha( ) ) {
			// wxImage store seperate alpha channel if present
			GLubyte* alphaData = imageData.GetAlpha( );
			int bytesPerPixel = 4;
			int imageSize = imageWidth * imageHeight * bytesPerPixel;
			Array<GLubyte> image( imageSize );
			// Merge wxImage alpha channel to RGBA
#pragma omp parallel for
			for ( int y = 0; y < imageHeight; y++ ) {
				for ( int x = 0; x < imageWidth; x++ ) {
					::memcpy( &image[( x + y * imageWidth ) * bytesPerPixel], &bitmapData[( x + y * imageWidth ) * 3], 3 );
					image[( x + y * imageWidth ) * bytesPerPixel + 3] = alphaData[x + y * imageWidth];
				}
			}
			this->create( image.GetPointer( ), imageWidth, imageHeight, true, p_wrapS, p_wrapT, p_anisotropic );
		} else {
			this->create( bitmapData, imageWidth, imageHeight, false, p_wrapS, p_wrapT, p_anisotropic );
		}

		deletePointer( reader );
	}

	Texture2D::~Texture2D( ) {
		// Delete textures
		glDeleteTextures( 1, &m_textureId );
	}

	void Texture2D::bind( ) {
		glBindTexture( m_textureType, m_textureId );
	}

	void Texture2D::unbind( ) {
		glBindTexture( m_textureType, 0 );
	}

	GLuint Texture2D::getTextureId( ) {
		return m_textureId;
	}

	uint Texture2D::getFileId( ) {
		return m_fileId;
	}

	void Texture2D::create( const GLubyte* p_data, const GLsizei p_width, const GLsizei p_height, const bool p_alpha, const GLint p_wrapS, const GLint p_wrapT, const bool p_anisotropic ) {
		// Generate texture ID
		glGenTextures( 1, &m_textureId );

		// Assign texture to ID
		glBindTexture( m_textureType, m_textureId );

		// Texture wraping parameters
		glTexParameteri( m_textureType, GL_TEXTURE_WRAP_S, p_wrapS );
		glTexParameteri( m_textureType, GL_TEXTURE_WRAP_T, p_wrapT );

		// Trilinear texture filtering
		glTexParameteri( m_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

		if ( p_anisotropic ) {
			// Set anisotropic texture filtering to maximumn supported by GPU
			// No need to check if extension available since it is ubiquitous extension
			// https://www.opengl.org/registry/specs/EXT/texture_filter_anisotropic.txt
			GLfloat af = 0.0f;
			glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &af );
			glTexParameterf( m_textureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, af );
		}

		// Give the image to OpenGL
		glTexImage2D( m_textureType, 0, p_alpha ? GL_RGBA8 : GL_RGB8, p_width, p_height, 0, p_alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, p_data );

		// Generate Mipmaps
		glGenerateMipmap( m_textureType );

		glBindTexture( m_textureType, 0 );
	}

}; // namespace gw2b
