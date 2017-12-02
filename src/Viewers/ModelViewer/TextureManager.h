/* \file       Viewers/ModelViewer/TextureManager.h
*  \brief      Contains the declaration of the texture manager class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2017 Khral Steelforge <https://github.com/kytulendu>

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

#ifndef VIEWERS_MODELVIEWER_TEXTUREMANAGER_H_INCLUDED
#define VIEWERS_MODELVIEWER_TEXTUREMANAGER_H_INCLUDED

#include <map>

#include "DatFile.h"
#include "Texture2D.h"

namespace gw2b {

	class TextureManager {

		std::map<uint32, Texture2D*> m_texture;

	public:
		/** Constructor. */
		TextureManager( );
		/** Destructor. */
		~TextureManager( );

		/** Clear all the texture. */
		void clear( );

		/** Load a texture.
		*  \param[in]  p_id			   File id of the texture to load. */
		bool load( DatFile& p_datFile, const uint32 p_id );
		/** Find and get texture by texture file id.
		*  \param[in]  p_id			   File id of the texture to get. */
		Texture2D* get( const uint32 p_id );
		/** Find and remove texture by texture file id.
		*  \param[in]  p_id			   File id of the texture to remove. */
		bool remove( const uint32 p_id );
		/** Check if the object is empty */
		bool empty( );

	}; // class TextureManager

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_TEXTUREMANAGER_H_INCLUDED
