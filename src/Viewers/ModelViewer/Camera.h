/* \file       Viewers/ModelViewer/Camera.h
*  \brief      Contains the declaration of the camera class.
*  \author     Rhoot
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

#ifndef VIEWERS_MODELVIEWER_CAMERA_H_INCLUDED
#define VIEWERS_MODELVIEWER_CAMERA_H_INCLUDED

namespace gw2b {

	class Camera {
		// Camera Attributes
		glm::vec3 m_position;
		glm::vec3 m_pivot;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::vec3 m_worldup;
		float m_distance;
		// Eular Angles
		float m_yaw;
		float m_pitch;
		// Camera options
		float m_movementSpeed;
		float m_mouseSensitivity;
		int m_mode;

	public:
		enum CameraMovement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT
		};

		enum CameraMode {
			ORBITALCAM,
			FPSCAM
		};

	public:
		Camera( );
		~Camera( );

		glm::mat4 calculateViewMatrix( );

		// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void processKeyboard( CameraMovement p_direction, float p_deltaTime );
		// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void processMouseMovement( float p_xoffset, float p_yoffset, GLboolean p_constrainPitch = true );
		// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void processMouseScroll( float p_yoffset );

		void setCameraMode( CameraMode p_mode );

		float yaw( ) const;
		void addYaw( float p_yaw );
		void setYaw( float p_yaw );

		float pitch( ) const;
		void addPitch( float p_pitch );
		void setPitch( float p_pitch );
		float clampPitch( float p_pitch );

		float distance( ) const;
		void setDistance( float p_distance );

		const glm::vec3& pivot( ) const;
		void pan( float p_x, float p_y );
		void setPivot( const glm::vec3& p_pivot );

		const glm::vec3& position( ) const;
		void setPosition( const glm::vec3& p_position );

	private:
		// Calculates the front vector from the Camera's (updated) Eular Angles
		void updateCameraVectors( );

	}; // class Camera

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_CAMERA_H_INCLUDED
