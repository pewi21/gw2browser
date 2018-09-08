/* \file       Viewers/ModelViewer/Light.cpp
*  \brief      Contains the declaration of the Light class.
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
#include "Light.h"

namespace gw2b {

    Light::Light( )
        : m_position( 0.0f, 0.0f, 0.0f )
        , m_ambient( 0.5f, 0.5f, 0.5f )
        , m_diffuse( 0.5f, 0.5f, 0.5f )
        , m_specular( 0.5f, 0.5f, 0.5f ) {
    }

    Light::~Light( ) {

    }

    const glm::vec3& Light::position( ) const {
        return m_position;
    }

    void Light::setPosition( const glm::vec3& p_pos ) {
        m_position = p_pos;
    }

    const glm::vec3& Light::ambient( ) const {
        return m_ambient;
    }

    void Light::setAmbient( const glm::vec3& p_ambient ) {
        m_ambient = p_ambient;
    }

    const glm::vec3& Light::diffuse( ) const {
        return m_diffuse;
    }

    void Light::setDiffuse( const glm::vec3& p_diffuse ) {
        m_diffuse = p_diffuse;
    }

    const glm::vec3& Light::specular( ) const {
        return m_specular;
    }

    void Light::setSpecular( const glm::vec3& p_specular ) {
        m_specular = p_specular;
    }

}; // namespace gw2b
