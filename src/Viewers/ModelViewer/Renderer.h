/** \file       Viewers/Renderer.h
 *  \brief      Contains the declaration of the model renderer class.
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

#ifndef VIEWERS_MODELVIEWER_RENDERER_H_INCLUDED
#define VIEWERS_MODELVIEWER_RENDERER_H_INCLUDED

#include <chrono>
#include <vector>

#include "Viewers/ModelViewer/Camera.h"
#include "Viewers/ModelViewer/FrameBuffer.h"
#include "Viewers/ModelViewer/Light.h"
#include "Viewers/ModelViewer/LightBox.h"
#include "Viewers/ModelViewer/Model.h"
#include "Viewers/ModelViewer/ShaderManager.h"
#include "Viewers/ModelViewer/Text2D.h"
#include "Viewers/ModelViewer/TextureManager.h"

namespace gw2b {
    class DatFile;

    class Renderer {
        typedef std::chrono::high_resolution_clock Time;

        // Internal status
        bool                        m_IsModelLoaded = false;            // Flag to check if model is loaded
        bool                        m_statusText = true;                // Toggle display text
        bool                        m_statusWireframe = false;          // Toggle wireframe rendering
        bool                        m_statusCullFace = false;           // Cull triangles which normal is not towards the camera
        bool                        m_statusTextured = true;            // Toggle texture
        bool                        m_statusNormalMapping = true;       // Toggle normal maping
        bool                        m_statusLighting = true;            // Toggle lighting
        bool                        m_statusAntiAlising = true;         // Toggle anti alising
        bool                        m_statusRenderLightSource = false;  // Toggle visualization of light source
        bool                        m_statusVisualizeNormal = false;    // Toggle visualization of normal
        bool                        m_statusVisualizeZbuffer = false;   // Toggle visualization of z-buffer
        bool                        m_cameraMode = false;               // Toggle camera mode

        wxSize                      m_clientSize;

        std::unique_ptr<FrameBuffer> m_framebuffer;
        std::vector<std::unique_ptr<Model>> m_model;
        TextureManager              m_texture;
        Light                       m_light;
        std::unique_ptr<LightBox>   m_lightBox;         // For render cube at light position
        /* List of shaders :
         * - main
         * - framebuffer
         * - normal_visualizer
         * - z_visualizer
         */
        ShaderManager               m_shaders;
        Camera                      m_camera;
        std::unique_ptr<Text2D>     m_text;

        float                       m_deltaTime;
        Time::time_point            m_oldstartTime;

        // fps meter
        Time::time_point            m_fpsStartTime;
        double                      m_fpsDiffTime = 0.0;
        int                         m_frameCounter = 0;
        float                       m_fps = 0.0f;
        //float                     angle = 0.0f;

    public:
        /** Constructor. */
        Renderer();
        /** Destructor. */
        virtual ~Renderer();

        /** Clear the viewer. */
        void clear();
        /** Initialize the renderer. */
        static bool init();
        /** Setup the renderer. */
        void setup();
        /** Strings describing the current GL connection. */
        const GLubyte* getGLVersion();
        const GLubyte* getGLVendor();
        const GLubyte* getGLRenderer();
        /** For window size change */
        void setViewport(int p_x, int p_y, wxSize p_clientSize);
        /** Create a frame buffer */
        void createFrameBuffer();
        /** Render stuff */
        void render();
        void focus();
        void reloadShader();
        /** Load the model */
        void loadModel(DatFile& p_datFile, const GW2Model& p_model);
        /** Camera related stuff */
        bool getCameraMode();
        void setCameraMode(bool p_mode);
        void setCameraMouseSensitivity(float p_sensitivity);
        void processCameraKeyboard(Camera::CameraMovement p_direction);
        void processCameraMouseMovement(float p_xoffset, float p_yoffset, GLboolean p_constrainPitch = true);
        void processCameraMouseScroll(float p_yoffset);
        void panCamera(float p_x, float p_y);
        /** Toggle things */
        void toggleStatusText() { m_statusText = !m_statusText; }
        void toggleStatusWireframe() { m_statusWireframe = !m_statusWireframe; }
        void toggleStatusCullFace() { m_statusCullFace = !m_statusCullFace; }
        void toggleStatusTextured() { m_statusTextured = !m_statusTextured; }
        void toggleStatusLighting() { m_statusLighting = !m_statusLighting; }
        void toggleStatusNormalMapping() { m_statusNormalMapping = !m_statusNormalMapping; }
        void toggleStatusAntiAlising() { m_statusAntiAlising = !m_statusAntiAlising; }
        void toggleStatusVisualizeNormal() { m_statusVisualizeNormal = !m_statusVisualizeNormal; }
        void toggleStatusVisualizeZbuffer() {  m_statusVisualizeZbuffer = !m_statusVisualizeZbuffer; }
        void toggleStatusRenderLightSource() { m_statusRenderLightSource = !m_statusRenderLightSource; }

    private:
        void clearShader();
        bool loadShader();
        void initShaderValue();
        bool isLightmapExcluded(const uint32& p_id);
        void drawModel( Shader* p_shader, const glm::mat4& p_trans);
        void displayStatusText();
        void updateMatrices();


    }; // class Renderer

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_RENDERER_H_INCLUDED
