/** \file       Viewers/ModelViewer/LightBox.h
 *  \brief      Contains the declaration of the LightBox class.
 *  \author     Khral Steelforge
 */

/**
 * Copyright (C) 2016 Khral Steelforge <https://github.com/kytulendu>
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

#ifndef VIEWERS_MODELVIEWER_LIGHTBOX_H_INCLUDED
#define VIEWERS_MODELVIEWER_LIGHTBOX_H_INCLUDED

#include "Shader.h"

namespace gw2b {

    class LightBox {
        GLuint                      m_cubeVAO;
        GLuint                      m_cubeVBO;
        Shader*                     m_cubeShader;
        GLuint                      m_uniformProjection;
        GLuint                      m_uniformView;
        GLuint                      m_uniformModel;
        GLuint                      m_uniformLightColor;
        glm::mat4                   m_projection;
        glm::mat4                   m_view;

    public:
        /** Constructor. Create lightbox renderer. */
        LightBox( );
        /** Destructor. Clears all data. */
        ~LightBox( );

        /** Set view matrix of lightbox renderer.
        *  \param[in]  p_view        View matrix. */
        void setViewMatrix( const glm::mat4& p_view );
        /** Set projection matrix of lightbox renderer.
        *  \param[in]  p_projection  Projection natrix. */
        void setProjectionMatrix( const glm::mat4& p_projection );

        /** Render colored box for light visualization at given position.
        *  \param[in]  p_pos         Light position.
        *  \param[in]  p_color       Box color. */
        void renderCube( const glm::vec3& p_pos, const glm::vec3& p_color );

    }; // class Light

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_LIGHTBOX_H_INCLUDED
