/** \file       Viewers/ModelViewer/ShaderManager.h
 *  \brief      Contains the declaration of the shader manager class.
 *  \author     Khral Steelforge
 */

/**
 * Copyright (C) 2017 Khral Steelforge <https://github.com/kytulendu>
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

#ifndef VIEWERS_MODELVIEWER_SHADERMANAGER_H_INCLUDED
#define VIEWERS_MODELVIEWER_SHADERMANAGER_H_INCLUDED

#include <map>
#include <string>

#include "Shader.h"

namespace gw2b {

    class ShaderManager {

        std::map<std::string, Shader*>  m_shaders;

    public:
        /** Constructor. */
        ShaderManager( );
        /** Destructor. */
        ~ShaderManager( );

        /** Clear all the shader. */
        void clear( );

        /** Add a shader.
        *  \param[in]  p_name           Name of the shader to create.
        *  \param[in]  p_vertexPath     Path to vertex shader file.
        *  \param[in]  p_fragmentPath   Path to fragment shader file.
        *  \param[in]  p_geometryPath   Path to geometry shader file. */
        bool add( const std::string& p_name, const char* p_vertexPath, const char* p_fragmentPath, const char* p_geometryPath = nullptr );
        /** Find and get shader by shader name.
        *  \param[in]  p_name           Name of the shader to create.
        *  \param[in]  p_vertexPath     Path to vertex shader file.
        *  \param[in]  p_fragmentPath   Path to fragment shader file.
        *  \param[in]  p_geometryPath   Path to geometry shader file. */
        Shader* get( const std::string& p_name );
        /** Check if the object shader is empty */
        bool empty( );

    }; // class ShaderManager

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_SHADERMANAGER_H_INCLUDED
