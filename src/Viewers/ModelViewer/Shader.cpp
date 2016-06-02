/* \file       Viewers/ModelViewer/Shader.cpp
*  \brief      Contains the declaration of the shader class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2016 Khral Steelforge <https://github.com/kytulendu>

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

#include <fstream>
#include <sstream>
#include <vector>

#include "Shader.h"

namespace gw2b {

	Shader::Shader( ) {
	}

	Shader::~Shader( ) {
	}

	void Shader::load( const char* p_vertexPath, const char* p_fragmentPath, const char* geometryPath ) {
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
			wxLogMessage( wxT( "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: %s" ), p_vertexPath );
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
			wxLogMessage( wxT( "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: %s" ), p_vertexPath );
		}

		// If geometry shader path is present, also load a geometry shader
		if ( geometryPath != nullptr ) {
			// Open file
			gShaderFile.open( geometryPath );
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
				wxLogMessage( wxT( "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: %s" ), geometryPath );
			}
		}

		// Compile Vertex Shader
		wxLogMessage( wxT( "INFO::SHADER::Compiling vertex shader : %s" ), p_vertexPath );
		GLuint vertex = glCreateShader( GL_VERTEX_SHADER );
		const GLchar *vShaderCode = vertexCode.c_str( );
		glShaderSource( vertex, 1, &vShaderCode, NULL );
		glCompileShader( vertex );
		checkCompileErrors( vertex, "VERTEX" );

		// Compile Fragment Shader
		wxLogMessage( wxT( "INFO::SHADER::Compiling vertex shader : %s" ), p_fragmentPath );
		GLuint fragment = glCreateShader( GL_FRAGMENT_SHADER );
		const GLchar *fShaderCode = fragmentCode.c_str( );
		glShaderSource( fragment, 1, &fShaderCode, NULL );
		glCompileShader( fragment );
		checkCompileErrors( vertex, "FRAGMENT" );

		// If geometry shader is given, compile geometry shader
		GLuint geometry;
		if ( geometryPath != nullptr ) {
			const GLchar * gShaderCode = geometryCode.c_str( );
			geometry = glCreateShader( GL_GEOMETRY_SHADER );
			glShaderSource( geometry, 1, &gShaderCode, NULL );
			glCompileShader( geometry );
			checkCompileErrors( geometry, "GEOMETRY" );
		}

		// Shader Program
		wxLogMessage( wxT( "INFO::SHADER::Linking shader program..." ) );
		this->program = glCreateProgram( );
		glAttachShader( this->program, vertex );
		glAttachShader( this->program, fragment );
		if ( geometryPath != nullptr ) {
			glAttachShader( this->program, geometry );
		}
		glLinkProgram( this->program );
		checkCompileErrors( this->program, "PROGRAM" );
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader( vertex );
		glDeleteShader( fragment );
		if ( geometryPath != nullptr ) {
			glDeleteShader( geometry );
		}

		wxLogMessage( wxT( "INFO::SHADER::Done." ) );
	}

	void Shader::use( ) {
		glUseProgram( this->program );
	}

	void Shader::clear( ) {
		glDeleteProgram( this->program );
	}

	void Shader::checkCompileErrors( GLuint shader, std::string type ) {
		GLint success;
		//GLchar infoLog[1024];
		GLint infoLogLength = 0;
		if ( type != "PROGRAM" ) {
			glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
			if ( !success ) {
				glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLogLength );
				std::vector<GLchar> infoLog( glm::max( infoLogLength, int( 1 ) ) );
				glGetShaderInfoLog( shader, infoLogLength, nullptr, &infoLog[0] );

				wxLogMessage( wxT( "ERROR::SHADER::SHADER-COMPILATION-ERROR::%s:\n%s" ), type, &infoLog[0] );
			}
		} else {
			glGetProgramiv( shader, GL_LINK_STATUS, &success );
			if ( !success ) {
				glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLogLength );
				std::vector<GLchar> infoLog( glm::max( infoLogLength, int( 1 ) ) );
				glGetShaderInfoLog( shader, infoLogLength, nullptr, &infoLog[0] );

				wxLogMessage( wxT( "ERROR::SHADER::PROGRAM-LINKING-ERROR::%s:\n%s" ), type, &infoLog[0] );
			}
		}

	}

}; // namespace gw2b
