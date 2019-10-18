/** \file       Viewers/ModelViewer/Light.h
 *  \brief      Contains the declaration of the light class.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2016 Khralkatorrix <https://github.com/kytulendu>
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

#ifndef VIEWERS_MODELVIEWER_LIGHT_H_INCLUDED
#define VIEWERS_MODELVIEWER_LIGHT_H_INCLUDED

namespace gw2b {

    class Light {
        glm::vec3                   m_position;     // Light position
        glm::vec3                   m_ambient;      // Ambient light color
        glm::vec3                   m_diffuse;      // Diffuse light color
        glm::vec3                   m_specular;     // Specular light color

    public:
        /** Constructor. Create light object. */
        Light( );
        /** Destructor. Clears all data. */
        ~Light( );

        /** Get light position.
        *  \return glm::vec3&        Light position. */
        const glm::vec3& position( ) const;
        /** Set light position.
        *  \param[in]  p_pos         Light position. */
        void setPosition( const glm::vec3& p_pos );

        /** Get ambient light color.
        *  \return glm::vec3&        Ambient light color. */
        const glm::vec3& ambient( ) const;
        /** Set ambient light color.
        *  \param[in]  p_ambient     Ambient light color. */
        void setAmbient( const glm::vec3& p_ambient );

        /** Get diffuse light color.
        *  \return glm::vec3&        Diffuse light color. */
        const glm::vec3& diffuse( ) const;
        /** Set diffuse light color.
        *  \param[in]  p_diffuse     Diffuse light color. */
        void setDiffuse( const glm::vec3& p_diffuse );

        /** Get specular light color.
        *  \return glm::vec3&        Specular light color. */
        const glm::vec3& specular( ) const;
        /** Set specular light color.
        *  \param[in]  p_specular    Specular light color. */
        void setSpecular( const glm::vec3& p_specular );

    }; // class Light

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_LIGHT_H_INCLUDED
