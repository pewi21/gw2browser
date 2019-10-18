/** \file       Viewers/ModelViewer/Texture2D.h
 *  \brief      Contains the declaration of the Texture2D class.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2016 Khralkatorrix <https://github.com/kytulendu>
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

#pragma once

#ifndef VIEWERS_MODELVIEWER_TEXTURE2D_H_INCLUDED
#define VIEWERS_MODELVIEWER_TEXTURE2D_H_INCLUDED

#include "DatFile.h"

namespace gw2b {

    class Texture2D {
        uint                        m_fileId;           // File id in DatFile of this texture
        GLuint                      m_textureId;        // Texture ID
        GLenum                      m_textureType;      // Type identifier of texture

    public:
        /** Constructor. Create texture, default to create GL_TEXTURE_2D texture.
        *  \param[in]  p_datFile    Reference to an instance of DatFile.
        *  \param[in]  p_fileId     File id to load texture from DatFile.
        *  \param[in]  p_textureType   Texture type.
        *  \param[in]  p_wrapS      Texture wrap parameter.
        *  \param[in]  p_wrapT      Texture wrap parameter.
        *  \param[in]  p_anisotropic   Use anisotropic texture filtering? */
        Texture2D( DatFile& p_datFile, const uint p_fileId,
            const GLenum p_textureType = GL_TEXTURE_2D,
            const GLint p_wrapS = GL_REPEAT,
            const GLint p_wrapT = GL_REPEAT,
            const bool p_anisotropic = true
        );
        /** Destructor. Clears all data. */
        ~Texture2D( );

        /** Bind the texture. */
        void bind( );
        /** Un-bind the texture. */
        void unbind( );
        /** Get texture ID represented by this data.
        *  \return GLuint           textureId. */
        GLuint getTextureId( ) const;
        /** Get texture file ID from DatFile represented by this data.
        *  \return uint             fileId. */
        uint getFileId( ) const;

    private:
        /** Generate texture. */
        void generate( );
        /** Set texture wrap parameter.
        *  \param[in]  p_wrapS      Texture wrap parameter.
        *  \param[in]  p_wrapT      Texture wrap parameter. */
        void setWraping( const GLint p_wrapS, const GLint p_wrapT );
        /** Set texture filtering.
        *  \param[in]  p_anisotropic   Anisotropic texture filtering? */
        void setFiltering( const bool p_anisotropic );

    }; // class Texture2D

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_TEXTURE2D_H_INCLUDED
