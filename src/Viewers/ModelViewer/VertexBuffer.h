/** \file       Viewers/ModelViewer/VertexBuffer.h
 *  \brief      Contains the declaration of the vertex buffer class.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2018 Khralkatorrix <https://github.com/kytulendu>
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

#ifndef VIEWERS_MODELVIEWER_VERTEXBUFFER_H_INCLUDED
#define VIEWERS_MODELVIEWER_VERTEXBUFFER_H_INCLUDED

namespace gw2b {

    class VertexBuffer {
        GLuint                      m_vao;              // Vertex Array Object
        GLuint                      m_vertexBuffer;
        GLuint                      m_normalBuffer;
        GLuint                      m_uvBuffer;
        GLuint                      m_tangentBuffer;

    public:
        /** Constructor. Create Index Buffer Object.
        *  \param[in]  p_vertices   Vertex Positions. */
        VertexBuffer( std::vector<glm::vec3> p_vertices );
        /** Constructor. Create Index Buffer Object.
        *  \param[in]  p_vertices   Vertex Positions.
        *  \param[in]  p_uvs        Vertex Texture Coords. */
        VertexBuffer( std::vector<glm::vec3> p_vertices, std::vector<glm::vec2> p_uvs );
        /** Constructor. Create Index Buffer Object.
        *  \param[in]  p_vertices   Vertex Positions.
        *  \param[in]  p_normals    Vertex Normals. */
        VertexBuffer( std::vector<glm::vec3> p_vertices, std::vector<glm::vec3> p_normals );
        /** Constructor. Create Index Buffer Object.
        *  \param[in]  p_vertices   Vertex Positions.
        *  \param[in]  p_normals    Vertex Normals.
        *  \param[in]  p_uvs        Vertex Texture Coords. */
        VertexBuffer( std::vector<glm::vec3> p_vertices, std::vector<glm::vec3> p_normals, std::vector<glm::vec2> p_uvs );
        /** Constructor. Create Index Buffer Object.
        *  \param[in]  p_vertices   Vertex Positions.
        *  \param[in]  p_normals    Vertex Normals.
        *  \param[in]  p_uvs        Vertex Texture Coords.
        *  \param[in]  p_tangents   Vertex Tangents. */
        VertexBuffer( std::vector<glm::vec3> p_vertices, std::vector<glm::vec3> p_normals, std::vector<glm::vec2> p_uvs, std::vector<glm::vec3> p_tangents );
        /** Destructor. Clears all data. */
        ~VertexBuffer( );

        /** Bind the Index Buffer Object. */
        void bind( );
        /** Un-bind the Index Buffer Object. */
        void unbind( );
        /** Set the vertex attribute pointers. */
        void setVertexAttribPointer( );
        /** Get Vertex Array Object represented by this data.
        *  \return GLuint           VAO id. */
        GLuint getVAO( );

    }; // class VertexBuffer

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_VERTEXBUFFER_H_INCLUDED
