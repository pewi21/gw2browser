/* \file       Viewers/ModelViewer/FrameBuffer.h
*  \brief      Contains the declaration of the FrameBuffer class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2017 Khral Steelforge <https://github.com/kytulendu>

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

#ifndef VIEWERS_MODELVIEWER_FRAMEBUFFER_H_INCLUDED
#define VIEWERS_MODELVIEWER_FRAMEBUFFER_H_INCLUDED

namespace gw2b {

	class FrameBuffer {
		GLuint						m_fbo;				// Framebuffer object
		GLuint						m_fbTexture;		// Framebuffer texture
		GLuint						m_quadVAO;			// Quad vertex array object
		GLuint						m_quadVBO;			// Quad buffer array object
		GLuint						m_rbo;				// Renderbuffer object
		wxSize						m_clientSize;		// Size of OpenGL control

	public:
		/** Constructor. Create framebuffer object.
		*  \param[in]  p_size       Size of the OpenGL screen. */
		FrameBuffer( const wxSize& p_size );
		/** Destructor. Clears all data. */
		~FrameBuffer( );

		/** Draw framebuffer. */
		void draw( );
		/** Bind to framebuffer. */
		void bind( );
		/** Unbind to framebuffer. */
		void unbind( );
		/** Set framebuffer texture size. */
		void setClientSize( const wxSize& p_size );

	private:
		/** Setup screen quad. */
		void setupQuad( );
		/** Generates a texture that is suited for attachments to a framebuffer. */
		GLuint generateAttachmentTexture( const GLboolean p_depth, const GLboolean p_stencil );

	}; // class FrameBuffer

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_FRAMEBUFFER_H_INCLUDED
