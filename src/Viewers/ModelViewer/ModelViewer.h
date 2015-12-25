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

#include "Camera.h"

#include "Readers/ModelReader.h"
#include "ViewerGLCanvas.h"

#include "INeedDatFile.h"

namespace gw2b {

	struct MeshCache {
		std::vector<glm::vec3>	vertices;
		std::vector<glm::vec2>	uvs;
		std::vector<glm::vec3>	normals;

	};

	struct VBO {
		GLuint					vertexBuffer;
		GLuint					uvBuffer;
		GLuint					normalBuffer;
	};

	struct TBO {
		GLuint					diffuseMap;
		//GLuint					normalMap;
		//GLuint					lightMap;
	};

	class ModelViewer;
	class RenderTimer : public wxTimer {
		ModelViewer* canvas;
	public:
		RenderTimer( ModelViewer* canvas );
		void Notify( );
		void start( );
	}; // class RenderTimer

	class ModelViewer : public ViewerGLCanvas, public INeedDatFile {
		Model                       m_model;
		std::vector<MeshCache>		m_meshCache;
		std::vector<VBO>			m_meshBuffer;		// Vertex Buffer Object
		std::vector<TBO>			m_textureBuffer;	// Texture Buffer Object
		Camera                      m_camera;
		wxPoint                     m_lastMousePos;
		float                       m_minDistance;
		float                       m_maxDistance;
		wxGLContext*				m_glContext;
		RenderTimer*				m_renderTimer;
	public:
		ModelViewer( wxWindow* p_parent, const wxPoint& p_pos = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
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
		int initGL( );
		void paintNow( wxPaintEvent& p_event );
		void onPaintEvt( wxPaintEvent& p_event );
		void render( );
		void drawMesh( const uint p_meshIndex );
		//void drawText( uint p_x, uint p_y, const wxString& p_text 
		bool loadMeshes( MeshCache& p_cache, const Mesh& p_mesh, uint p_indexBase );
		bool populateBuffers( VBO& p_buffer, const MeshCache& p_cache );
		GLuint createDummyTexture( const GLubyte *p_data );
		GLuint loadTexture( const uint p_fileId );
		GLuint loadShaders( const char *vertex_file_path, const char *fragment_file_path );
		void focus( );
		void onMotionEvt( wxMouseEvent& p_event );
		void onMouseWheelEvt( wxMouseEvent& p_event );
		void onKeyDownEvt( wxKeyEvent& p_event );
		void onClose( wxCloseEvent& evt );

	private:
		bool						m_glInitialized = false;
		bool						m_statusWireframe = false;
		bool						m_statusTextured = true;
		GLuint						dummyBlackTexture;
		GLuint						dummyWhiteTexture;
		glm::mat4					MVP;
		GLuint						MatrixID;
		GLuint						programID;
		GLuint						VertexArrayID;
		GLuint						TextureArrayID;

	}; // class ModelViewer

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_MODELVIEWER_H_INCLUDED
