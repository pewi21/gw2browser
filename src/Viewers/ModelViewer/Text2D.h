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
		GLuint						m_uniformProjection;
		GLuint						m_uniformTextColor;
		wxSize						m_ClientSize;

	public:
		/** Constructor. Create text renderer.
		*  \param[in]  p_fontfile    Path to font file.
		*  \param[in]  p_fontsize    Font size, default is 12 point. */
		Text2D( const char* p_fontfile = "..//data//fonts//LiberationSans-Regular.ttf", const FT_UInt p_fontsize = 12 );
		/** Destructor. Clears all data. */
		~Text2D( );

		/** Draw text at given position.
		*  \param[in]  p_text        Text to display.
		*  \param[in]  p_x           X position.
		*  \param[in]  p_y           Y position.
		*  \param[in]  p_scale       Text size.
		*  \param[in]  p_color       Color of text. */
		void drawText( const wxString& p_text, GLfloat p_x, GLfloat p_y, GLfloat p_scale, glm::vec3 p_color );
		/** Set viewport size. */
		void setClientSize( const wxSize& p_size );

	private:
		/** Load font and load font texture to character map.
		*  \param[in]  p_characters  Character texture map to load character texture to.
		*  \param[in]  p_fontFile    Path to font file.
		*  \param[in]  p_height      Font size.
		*  \return bool				 True if success, false if failed to load font. */
		bool loadFont( std::map<GLchar, Character>& p_characters, const char *p_fontFile, const FT_UInt p_height );

	}; // class Text2D

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_TEXT2D_H_INCLUDED
