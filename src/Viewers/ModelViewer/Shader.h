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
	public:
		GLuint program;

		// Constructor for Shader object, need to clean the shader manually
		Shader( const char* p_vertexPath, const char* p_fragmentPath, const char* p_geometryPath = nullptr );
		// Destructor
		~Shader( );

		// Uses the current shader
		void use( );
		// Delete the current shader
		void clear( );

	private:
		bool checkCompileErrors( GLuint shader, std::string type );

	}; // class Shader

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_SHADER_H_INCLUDED
