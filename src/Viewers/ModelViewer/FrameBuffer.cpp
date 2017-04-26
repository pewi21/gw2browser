/* \file       Viewers/ModelViewer/FrameBuffer.cpp
*  \brief      Contains the definition of the FrameBuffer class.
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

#include "stdafx.h"

#include "FrameBuffer.h"

namespace gw2b {

	FrameBuffer::FrameBuffer( const wxSize& p_size )
		: m_clientSize( p_size ) {

		this->setupQuad( );

		// Setup Framebuffers
		glGenFramebuffers( 1, &m_fbo );
		this->bind( );
		// Create a color attachment texture
		m_fbTexture = generateAttachmentTexture( false, false );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbTexture, 0 );
		// Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers( 1, &m_rbo );
		glBindRenderbuffer( GL_RENDERBUFFER, m_rbo );
		// Use a single renderbuffer object for both a depth AND stencil buffer
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_clientSize.x, m_clientSize.y );
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );
		// Now actually attach it
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo );

		// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
			wxLogMessage( wxT( "Framebuffer is not complete!" ) );
		}
		this->unbind( );
	}

	FrameBuffer::~FrameBuffer( ) {
		// Delete RBO
		if ( m_rbo ) {
			glDeleteRenderbuffers( 1, &m_rbo );
		}
		// Delete FBO
		if ( m_fbo ) {
			glDeleteFramebuffers( 1, &m_fbo );
		}
		// Delete screen texture
		if ( m_fbTexture ) {
			glDeleteTextures( 1, &m_fbTexture );
		}
		// Delete VBO
		if ( m_quadVBO ) {
			glDeleteBuffers( 1, &m_quadVBO );
		}
		// Delete VAO
		if ( m_quadVAO ) {
			glDeleteVertexArrays( 1, &m_quadVAO );
		}
	}

	void FrameBuffer::draw( ) {
		// Bind the quad VAO
		glBindVertexArray( m_quadVAO );
		// Use Texture Unit 0
		glActiveTexture( GL_TEXTURE0 );
		// Use the color attachment texture as the texture of the quad plane
		glBindTexture( GL_TEXTURE_2D, m_fbTexture );
		// Draw the quad
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		// Unbind texture
		glBindTexture( GL_TEXTURE_2D, 0 );
		// Unbind VAO
		glBindVertexArray( 0 );
	}

	void FrameBuffer::bind( ) {
		glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
	}

	void FrameBuffer::unbind( ) {
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void FrameBuffer::setClientSize( const wxSize& p_size ) {
		m_clientSize = p_size;
	}

	void FrameBuffer::setupQuad( ) {
		// Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		GLfloat quadVertices[] = {
			// Positions   // TexCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f,  -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f,  -1.0f,  1.0f, 0.0f,
			1.0f,   1.0f,  1.0f, 1.0f
		};

		// Setup screen VAO
		glGenVertexArrays( 1, &m_quadVAO );
		glGenBuffers( 1, &m_quadVBO );
		glBindVertexArray( m_quadVAO );
		glBindBuffer( GL_ARRAY_BUFFER, m_quadVBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), &quadVertices, GL_STATIC_DRAW );
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), ( GLvoid* ) 0 );
		glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), ( GLvoid* ) ( 2 * sizeof( GLfloat ) ) );
		glBindVertexArray( 0 );
	}

	GLuint FrameBuffer::generateAttachmentTexture( const GLboolean p_depth, const GLboolean p_stencil ) {
		// What enum to use?
		GLenum attachment_type;
		if ( !p_depth && !p_stencil ) {
			attachment_type = GL_RGB;
		} else if ( p_depth && !p_stencil ) {
			attachment_type = GL_DEPTH_COMPONENT;
		} else if ( !p_depth && p_stencil ) {
			attachment_type = GL_STENCIL_INDEX;
		}

		// Generate texture ID and load texture data
		GLuint textureID;
		glGenTextures( 1, &textureID );
		glBindTexture( GL_TEXTURE_2D, textureID );
		if ( !p_depth && !p_stencil ) {
			glTexImage2D( GL_TEXTURE_2D, 0, attachment_type, m_clientSize.x, m_clientSize.y, 0, attachment_type, GL_UNSIGNED_BYTE, NULL );
		} else {
			// Using both a stencil and depth test, needs special format arguments
			glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_clientSize.x, m_clientSize.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL );
		}
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glBindTexture( GL_TEXTURE_2D, 0 );

		return textureID;
	}

}; // namespace gw2b
