/* \file       Viewers/ModelViewer/Texture2D.h
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

#pragma once

#ifndef VIEWERS_MODELVIEWER_TEXTURE2D_H_INCLUDED
#define VIEWERS_MODELVIEWER_TEXTURE2D_H_INCLUDED

#include "DatFile.h"

namespace gw2b {

	class Texture2D {
		uint						m_fileId;			// File id in DatFile of this texture
		GLuint						m_textureId;		// Texture ID
		GLenum						m_textureType;		// Type identifier of texture

	public:
		/** Constructor. Create texture, default to create GL_TEXTURE_2D texture.
		*  \param[in]  p_datFile    Reference to an instance of DatFile.
		*  \param[in]  p_fileId     File id to load texture from DatFile.
		*  \param[in]  p_textureType   Texture type. */
		Texture2D( DatFile& p_datFile, const uint p_fileId, const GLenum p_textureType = GL_TEXTURE_2D );
		/** Destructor. Clears all data. */
		~Texture2D( );

		/** Bind the texture. */
		void bind( );
		/** Get texture ID represented by this data.
		*  \return GLuint			textureId. */
		GLuint getTextureId( );
		/** Get texture file ID from DatFile represented by this data.
		*  \return uint				fileId. */
		uint getFileId( );

	private:
		/** Set texture parameters. */
		void init( );

	}; // class Texture2D

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_TEXTURE2D_H_INCLUDED
