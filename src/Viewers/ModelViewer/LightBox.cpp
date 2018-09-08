/* \file       Viewers/ModelViewer/LightBox.cpp
*  \brief      Contains the declaration of the LightBox class.
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

#include "LightBox.h"

namespace gw2b {

    LightBox::LightBox( ) {
        // Load shader
        try {
            m_cubeShader = new Shader( "..//data//shaders//light_box.vert", "..//data//shaders//light_box.frag" );

            m_cubeShader->use( );
            // Get the uniform location
            m_uniformProjection = glGetUniformLocation( m_cubeShader->getProgramId( ), "projection" );
            m_uniformView = glGetUniformLocation( m_cubeShader->getProgramId( ), "view" );
            m_uniformModel = glGetUniformLocation( m_cubeShader->getProgramId( ), "model" );
            m_uniformLightColor = glGetUniformLocation( m_cubeShader->getProgramId( ), "lightColor" );

        } catch ( const exception::Exception& err ) {
            wxLogMessage( wxT( "Failed to load lightbox shader : %s" ), wxString( err.what( ) ) );
            throw exception::Exception( "Failed to load lightbox shader." );
        }

        GLfloat vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
        };

        glGenVertexArrays( 1, &m_cubeVAO );
        glGenBuffers( 1, &m_cubeVBO );
        // Fill buffer
        glBindBuffer( GL_ARRAY_BUFFER, m_cubeVBO );
        glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
        // Link vertex attributes
        glBindVertexArray( m_cubeVAO );
        // Position attribute
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), ( GLvoid* ) 0 );
        glEnableVertexAttribArray( 0 );

        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
    }

    LightBox::~LightBox( ) {
        // Cleanup
        if ( m_cubeVBO ) {
            glDeleteBuffers( 1, &m_cubeVBO );
        }
        if ( m_cubeVAO ) {
            glDeleteBuffers( 1, &m_cubeVAO );
        }
        if ( m_cubeShader ) {
            delete m_cubeShader;
        }
    }

    void LightBox::setViewMatrix( const glm::mat4& p_view ) {
        m_view = p_view;
    }

    void LightBox::setProjectionMatrix( const glm::mat4& p_projection ) {
        m_projection = p_projection;
    }

    void LightBox::renderCube( const glm::vec3& p_pos, const glm::vec3& p_color ) {
        glDisable( GL_CULL_FACE );

        m_cubeShader->use( );
        glUniformMatrix4fv( m_uniformProjection, 1, GL_FALSE, glm::value_ptr( m_projection ) );
        glUniformMatrix4fv( m_uniformView, 1, GL_FALSE, glm::value_ptr( m_view ) );
        auto model = glm::translate( glm::mat4( ), p_pos );
        model = glm::scale( model, glm::vec3( 6.0f ) );
        glUniformMatrix4fv( m_uniformModel, 1, GL_FALSE, glm::value_ptr( model ) );
        glUniform3fv( m_uniformLightColor, 1, glm::value_ptr( p_color ) );

        // Render Cube
        glBindVertexArray( m_cubeVAO );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
    }

}; // namespace gw2b
