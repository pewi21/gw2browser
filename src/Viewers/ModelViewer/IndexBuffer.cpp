/* \file       Viewers/ModelViewer/IndexBuffer.cpp
*  \brief      Contains the definition of the index buffer class.
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

#include "IndexBuffer.h"

namespace gw2b {

    IndexBuffer::IndexBuffer( std::vector<uint> p_indices ) {
        glGenBuffers( 1, &m_ibo );
        this->bind( );
        // Element Buffer for the indices
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, p_indices.size( ) * sizeof( uint ), &p_indices.front( ), GL_STATIC_DRAW );
        this->unbind( );
    }

    IndexBuffer::~IndexBuffer( ) {
        if ( m_ibo ) {
            glDeleteBuffers( 1, &m_ibo );
        }
    }

    void IndexBuffer::bind( ) {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
    }

    void IndexBuffer::unbind( ) {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    GLuint IndexBuffer::getBufferId( ) {
        return m_ibo;
    }

}; // namespace gw2b
