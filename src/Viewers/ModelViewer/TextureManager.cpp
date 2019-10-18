/** \file       Viewers/ModelViewer/TextureManager.cpp
 *  \brief      Contains the declaration of the texture manager class.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2017 Khralkatorrix <https://github.com/kytulendu>
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

#include <utility>

#include "Exception.h"

#include "TextureManager.h"

namespace gw2b {

    TextureManager::TextureManager( ) {

    }

    TextureManager::~TextureManager( ) {
        this->clear( );
    }

    void TextureManager::clear( ) {
        for ( auto it : m_texture ) {
            delete it.second;
        }
        m_texture.clear( );
    }

    bool TextureManager::load( DatFile& p_datFile, const uint32 p_id ) {
        auto it = m_texture.find( p_id );
        if ( it == m_texture.end( ) ) {
            try {
                m_texture.insert( std::pair<uint32, Texture2D*>( p_id, new Texture2D( p_datFile, p_id ) ) );
            } catch ( const exception::Exception& err ) {
                wxLogMessage( wxT( "Failed to load texture %d : %s" ), p_id, wxString( err.what( ) ) );
                return false;
            }
            return true;
        } else {
            wxLogMessage( wxT( "Texture %d is already loaded." ), p_id );
            return false;
        }
    }

    Texture2D* TextureManager::get( const uint32 p_id ) {
        auto it = m_texture.find( p_id );
        if ( it == m_texture.end( ) ) {
            //wxLogMessage( wxT( "Cannot find texture : %d" ), p_id );
            return nullptr;
        }
        return it->second;
    }

    bool TextureManager::remove( const uint32 p_id ) {
        auto it = m_texture.find( p_id );
        if ( it != m_texture.end( ) ) {
            delete it->second;
            m_texture.erase( it );
            return true;
        }
        return false;
    }

    bool TextureManager::empty( ) {
        if ( m_texture.empty( ) ) {
            return true;
        } else {
            return false;
        }
    }

}; // namespace gw2b
