/* \file       Viewers/ModelViewer/VertexBuffer.cpp
*  \brief      Contains the definition of the vertex buffer class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2018 Khral Steelforge <https://github.com/kytulendu>

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

#include "VertexBuffer.h"

namespace gw2b {

    VertexBuffer::VertexBuffer( std::vector<glm::vec3> p_vertices ) {
        // Generate Vertex Array Object
        glGenVertexArrays( 1, &m_vao );
        // Bind Vertex Array Object
        this->bind( );

        // Generate buffers
        glGenBuffers( 1, &m_vertexBuffer );

        // Load data into vertex buffers
        // Vertex Positions
        glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_vertices.size( ) * sizeof( glm::vec3 ), &p_vertices.front( ), GL_STATIC_DRAW );

        // Unbind Buffer Object
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        // Unbind Vertex Array Object
        this->unbind( );
    }

    VertexBuffer::VertexBuffer( std::vector<glm::vec3> p_vertices, std::vector<glm::vec2> p_uvs ) {
        // Generate Vertex Array Object
        glGenVertexArrays( 1, &m_vao );
        // Bind Vertex Array Object
        this->bind( );

        // Generate buffers
        glGenBuffers( 1, &m_vertexBuffer );
        glGenBuffers( 1, &m_uvBuffer );

        // Load data into vertex buffers
        // Vertex Positions
        glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_vertices.size( ) * sizeof( glm::vec3 ), p_vertices.data( ), GL_STATIC_DRAW );
        // Vertex Texture Coords
        glBindBuffer( GL_ARRAY_BUFFER, m_uvBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_uvs.size( ) * sizeof( glm::vec2 ), p_uvs.data( ), GL_STATIC_DRAW );

        // Unbind Buffer Object
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        // Unbind Vertex Array Object
        this->unbind( );
    }

    VertexBuffer::VertexBuffer( std::vector<glm::vec3> p_vertices, std::vector<glm::vec3> p_normals ) {
        // Generate Vertex Array Object
        glGenVertexArrays( 1, &m_vao );
        // Bind Vertex Array Object
        this->bind( );

        // Generate buffers
        glGenBuffers( 1, &m_vertexBuffer );
        glGenBuffers( 1, &m_normalBuffer );

        // Load data into vertex buffers
        // Vertex Positions
        glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_vertices.size( ) * sizeof( glm::vec3 ), p_vertices.data( ), GL_STATIC_DRAW );
        // Vertex Normals
        glBindBuffer( GL_ARRAY_BUFFER, m_normalBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_normals.size( ) * sizeof( glm::vec3 ), p_normals.data( ), GL_STATIC_DRAW );

        // Unbind Buffer Object
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        // Unbind Vertex Array Object
        this->unbind( );
    }

    VertexBuffer::VertexBuffer( std::vector<glm::vec3> p_vertices, std::vector<glm::vec3> p_normals, std::vector<glm::vec2> p_uvs ) {
        // Generate Vertex Array Object
        glGenVertexArrays( 1, &m_vao );
        // Bind Vertex Array Object
        this->bind( );

        // Generate buffers
        glGenBuffers( 1, &m_vertexBuffer );
        glGenBuffers( 1, &m_normalBuffer );
        glGenBuffers( 1, &m_uvBuffer );

        // Load data into vertex buffers
        // Vertex Positions
        glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_vertices.size( ) * sizeof( glm::vec3 ), p_vertices.data( ), GL_STATIC_DRAW );
        // Vertex Normals
        glBindBuffer( GL_ARRAY_BUFFER, m_normalBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_normals.size( ) * sizeof( glm::vec3 ), p_normals.data( ), GL_STATIC_DRAW );
        // Vertex Texture Coords
        glBindBuffer( GL_ARRAY_BUFFER, m_uvBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_uvs.size( ) * sizeof( glm::vec2 ), p_uvs.data( ), GL_STATIC_DRAW );

        // Unbind Buffer Object
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        // Unbind Vertex Array Object
        this->unbind( );
    }

    VertexBuffer::VertexBuffer( std::vector<glm::vec3> p_vertices, std::vector<glm::vec3> p_normals, std::vector<glm::vec2> p_uvs, std::vector<glm::vec3> p_tangents ) {
        // Generate Vertex Array Object
        glGenVertexArrays( 1, &m_vao );
        // Bind Vertex Array Object
        this->bind( );

        // Generate buffers
        glGenBuffers( 1, &m_vertexBuffer );
        glGenBuffers( 1, &m_normalBuffer );
        glGenBuffers( 1, &m_uvBuffer );
        glGenBuffers( 1, &m_tangentBuffer );

        // Load data into vertex buffers
        // Vertex Positions
        glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_vertices.size( ) * sizeof( glm::vec3 ), p_vertices.data( ), GL_STATIC_DRAW );
        // Vertex Normals
        glBindBuffer( GL_ARRAY_BUFFER, m_normalBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_normals.size( ) * sizeof( glm::vec3 ), p_normals.data( ), GL_STATIC_DRAW );
        // Vertex Texture Coords
        glBindBuffer( GL_ARRAY_BUFFER, m_uvBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_uvs.size( ) * sizeof( glm::vec2 ), p_uvs.data( ), GL_STATIC_DRAW );
        // Vertex Tangents
        glBindBuffer( GL_ARRAY_BUFFER, m_tangentBuffer );
        glBufferData( GL_ARRAY_BUFFER, p_tangents.size( ) * sizeof( glm::vec3 ), p_tangents.data( ), GL_STATIC_DRAW );

        // Unbind Buffer Object
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Unbind Vertex Array Object
        this->unbind( );
    }

    VertexBuffer::~VertexBuffer( ) {
        if ( m_vertexBuffer ) {
            glDeleteBuffers( 1, &m_vertexBuffer );
        }
        if ( m_normalBuffer ) {
            glDeleteBuffers( 1, &m_normalBuffer );
        }
        if ( m_uvBuffer ) {
            glDeleteBuffers( 1, &m_uvBuffer );
        }
        if ( m_tangentBuffer ) {
            glDeleteBuffers( 1, &m_tangentBuffer );
        }
    }

    void VertexBuffer::bind( ) {
        glBindVertexArray( m_vao );
    }

    void VertexBuffer::unbind( ) {
        glBindVertexArray( 0 );
    }

    void VertexBuffer::setVertexAttribPointer( ) {
        // Set the vertex attribute pointers
        // positions
        glEnableVertexAttribArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );
        // normals
        if ( m_normalBuffer ) {
            glEnableVertexAttribArray( 1 );
            glBindBuffer( GL_ARRAY_BUFFER, m_normalBuffer );
            glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );
        }
        // texCoords
        if ( m_uvBuffer ) {
            if ( m_normalBuffer ) {
                glEnableVertexAttribArray( 2 );
                glBindBuffer( GL_ARRAY_BUFFER, m_uvBuffer );
                glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );
            } else {    // No normals
                glEnableVertexAttribArray( 1 );
                glBindBuffer( GL_ARRAY_BUFFER, m_uvBuffer );
                glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );
            }
        }
        // tangents
        if ( m_tangentBuffer ) {
            glEnableVertexAttribArray( 3 );
            glBindBuffer( GL_ARRAY_BUFFER, m_tangentBuffer );
            glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, 0, ( GLvoid* ) 0 );
        }
    }

    GLuint VertexBuffer::getVAO( ) {
        return m_vao;
    }

}; // namespace gw2b
