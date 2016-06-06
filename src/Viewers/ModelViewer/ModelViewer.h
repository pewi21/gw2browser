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
		struct VBO;
		struct IBO;
		struct TBO;
		struct PackedVertex;

		DatFile&                    m_datFile;

		wxGLContext*				m_glContext;
		RenderTimer*				m_renderTimer;
		Camera                      m_camera;
		wxPoint                     m_lastMousePos;
		float                       m_minDistance;
		float                       m_maxDistance;

		bool						m_glInitialized = false;
		bool						m_statusWireframe = false;
		bool						m_statusTextured = true;
		bool						m_statusText = true;
		bool						m_statusCullFace = true;

		// Mesh
		Model                       m_model;
		std::vector<MeshCache>		m_meshCache;
		std::vector<VBO>			m_vertexBuffer;		// Vertex Buffer Object
		std::vector<IBO>			m_indexBuffer;		// Index Buffer Object

		// Textures
		std::vector<TBO>			m_textureBuffer;	// Texture Buffer Object
		GLuint						m_dummyBlackTexture;
		GLuint						m_dummyWhiteTexture;

		GLuint						diffuseTextureID;
		GLuint						normalTextureID;
		//GLuint						specularTextureID;

		//GLuint						SpecularTexture;

		// Shader stuff
		Shader						m_mainShader;
		GLuint						lightID;
		glm::vec3					lightPos;
		GLuint						viewPosID;

		// Matices handles
		GLuint						modelMatrixID;
		GLuint						viewMatrixID;
		GLuint						projectionMatrixID;

		// Text rendering stuff
		Text2D						m_text;

		GLuint						modelVAO;

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
		int initGL( );
		void onPaintEvt( wxPaintEvent& p_event );
		void render( );
		void drawMesh( const uint p_meshIndex );
		void displayStatusText( const uint p_vertexCount, const uint p_triangleCount );
		void loadMeshes( MeshCache& p_cache, const Mesh& p_mesh, uint p_indexBase );
		void computeTangent(
			std::vector<glm::vec3>& in_vertices,
			std::vector<glm::vec2>& in_uvs,
			std::vector<glm::vec3>& in_normals,
			std::vector<glm::vec3>& out_tangents,
			std::vector<glm::vec3>& out_bitangents );
		bool getSimilarVertexIndex( PackedVertex& p_packed, std::map<PackedVertex, uint>& p_vertexToOutIndex, uint& p_result );
		void indexVBO(
			std::vector<glm::vec3>& in_vertices,
			std::vector<glm::vec2>& in_uvs,
			std::vector<glm::vec3>& in_normals,
			std::vector<glm::vec3>& in_tangents,
			std::vector<glm::vec3>& in_bitangents,
			std::vector<uint>& out_indices,
			std::vector<glm::vec3>& out_vertices,
			std::vector<glm::vec2>& out_uvs,
			std::vector<glm::vec3>& out_normals,
			std::vector<glm::vec3>& out_tangents,
			std::vector<glm::vec3>& out_bitangents );
		void populateBuffers( VBO& p_vbo, IBO& p_ibo, const MeshCache& p_cache );
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
