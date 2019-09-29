/** \file       Viewers/ModelViewer/IndexBuffer.h
 *  \brief      Contains the declaration of the index buffer class.
 *  \author     Khral Steelforge
 */

/**
 * Copyright (C) 2018 Khral Steelforge <https://github.com/kytulendu>
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

#ifndef VIEWERS_MODELVIEWER_INDEXBUFFER_H_INCLUDED
#define VIEWERS_MODELVIEWER_INDEXBUFFER_H_INCLUDED

namespace gw2b {

    class IndexBuffer {
        GLuint                      m_ibo;              // Index Buffer Object

    public:
        /** Constructor. Create Index Buffer Object.
        *  \param[in]  p_indices    Triangle indices. */
        IndexBuffer( std::vector<uint> p_indices );
        /** Destructor. Clears all data. */
        ~IndexBuffer( );

        /** Bind the Index Buffer Object. */
        void bind( );
        /** Un-bind the Index Buffer Object. */
        void unbind( );
        /** Get Index Buffer Object ID represented by this data.
        *  \return GLuint           IBO id. */
        GLuint getBufferId( );

    }; // class IndexBuffer

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_INDEXBUFFER_H_INCLUDED
