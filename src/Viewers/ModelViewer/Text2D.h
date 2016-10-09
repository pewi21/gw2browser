/* \file       Viewers/ModelViewer/Text2D.h
*  \brief      Contains the declaration of the Text2D class.
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

#ifndef VIEWERS_MODELVIEWER_TEXT2D_H_INCLUDED
#define VIEWERS_MODELVIEWER_TEXT2D_H_INCLUDED

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"

namespace gw2b {

	class Text2D {
		struct Character;

		std::map<GLchar, Character> m_characterTextureMap;
		GLuint						m_textVAO;
		GLuint						m_textVBO;
		Shader*						m_textShader;
		wxSize						m_ClientSize;

	public:
		// Constructor
		Text2D( );
		// Destructor
		~Text2D( );

		bool init( );
		void clear( );
		void drawText( const wxString& p_text, GLfloat p_x, GLfloat p_y, GLfloat p_scale, glm::vec3 p_color );
		bool loadFont( std::map<GLchar, Character>& p_characters, const char *p_fontFile, const FT_UInt p_height );

		void setClientSize( const wxSize& p_size );

	}; // class Text2D

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_TEXT2D_H_INCLUDED
