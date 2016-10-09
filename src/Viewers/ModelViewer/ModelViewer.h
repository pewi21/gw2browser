/* \file       Viewers/ModelViewer.h
*  \brief      Contains the declaration of the model viewer class.
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

#ifndef VIEWERS_MODELVIEWER_MODELVIEWER_H_INCLUDED
#define VIEWERS_MODELVIEWER_MODELVIEWER_H_INCLUDED

#include <map>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "LightBox.h"
#include "Shader.h"
#include "Text2D.h"

#include "DatFile.h"
#include "ViewerGLCanvas.h"
#include "Readers/ModelReader.h"

namespace gw2b {
	class DatFile;

	class ModelViewer;
	class RenderTimer : public wxTimer {
		ModelViewer* canvas;
	public:
		RenderTimer( ModelViewer* canvas );
		void Notify( );
		void start( );
	}; // class RenderTimer

	class ModelViewer : public ViewerGLCanvas {
		struct MeshCache;
		struct VAO;
		struct VBO;
		struct IBO;
		struct TBO;
		struct PackedVertex;

		DatFile&                    m_datFile;

		wxGLContext*				m_glContext;
		RenderTimer*				m_renderTimer;

		// Internal status

		// Is OpenGL is initialized?
		bool						m_glInitialized = false;
		// Toggle display text
		bool						m_statusText = true;
		// Toggle wireframe rendering
		bool						m_statusWireframe = false;
		// Cull triangles which normal is not towards the camera,
		// remove lighting glitch cause by some triangles
		bool						m_statusCullFace = false;
		// Toggle texture
		bool						m_statusTextured = true;
		// Toggle normal maping
		bool						m_statusNormalMapping = true;
		// Toggle lighting
		bool						m_statusLighting = true;
		// Toggle visualization of light source
		bool						m_statusRenderLightSource = false;
		// Toggle visualization of normal
		bool						m_statusVisualizeNormal = false;
		// Toggle visualization of z-buffer
		bool						m_statusVisualizeZbuffer = false;

		// Mesh
		GW2Model                    m_model;
		std::vector<MeshCache>		m_meshCache;
		std::vector<VAO>			m_vertexArray;		// Vertex Array Object
		std::vector<VBO>			m_vertexBuffer;		// Vertex Buffer Object
		std::vector<IBO>			m_indexBuffer;		// Index Buffer Object

		// Textures
		std::vector<TBO>			m_textureBuffer;	// Texture Buffer Object
		GLuint						m_dummyBlackTexture;
		GLuint						m_dummyWhiteTexture;

		// Light
		Light						m_light;
		LightBox					m_lightBox;			// For render cube at light position

		// Shader stuff
		Shader*						m_mainShader;
		Shader*						m_normalVisualizerShader;
		Shader*						m_zVisualizerShader;

		// Camera
		Camera                      m_camera;
		wxPoint                     m_lastMousePos;
		float                       m_minDistance;
		float                       m_maxDistance;

		// Text rendering stuff
		Text2D						m_text;

		//float angle = 0.0f;
	public:
		/** Constructor. Creates the model viewer with the given parent.
		*  \param[in]  p_parent     Parent of the control.
		*  \param[in]  p_pos        Optional position of the control.
		*  \param[in]  p_size       Optional size of the control. */
		ModelViewer( wxWindow* p_parent, const int *p_attrib, const wxPoint& p_pos, const wxSize& p_size, long p_style, DatFile& p_datFile );
		/** Destructor. */
		virtual ~ModelViewer( );

		virtual void clear( ) override;
		virtual void setReader( FileReader* p_reader ) override;
		/** Gets the image reader containing the data displayed by this viewer.
		*  \return ModelReader*    Reader containing the data. */
		ModelReader* modelReader( ) {
			return reinterpret_cast<ModelReader*>( this->reader( ) );
		} // already asserted with a dynamic_cast
		/** Gets the image reader containing the data displayed by this viewer.
		*  \return ModelReader*    Reader containing the data. */
		const ModelReader* modelReader( ) const {
			return reinterpret_cast<const ModelReader*>( this->reader( ) );
		} // already asserted with a dynamic_cast

	private:
		void clearBuffer( );
		void clearShader( );
		int initGL( );
		void onPaintEvt( wxPaintEvent& p_event );
		void render( );
		void drawModel( Shader* p_shader, const glm::mat4& p_trans );
		void drawMesh( Shader* p_shader, const glm::mat4& p_trans, const uint p_meshIndex );
		void displayStatusText( );
		void loadModel( GW2Model& p_model );
		void loadMesh( MeshCache& p_cache, const GW2Mesh& p_mesh );
		void computeTangent( MeshCache& p_mesh );
		bool getSimilarVertexIndex( PackedVertex& p_packed, std::map<PackedVertex, uint>& p_vertexToOutIndex, uint& p_result );
		void indexVBO( const MeshCache p_inMesh, MeshCache& p_outMesh );
		void populateBuffers( VAO& p_vao, VBO& p_vbo, IBO& p_ibo, const MeshCache& p_cache );
		void loadMaterial( GW2Model& p_model );
		GLuint createDummyTexture( const GLubyte* p_data );
		GLuint loadTexture( const uint p_fileId );
		void updateMatrices( );
		void focus( );
		void onMotionEvt( wxMouseEvent& p_event );
		void onMouseWheelEvt( wxMouseEvent& p_event );
		void onKeyDownEvt( wxKeyEvent& p_event );
		void onClose( wxCloseEvent& evt );

	}; // class ModelViewer

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_MODELVIEWER_H_INCLUDED
