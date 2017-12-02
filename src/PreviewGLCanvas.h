/* \file       PreviewGLCanvas.cpp
*  \brief      Contains declaration of the preview GLCanvas control.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2015 Khral Steelforge <https://github.com/kytulendu>
Copyright (C) 2012 Rhoot <https://github.com/rhoot>

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

#pragma once

#ifndef PREVIEWGLCANVAS_H_INCLUDED
#define PREVIEWGLCANVAS_H_INCLUDED

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

#include "Readers/MapReader.h"
#include "Readers/ModelReader.h"

#include "FileReader.h"

namespace gw2b {
	class DatFile;
	class DatIndexEntry;

	class PreviewGLCanvas;
	class RenderTimer : public wxTimer {
		PreviewGLCanvas* canvas;
	public:
		RenderTimer( PreviewGLCanvas* canvas );
		void Notify( );
		void start( );
	}; // class RenderTimer

	/** Panel control used to preview files from the .dat. */
	class PreviewGLCanvas : public wxGLCanvas {
		typedef std::chrono::high_resolution_clock Time;

		FileReader*                 m_reader;

		wxGLContext*				m_glContext;
		RenderTimer*				m_renderTimer;
		wxTimer*					m_movementKeyTimer;

		wxSize						m_clientSize;

		// Internal status
		bool						m_isViewingMap = false;				// Is we are viewing map?
		bool						m_glInitialized = false;			// Is OpenGL is initialized?
		bool						m_statusText = true;				// Toggle display text
		bool						m_statusWireframe = false;			// Toggle wireframe rendering
		bool						m_statusCullFace = false;			// Cull triangles which normal is not towards the camera
		bool						m_statusTextured = true;			// Toggle texture
		bool						m_statusNormalMapping = true;		// Toggle normal maping
		bool						m_statusLighting = true;			// Toggle lighting
		bool						m_statusAntiAlising = true;			// Toggle anti alising
		bool						m_statusRenderLightSource = false;	// Toggle visualization of light source
		bool						m_statusVisualizeNormal = false;	// Toggle visualization of normal
		bool						m_statusVisualizeZbuffer = false;	// Toggle visualization of z-buffer
		bool						m_cameraMode = false;				// Toggle camera mode

		// Framebuffer
		std::unique_ptr<FrameBuffer> m_framebuffer;

		// Model
		std::vector<std::unique_ptr<Model>>	m_model;

		// Textures
		TextureManager				m_texture;

		// Light
		Light						m_light;
		std::unique_ptr<LightBox>	m_lightBox;			// For render cube at light position

		// Shader stuff
		// List of shaders :
		// - main
		// - framebuffer
		// - normal_visualizer
		// - z_visualizer
		ShaderManager				m_shaders;

		// Camera
		Camera                      m_camera;
		wxPoint                     m_lastMousePos;
		float                       m_minDistance;
		float                       m_maxDistance;

		// Text rendering stuff
		std::unique_ptr<Text2D>		m_text;

		//float angle = 0.0f;

		// Movement key related
		float						m_deltaTime;
		Time::time_point			m_oldstartTime;

		// fps meter
		//Time::time_point			m_fpsStartTime;
		//double					m_fpsDiffTime = 0.0;
		//int						m_frameCounter = 0;
		//float						m_fps = 0.0f;

	public:
		/** Constructor. Creates the preview GLCanvas with the given parent.
		*  \param[in]  p_parent     Parent of the control.
		*  \param[in]  p_attrib     OpenGL attribute list. */
		PreviewGLCanvas( wxWindow* p_parent, const int* p_attrib );
		/** Destructor. */
		~PreviewGLCanvas( );
		/** Tells this GLCanvas to preview a file.
		*  \param[in]  p_datFile    .dat file containing the file to preview.
		*  \param[in]  p_entry      Entry to preview.
		*  \return bool    true if successful, false if not. */
		bool previewFile( DatFile& p_datFile, const DatIndexEntry& p_entry );
		/** Clear the viewer. */
		void clear( );

	private:
		/** Helper method to create a viewer control to handle the given data type.
		*  The caller is responsible for freeing the viewer.
		*  \param[in]  p_dataType   Type of data to create a viewer for.
		*  \param[in]  p_datFile    Reference to an instance of DatFile.
		*  \return Viewer* Newly created viewer. */
		//ViewerGLCanvas* createViewerForDataType( FileReader::DataType p_dataType, DatFile& p_datFile );

		/** Gets the reader containing the data displayed by this viewer.
		*  \return FileReader*     Reader containing the data. */
		FileReader* reader( ) {
			return m_reader;
		}
		/** Gets the reader containing the data displayed by this viewer.
		*  \return FileReader*     Reader containing the data. */
		const FileReader* reader( ) const {
			return m_reader;
		}

		/** Gets the model reader containing the data displayed by this viewer.
		*  \return ModelReader*    Reader containing the data. */
		ModelReader* modelReader( ) {
			return reinterpret_cast<ModelReader*>( this->reader( ) );
		} // already asserted with a dynamic_cast
		  /** Gets the model reader containing the data displayed by this viewer.
		  *  \return ModelReader*    Reader containing the data. */
		const ModelReader* modelReader( ) const {
			return reinterpret_cast<const ModelReader*>( this->reader( ) );
		} // already asserted with a dynamic_cast
		  /** Gets the map reader containing the data displayed by this viewer.
		  *  \return MapReader*    Reader containing the data. */
		MapReader* mapReader( ) {
			return reinterpret_cast<MapReader*>( this->reader( ) );
		} // already asserted with a dynamic_cast
		  /** Gets the map reader containing the data displayed by this viewer.
		  *  \return MapReader*    Reader containing the data. */
		const MapReader* mapReader( ) const {
			return reinterpret_cast<const MapReader*>( this->reader( ) );
		} // already asserted with a dynamic_cast

		void clearShader( );
		bool isLightmapExcluded( const uint32& p_id );
		void loadModel( DatFile& p_datFile, const GW2Model& p_model );
		bool loadShader( );
		void initShaderValue( );
		void reloadShader( );
		bool initGL( );
		void onPaintEvt( wxPaintEvent& p_event );
		void render( );
		void drawModel( Shader* p_shader, const glm::mat4& p_trans );
		void displayStatusText( );
		void updateMatrices( );
		void focus( );
		void createFrameBuffer( );
		void onMotionEvt( wxMouseEvent& p_event );
		void onMouseWheelEvt( wxMouseEvent& p_event );
		void onKeyDownEvt( wxKeyEvent& p_event );
		void onMovementKeyTimerEvt( wxTimerEvent& p_event );
		void onClose( wxCloseEvent& evt );
		void onResize( wxSizeEvent& evt );

	}; // class PreviewGLCanvas

}; // namespace gw2b

#endif // PREVIEWGLCANVAS_H_INCLUDED
