/** \file       Viewers/ModelViewer/Text2D.cpp
 *  \brief      Contains the declaration of the Text2D class.
 *  \author     Khral Steelforge
 */

/**
 * Copyright (C) 2016 Khral Steelforge <https://github.com/kytulendu>
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

#include "Exception.h"

#include "Text2D.h"

namespace gw2b {

    struct Text2D::Character {
        GLuint TextureID;       // ID handle of the glyph texture
        glm::ivec2 Size;        // Size of glyph
        glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
        GLuint Advance;         // Horizontal offset to advance to next glyph
    };

    Text2D::Text2D( const char* p_fontfile, const FT_UInt p_fontsize ) {
        // Load font
        if ( !loadFont( m_characterTextureMap, p_fontfile, p_fontsize ) ) {
            throw exception::Exception( "Failed to load font." );
        }

        // Load text shader
        try {
            m_textShader = new Shader( "../data/shaders/text.vert", "../data/shaders/text.frag" );

            m_textShader->use( );
            // Get the uniform location
            m_uniformProjection = glGetUniformLocation( m_textShader->getProgramId( ), "projection" );
            m_uniformTextColor = glGetUniformLocation( m_textShader->getProgramId( ), "textColor" );

        } catch ( const exception::Exception& err ) {
            wxLogMessage( wxT( "Failed to load text shader : %s" ), wxString( err.what( ) ) );
            throw exception::Exception( "Failed to load text shader." );
        }

        // Configure VAO/VBO for texture quads
        glGenVertexArrays( 1, &m_textVAO );
        glBindVertexArray( m_textVAO );

        glGenBuffers( 1, &m_textVBO );
        glBindBuffer( GL_ARRAY_BUFFER, m_textVBO );
        glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * 6 * 4, NULL, GL_DYNAMIC_DRAW );

        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
    }

    Text2D::~Text2D( ) {
        // Clean character textures
        for ( auto& it : m_characterTextureMap ) {
            if ( it.first ) {
                glDeleteTextures( 1, &it.second.TextureID );
            }
        }

        // Clean shaders
        delete m_textShader;

        // Clean VAO
        glDeleteVertexArrays( 1, &m_textVAO );
    }

    void Text2D::drawText( const wxString& p_text, GLfloat p_x, GLfloat p_y, GLfloat p_scale, glm::vec3 p_color ) {
        // Disable depth-testing
        glDisable( GL_DEPTH_TEST );
        // Enable blending
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        // Activate corresponding render state
        m_textShader->use( );

        glm::mat4 projection = glm::ortho( 0.0f, static_cast<GLfloat>( m_clientSize.x ), 0.0f, static_cast<GLfloat>( m_clientSize.y ) );
        glUniformMatrix4fv( m_uniformProjection, 1, GL_FALSE, glm::value_ptr( projection ) );

        glUniform3fv( m_uniformTextColor, 1, glm::value_ptr( p_color ) );

        glActiveTexture( GL_TEXTURE0 );
        glBindVertexArray( m_textVAO );

        // Iterate through all characters
        for ( auto const& c : p_text ) {
            Character ch = m_characterTextureMap[c];

            GLfloat xpos = p_x + ch.Bearing.x * p_scale;
            GLfloat ypos = p_y - ( ch.Size.y - ch.Bearing.y ) * p_scale;

            GLfloat w = ch.Size.x * p_scale;
            GLfloat h = ch.Size.y * p_scale;
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { xpos, ypos + h, 0.0, 0.0 },
                { xpos, ypos, 0.0, 1.0 },
                { xpos + w, ypos, 1.0, 1.0 },

                { xpos, ypos + h, 0.0, 0.0 },
                { xpos + w, ypos, 1.0, 1.0 },
                { xpos + w, ypos + h, 1.0, 0.0 }
            };
            // Render glyph texture over quad
            glBindTexture( GL_TEXTURE_2D, ch.TextureID );
            // Update content of Vertex Buffer Object memory
            glBindBuffer( GL_ARRAY_BUFFER, m_textVBO );
            glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices ); // Be sure to use glBufferSubData and not glBufferData

            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            // Render quad
            glDrawArrays( GL_TRIANGLES, 0, 6 );
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            p_x += ( ch.Advance >> 6 ) * p_scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray( 0 );
        glBindTexture( GL_TEXTURE_2D, 0 );

        // Disable blending
        glDisable( GL_BLEND );
        // Enable depth-testing
        glEnable( GL_DEPTH_TEST );
    }

    void Text2D::setClientSize( const wxSize& p_size ) {
        m_clientSize = p_size;
    }

    bool Text2D::loadFont( std::map<GLchar, Character>& p_characters, const char *p_fontFile, const FT_UInt p_height ) {
        // FreeType
        FT_Library ft;

        // All functions return a value different than 0 whenever an error occurred
        if ( FT_Init_FreeType( &ft ) ) {
            wxLogMessage( wxT( "FreeType: Could not initialize FreeType library." ) );
            return false;
        }

        // Load font as face
        FT_Face face;
        if ( FT_New_Face( ft, p_fontFile, 0, &face ) ) {
            wxLogMessage( wxT( "FreeType: Failed to load font." ) );
            return false;
        }

        // Set size to load glyphs as
        FT_Set_Pixel_Sizes( face, 0, p_height );

        // Disable byte-alignment restriction
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

        // Create 128 Charr charracters
        for ( unsigned char charr = 0; charr < 128; charr++ ) {
            // Load character glyph
            if ( FT_Load_Char( face, charr, FT_LOAD_RENDER ) ) {
                wxLogMessage( wxT( "FreeType: Failed to load Glyph." ) );
                continue;
            }

            // Generate texture
            GLuint texture;
            glGenTextures( 1, &texture );
            glBindTexture( GL_TEXTURE_2D, texture );
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
                );

            // Set texture options
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

            // Now store character for later use
            Character charracter = {
                texture,
                glm::ivec2( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
                glm::ivec2( face->glyph->bitmap_left, face->glyph->bitmap_top ),
                static_cast<GLuint>( face->glyph->advance.x )
            };
            p_characters.insert( std::pair<GLchar, Character>( charr, charracter ) );
        }
        glBindTexture( GL_TEXTURE_2D, 0 );

        // Destroy FreeType once we're finished
        FT_Done_Face( face );
        FT_Done_FreeType( ft );

        return true;
    }

}; // namespace gw2b
