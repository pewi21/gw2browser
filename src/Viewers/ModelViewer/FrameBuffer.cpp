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

    FrameBuffer::FrameBuffer( const wxSize& p_size, GLuint p_attachCount )
        : m_clientSize( p_size )
        , m_isMultisample( false ) {

        this->setupQuad( );

        // Create normal framebuffer
        this->setupFramebuffer( p_attachCount );
        // Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if ( !this->checkFrameBufferStatus( ) ) {
            wxLogMessage( wxT( "Framebuffer is not complete!" ) );
        }

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    }

    FrameBuffer::FrameBuffer( const wxSize& p_size, GLuint p_samples, GLuint p_attachCount )
        : m_clientSize( p_size )
        , m_isMultisample( true ) {

        this->setupQuad( );

        // Create multi-sample framebuffer
        this->setupMultiSampleFramebuffer( p_samples, p_attachCount );

        // Create normal framebuffer
        this->setupFramebuffer( p_attachCount );
        // Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if ( !this->checkFrameBufferStatus( ) ) {
            wxLogMessage( wxT( "Framebuffer is not complete!" ) );
        }

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
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

        if ( m_isMultisample ) {
            // Delete multi-sample RBO
            if ( m_rboMultiSample ) {
                glDeleteRenderbuffers( 1, &m_rboMultiSample );
            }
            // Delete multi-sample texture
            for ( auto& it : m_textureColorBufferMultiSampled ) {
                glDeleteTextures( 1, &it );
            }
            // Delete multi-sample FBO
            if ( m_fboMultiSample ) {
                glDeleteFramebuffers( 1, &m_fboMultiSample );
            }
        }

        // Delete screen texture
        for ( auto& it : m_fbTexture ) {
            glDeleteTextures( 1, &it );
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

        for ( GLuint i = 0; i < m_fbTexture.size( ); i++ ) {
            // Use Texture Unit i
            glActiveTexture( GL_TEXTURE0 + i );
            // Use the color attachment texture as the texture of the quad plane
            glBindTexture( GL_TEXTURE_2D, m_fbTexture[i] );
        }

        // Draw the quad
        glDrawArrays( GL_TRIANGLES, 0, 6 );

        // Unbind texture
        for ( GLuint i = 0; i < m_fbTexture.size( ); i++ ) {
            glActiveTexture( GL_TEXTURE0 + i );
            glBindTexture( GL_TEXTURE_2D, 0 );
        }

        // Unbind VAO
        glBindVertexArray( 0 );
    }

    void FrameBuffer::bind( ) {
        if ( m_isMultisample ) {
            glBindFramebuffer( GL_FRAMEBUFFER, m_fboMultiSample );
        } else {
            glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
        }
    }

    void FrameBuffer::unbind( ) {
        if ( m_isMultisample ) {
            // Blit rendered image from MSAA (multi-sample) framebuffer to normal (single-sample) framebuffer
            glBindFramebuffer( GL_READ_FRAMEBUFFER, m_fboMultiSample );
            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo );
            glBlitFramebuffer( 0, 0, m_clientSize.x, m_clientSize.y,    // src rect
                0, 0, m_clientSize.x, m_clientSize.y,                   // dst rect
                GL_COLOR_BUFFER_BIT,                                    // buffer mask
                GL_LINEAR );                                            // scale filter
        }

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

    void FrameBuffer::setupFramebuffer( GLuint p_numTex ) {
        // Create a FBO to hold a render-to-texture
        glGenFramebuffers( 1, &m_fbo );
        glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );

        for ( GLuint i = 0; i < p_numTex; i++ ) {
            // Create a color attachment texture
            m_fbTexture.push_back( generateAttachmentTexture( ) );
            // Attach a texture to FBO color attachement point
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_fbTexture[i], 0 );
        }

        // Create a renderbuffer object
        glGenRenderbuffers( 1, &m_rbo );
        glBindRenderbuffer( GL_RENDERBUFFER, m_rbo );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_clientSize.x, m_clientSize.y );
        glBindRenderbuffer( GL_RENDERBUFFER, 0 );
        // Attach a RBO to FBO attachement point
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo );

        std::vector<GLuint> attachments;
        // Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        for ( GLuint i = 0; i < m_fbTexture.size( ); i++ ) {
            attachments.push_back( GL_COLOR_ATTACHMENT0 + i );
        }
        glDrawBuffers( attachments.size( ), &attachments[0] );
    }

    void FrameBuffer::setupMultiSampleFramebuffer( GLuint p_samples, GLuint p_numTex ) {
        // Create a multi-sample framebuffer object
        glGenFramebuffers( 1, &m_fboMultiSample );
        glBindFramebuffer( GL_FRAMEBUFFER, m_fboMultiSample );

        for ( GLuint i = 0; i < p_numTex; i++ ) {
            // Create a multi-sampled color attachment texture
            m_textureColorBufferMultiSampled.push_back( generateMultiSampleTexture( p_samples ) );
            // Attach a texture to FBO color attachement point
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, m_textureColorBufferMultiSampled[i], 0 );
        }

        // Create a renderbuffer object for depth and stencil attachments
        glGenRenderbuffers( 1, &m_rboMultiSample );
        glBindRenderbuffer( GL_RENDERBUFFER, m_rboMultiSample );
        glRenderbufferStorageMultisample( GL_RENDERBUFFER, p_samples, GL_DEPTH24_STENCIL8, m_clientSize.x, m_clientSize.y );
        glBindRenderbuffer( GL_RENDERBUFFER, 0 );
        // Attach msaa RBO to FBO attachment points
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboMultiSample );

        std::vector<GLuint> attachments;
        // Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        for ( GLuint i = 0; i < m_textureColorBufferMultiSampled.size( ); i++ ) {
            attachments.push_back( GL_COLOR_ATTACHMENT0 + i );
        }
        glDrawBuffers( attachments.size( ), &attachments[0] );
    }

    GLuint FrameBuffer::generateAttachmentTexture( ) {
        // Generate texture
        GLuint textureID;
        glGenTextures( 1, &textureID );

        glBindTexture( GL_TEXTURE_2D, textureID );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, m_clientSize.x, m_clientSize.y, 0, GL_RGBA, GL_FLOAT, NULL );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glBindTexture( GL_TEXTURE_2D, 0 );

        return textureID;
    }

    GLuint FrameBuffer::generateMultiSampleTexture( GLuint p_samples ) {
        GLuint texture;
        glGenTextures( 1, &texture );

        glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, texture );
        glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, p_samples, GL_RGBA16F, m_clientSize.x, m_clientSize.y, GL_TRUE );
        glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, 0 );

        return texture;
    }

    bool FrameBuffer::checkFrameBufferStatus( ) {
        GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
        switch ( status ) {
        case GL_FRAMEBUFFER_COMPLETE:
            wxLogMessage( wxT( "Framebuffer complete." ) );
            return true;

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            wxLogMessage( wxT( "Framebuffer : Attachment is not complete." ) );
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            wxLogMessage( wxT( "Framebuffer : No image is attached to FBO." ) );
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            wxLogMessage( wxT( "Framebuffer : Incomplete draw buffer." ) );
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            wxLogMessage( wxT( "Framebuffer : Incomplete read buffer." ) );
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            wxLogMessage( wxT( "Framebuffer : Incomplete multisample." ) );
            return false;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            wxLogMessage( wxT( "Framebuffer : Unsupported by FBO implementation." ) );
            return false;

        default:
            wxLogMessage( wxT( "Framebuffer : Unknown error." ) );
            return false;
        }
    }

}; // namespace gw2b
