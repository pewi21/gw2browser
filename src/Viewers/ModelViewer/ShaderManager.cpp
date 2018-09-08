/* \file       Viewers/ModelViewer/ShaderManager.cpp
*  \brief      Contains the declaration of the shader manager class.
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

#include "stdafx.h"

#include <utility>

#include "Exception.h"

#include "ShaderManager.h"

namespace gw2b {

    ShaderManager::ShaderManager( ) {

    }

    ShaderManager::~ShaderManager( ) {
        this->clear( );
    }

    void ShaderManager::clear( ) {
        for ( auto it : m_shaders ) {
            delete it.second;
        }
        m_shaders.clear( );
    }

    bool ShaderManager::add( const std::string& p_name, const char* p_vertexPath, const char* p_fragmentPath, const char* p_geometryPath ) {
        auto it = m_shaders.find( p_name );
        if ( it == m_shaders.end( ) ) {
            try {
                m_shaders.insert( std::pair<std::string, Shader*>( p_name, new Shader( p_vertexPath, p_fragmentPath, p_geometryPath ) ) );
            } catch ( const exception::Exception& err ) {
                wxLogMessage( wxT( "Shader %s error : %s" ), wxString( p_name ), wxString( err.what( ) ) );
                return false;
            }
            return true;
        } else {
            wxLogMessage( wxT( "Shader name %s is already exist." ), wxString( p_name ) );
            return false;
        }
    }

    Shader* ShaderManager::get( const std::string& p_name ) {
        auto it = m_shaders.find( p_name );
        if ( it == m_shaders.end( ) ) {
            wxLogMessage( wxT( "Cannot find shader : %s" ), wxString( p_name ) );
            return nullptr;
        }
        return it->second;
    }

    bool ShaderManager::empty( ) {
        if ( m_shaders.empty( ) ) {
            return true;
        } else {
            return false;
        }
    }

}; // namespace gw2b
