/** \file       Viewers/ModelViewer/Shader.cpp
 *  \brief      Contains the declaration of the shader class.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2016-2017 Khralkatorrix <https://github.com/kytulendu>
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

#include "stdafx.h"

#include <fstream>
#include <sstream>
#include <vector>

#include "Exception.h"

#include "Shader.h"

namespace gw2b {

    Shader::Shader( const char* p_vertexPath, const char* p_fragmentPath, const char* p_geometryPath ) {
        // Read the vertex and fragment shader code from the file
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;

        // Open files
        vShaderFile.open( p_vertexPath );
        fShaderFile.open( p_fragmentPath );

        std::stringstream vShaderStream, fShaderStream;

        // Is the file opened
        if ( vShaderFile.is_open( ) ) {
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf( );
            // close file handlers
            vShaderFile.close( );
            // Convert stream into string
            vertexCode = vShaderStream.str( );
        } else {
            wxLogMessage( wxT( "Shader FILE_NOT_SUCCESFULLY_READ: %s" ), p_vertexPath );
            throw exception::Exception( "Unable to read vertex shader file." );
        }

        // Is the file opened
        if ( fShaderFile.is_open( ) ) {
            // Read file's buffer contents into streams
            fShaderStream << fShaderFile.rdbuf( );
            // close file handlers
            fShaderFile.close( );
            // Convert stream into string
            fragmentCode = fShaderStream.str( );
        } else {
            wxLogMessage( wxT( "Shader FILE_NOT_SUCCESFULLY_READ: %s" ), p_fragmentPath );
            throw exception::Exception( "Unable to read fragment shader file." );
        }

        // If geometry shader path is present, also load a geometry shader
        if ( p_geometryPath != nullptr ) {
            // Open file
            gShaderFile.open( p_geometryPath );
            // Is the file opened
            if ( gShaderFile.is_open( ) ) {
                std::stringstream gShaderStream;
                // Read file's buffer contents into streams
                gShaderStream << gShaderFile.rdbuf( );
                // close file handlers
                gShaderFile.close( );
                // Convert stream into string
                geometryCode = gShaderStream.str( );
            } else {
                wxLogMessage( wxT( "Shader FILE_NOT_SUCCESFULLY_READ: %s" ), p_geometryPath );
                throw exception::Exception( "Unable to read geometry shader file." );
            }
        }

        // Compile Vertex Shader
        wxLogMessage( wxT( "Compiling vertex shader : %s" ), p_vertexPath );
        GLuint vertex = glCreateShader( GL_VERTEX_SHADER );
        const GLchar *vShaderCode = vertexCode.c_str( );
        glShaderSource( vertex, 1, &vShaderCode, NULL );
        glCompileShader( vertex );
        if ( !checkCompileErrors( vertex, "VERTEX" ) ) {
            glDeleteShader( vertex );
            throw exception::Exception( "Vertex shader compilation error." );
        }

        // Compile Fragment Shader
        wxLogMessage( wxT( "Compiling fragment shader : %s" ), p_fragmentPath );
        GLuint fragment = glCreateShader( GL_FRAGMENT_SHADER );
        const GLchar *fShaderCode = fragmentCode.c_str( );
        glShaderSource( fragment, 1, &fShaderCode, NULL );
        glCompileShader( fragment );
        if ( !checkCompileErrors( fragment, "FRAGMENT" ) ) {
            glDeleteShader( vertex );
            glDeleteShader( fragment );
            throw exception::Exception( "Fragment shader compilation error." );
        }

        // If geometry shader is given, compile geometry shader
        GLuint geometry = 0;
        if ( p_geometryPath != nullptr ) {
            wxLogMessage( wxT( "Compiling geometry shader : %s" ), p_geometryPath );
            const GLchar * gShaderCode = geometryCode.c_str( );
            geometry = glCreateShader( GL_GEOMETRY_SHADER );
            glShaderSource( geometry, 1, &gShaderCode, NULL );
            glCompileShader( geometry );
            if ( !checkCompileErrors( geometry, "GEOMETRY" ) ) {
                glDeleteShader( vertex );
                glDeleteShader( fragment );
                glDeleteShader( geometry );
                throw exception::Exception( "Geometry shader compilation error." );
            }
        }

        // Shader Program
        wxLogMessage( wxT( "Linking shader program..." ) );
        m_program = glCreateProgram( );
        glAttachShader( m_program, vertex );
        glAttachShader( m_program, fragment );
        if ( p_geometryPath != nullptr ) {
            glAttachShader( m_program, geometry );
        }
        glLinkProgram( m_program );
        if ( checkCompileErrors( m_program, "PROGRAM" ) ) {
            wxLogMessage( wxT( "Done linking shader program." ) );
        }
        // Delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader( vertex );
        glDeleteShader( fragment );
        if ( p_geometryPath != nullptr ) {
            glDeleteShader( geometry );
        }

    }

    Shader::~Shader( ) {
        this->clear( );
    }

    void Shader::use( ) {
        glUseProgram( m_program );
    }

    GLuint Shader::getProgramId( ) const {
        return m_program;
    }

    void Shader::clear( ) {
        glDeleteProgram( m_program );
    }

    // ------------------------------------------------------------------------
    void Shader::setBool( const std::string& p_name, const bool p_value ) const {
        glUniform1i( glGetUniformLocation( m_program, p_name.c_str( ) ), static_cast<int> ( p_value ) );
    }
    // ------------------------------------------------------------------------
    void Shader::setInt( const std::string& p_name, const int p_value ) const {
        glUniform1i( glGetUniformLocation( m_program, p_name.c_str( ) ), p_value );
    }
    // ------------------------------------------------------------------------
    void Shader::setFloat( const std::string& p_name, const float p_value ) const {
        glUniform1f( glGetUniformLocation( m_program, p_name.c_str( ) ), p_value );
    }
    // ------------------------------------------------------------------------
    void Shader::setVec2( const std::string& p_name, const glm::vec2& p_value ) const {
        glUniform2fv( glGetUniformLocation( m_program, p_name.c_str( ) ), 1, glm::value_ptr( p_value ) );
    }
    void Shader::setVec2( const std::string& p_name, const float p_x, const float p_y ) const {
        glUniform2f( glGetUniformLocation( m_program, p_name.c_str( ) ), p_x, p_y );
    }
    // ------------------------------------------------------------------------
    void Shader::setVec3( const std::string& p_name, const glm::vec3& p_value ) const {
        glUniform3fv( glGetUniformLocation( m_program, p_name.c_str( ) ), 1, glm::value_ptr( p_value ) );
    }
    void Shader::setVec3( const std::string& p_name, const float p_x, const float p_y, const float p_z ) const {
        glUniform3f( glGetUniformLocation( m_program, p_name.c_str( ) ), p_x, p_y, p_z );
    }
    // ------------------------------------------------------------------------
    void Shader::setVec4( const std::string& p_name, const glm::vec4& p_value ) const {
        glUniform4fv( glGetUniformLocation( m_program, p_name.c_str( ) ), 1, glm::value_ptr( p_value ) );
    }
    void Shader::setVec4( const std::string& p_name, const float p_x, const float p_y, const float p_z, const float p_w ) const {
        glUniform4f( glGetUniformLocation( m_program, p_name.c_str( ) ), p_x, p_y, p_z, p_w );
    }
    // ------------------------------------------------------------------------
    void Shader::setMat2( const std::string& p_name, const glm::mat2& p_mat ) const {
        glUniformMatrix2fv( glGetUniformLocation( m_program, p_name.c_str( ) ), 1, GL_FALSE, glm::value_ptr( p_mat ) );
    }
    // ------------------------------------------------------------------------
    void Shader::setMat3( const std::string& p_name, const glm::mat3& p_mat ) const {
        glUniformMatrix3fv( glGetUniformLocation( m_program, p_name.c_str( ) ), 1, GL_FALSE, glm::value_ptr( p_mat ) );
    }
    // ------------------------------------------------------------------------
    void Shader::setMat4( const std::string& p_name, const glm::mat4& p_mat ) const {
        glUniformMatrix4fv( glGetUniformLocation( m_program, p_name.c_str( ) ), 1, GL_FALSE, glm::value_ptr( p_mat ) );
    }
    // ------------------------------------------------------------------------
    void Shader::setTexture( const std::string& p_name, const int p_tmu ) const {
        glUniform1i( glGetUniformLocation( m_program, p_name.c_str( ) ), p_tmu );
    }
    // ------------------------------------------------------------------------

    bool Shader::checkCompileErrors( GLuint shader, std::string type ) {
        GLint success;
        GLint infoLogLength = 0;
        if ( type != "PROGRAM" ) {
            glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
            if ( !success ) {
                glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLogLength );
                std::vector<GLchar> infoLog( glm::max( infoLogLength, int( 1 ) ) );
                glGetShaderInfoLog( shader, infoLogLength, nullptr, &infoLog[0] );

                wxLogMessage( wxT( "Shader compilation error::%s:\n%s" ), type, &infoLog[0] );
                return false;
            }
        } else {
            glGetProgramiv( shader, GL_LINK_STATUS, &success );
            if ( !success ) {
                glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLogLength );
                std::vector<GLchar> infoLog( glm::max( infoLogLength, int( 1 ) ) );
                glGetShaderInfoLog( shader, infoLogLength, nullptr, &infoLog[0] );

                wxLogMessage( wxT( "Shader program linking error::%s:\n%s" ), type, &infoLog[0] );
                return false;
            }
        }
        return true;
    }

}; // namespace gw2b
