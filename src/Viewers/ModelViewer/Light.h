/* \file       Viewers/ModelViewer/Light.h
*  \brief      Contains the declaration of the light class.
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

#ifndef VIEWERS_MODELVIEWER_LIGHT_H_INCLUDED
#define VIEWERS_MODELVIEWER_LIGHT_H_INCLUDED

namespace gw2b {

	class Light {
		glm::vec3					m_position;
		glm::vec3					m_ambient;
		glm::vec3					m_diffuse;
		glm::vec3					m_specular;

	public:
		Light( );
		~Light( );

		const glm::vec3& position( ) const;
		void setPosition( glm::vec3& p_pos );

		const glm::vec3& ambient( ) const;
		void setAmbient( glm::vec3& p_ambient );

		const glm::vec3& diffuse( ) const;
		void setDiffuse( glm::vec3& p_diffuse );

		const glm::vec3& specular( ) const;
		void setSpecular( glm::vec3& p_specular );

	private:
		

	}; // class Light

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_LIGHT_H_INCLUDED
