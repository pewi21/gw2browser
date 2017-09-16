/* \file       Viewers/ModelViewer/Shader.h
*  \brief      Contains the declaration of the Shader class.
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

#pragma once

#ifndef VIEWERS_MODELVIEWER_SHADER_H_INCLUDED
#define VIEWERS_MODELVIEWER_SHADER_H_INCLUDED

namespace gw2b {

	class Shader {
		GLuint						m_program;

	public:
		/** Constructor. Create shader.
		*  \param[in]  p_vertexPath    Path to vertex shader file.
		*  \param[in]  p_fragmentPath  Path to fragment shader file.
		*  \param[in]  p_geometryPath  Path to geometry shader file. */
		Shader( const char* p_vertexPath, const char* p_fragmentPath, const char* p_geometryPath = nullptr );
		/** Destructor. Clears all data. */
		~Shader( );

		/** Uses the shader. */
		void use( );
		/** Get shader program ID represented by this data.
		*  \return GLuint			programId. */
		GLuint getProgramId( ) const;
		/** Delete the shader. */
		void clear( );

		/** Utility uniform functions. */
		void setBool( const std::string &p_name, bool p_value ) const;
		void setInt( const std::string &p_name, int p_value ) const;
		void setFloat( const std::string &p_name, float p_value ) const;
		void setVec2( const std::string &p_name, const glm::vec2 &p_value ) const;
		void setVec2( const std::string &p_name, float p_x, float p_y ) const;
		void setVec3( const std::string &p_name, const glm::vec3 &p_value ) const;
		void setVec3( const std::string &p_name, float p_x, float p_y, float p_z ) const;
		void setVec4( const std::string &p_name, const glm::vec4 &p_value ) const;
		void setVec4( const std::string &p_name, float p_x, float p_y, float p_z, float p_w ) const;
		void setMat2( const std::string &p_name, const glm::mat2 &p_mat ) const;
		void setMat3( const std::string &p_name, const glm::mat3 &p_mat ) const;
		void setMat4( const std::string &p_name, const glm::mat4 &p_mat ) const;

	private:
		/** Check for shader compilation error. */
		bool checkCompileErrors( GLuint shader, std::string type );

	}; // class Shader

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_SHADER_H_INCLUDED
