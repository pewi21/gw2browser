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

#ifndef VIEWERS_MODELVIEWER_H_INCLUDED
#define VIEWERS_MODELVIEWER_H_INCLUDED

//#include "Camera.h"
#include "Readers/ModelReader.h"
#include "ViewerGLCanvas.h"

#include "INeedDatFile.h"

namespace gw2b {

	// OpenGL view data
	struct GLData {
		bool initialized;           // have OpenGL been initialized?
		//float beginx, beginy;       // position of mouse
		//float quat[4];              // orientation of object
		//float zoom;                 // field of view in degrees
	};

	/*
	struct MeshCache {
		IDirect3DIndexBuffer9*  indexBuffer;
		IDirect3DVertexBuffer9* vertexBuffer;
	};

	struct TextureCache {
		IDirect3DTexture9*  diffuseMap;
		//IDirect3DTexture9*  normalMap;
	};

	struct AutoReleaser {
		template <typename T>
		void operator()( T* p_pointer ) const {
			p_pointer->Release( );
		}
	};
	*/

	class ModelViewer;

	class RenderTimer : public wxTimer {
		ModelViewer* canvas;
	public:
		RenderTimer( ModelViewer* canvas );
		void Notify( );
		void start( );
	};

	//============================================================================/

	class INeedDatFile;

	class ModelViewer : public ViewerGLCanvas, public INeedDatFile {
		Model                       m_model;
		//Array<MeshCache>            m_meshCache;
		//Array<TextureCache>         m_textureCache;
		//Camera                      m_camera;
		wxPoint                     m_lastMousePos;
		//float                       m_minDistance;
		//float                       m_maxDistance;
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

		//void focus( );
		//void drawMesh( uint p_meshIndex );
		//void drawText( uint p_x, uint p_y, const wxString& p_text );

	private:
		void paintNow( wxPaintEvent& p_event );
		void onPaintEvt( wxPaintEvent& p_event );
		void onClose( wxCloseEvent& evt );
		//void onMotionEvt( wxMouseEvent& p_event );
		//void onMouseWheelEvt( wxMouseEvent& p_event );
		//void onKeyDownEvt( wxKeyEvent& p_event );
		void initGL( );
		void render( );
		GLuint loadShaders( const char *vertex_file_path, const char *fragment_file_path );
		//bool createBuffers( MeshCache& p_cache, uint p_vertexCount, uint p_vertexSize, uint p_indexCount, uint p_indexSize );
		//bool populateBuffers( const Mesh& p_mesh, MeshCache& p_cache );
		void updateMatrices( );
		//IDirect3DTexture9* loadTexture( uint p_fileId );

		GLData						m_gldata;
		wxGLContext					*m_glContext;

		GLuint						VertexArrayID;

		GLuint						vertexbuffer;
		GLuint						colorbuffer;

		// shader ID
		GLuint						programID;

		GLuint MatrixID;
		glm::mat4 MVP;

		RenderTimer* timer;

		bool statusWireframe = false;

	}; // class ImageViewer

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_H_INCLUDED
