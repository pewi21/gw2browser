/* \file       Viewers/ModelViewer/Camera.cpp
*  \brief      Contains the declaration of the camera class.
*  \author     Rhoot
*/

/*
Copyright (C) 2015-2016 Khral Steelforge <https://github.com/kytulendu>
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

#include "stdafx.h"
#include "Camera.h"

namespace gw2b {

	Camera::Camera( )
		: m_position( 0.0f, 0.0f, 0.0f )
		, m_pivot( 0.0f, 0.0f, -1.0f )
		, m_worldup( 0.0f, 1.0f, 0.0f )
		, m_distance( 200.0f )
		, m_yaw( 0.0f )
		, m_pitch( 0.0f )
		, m_movementSpeed( 80.0f )
		, m_mouseSensitivity( 0.25f )
		, m_mode( ORBITALCAM )
	{
		this->updateCameraVectors( );
	}

	Camera::~Camera( ) {

	}

	glm::mat4 Camera::calculateViewMatrix( ) {
		this->updateCameraVectors( );

		glm::vec3 center;
		if ( m_mode == ORBITALCAM ) {
			center = m_pivot;
		} else if ( m_mode == FPSCAM ) {
			center = m_position + m_pivot;
		}

		return glm::lookAt( m_position, center, m_up );
	}

	void Camera::processKeyboard( CameraMovement p_direction, float p_deltaTime ) {
		float velocity = m_movementSpeed * p_deltaTime;
		if ( p_direction == FORWARD ) {
			m_position += m_pivot * velocity;
		}
		if ( p_direction == BACKWARD ) {
			m_position -= m_pivot * velocity;
		}
		if ( p_direction == LEFT ) {
			m_position -= m_right * velocity;
		}
		if ( p_direction == RIGHT ) {
			m_position += m_right * velocity;
		}
	}

	void Camera::processMouseMovement( float p_xoffset, float p_yoffset, GLboolean p_constrainPitch ) {
		this->addYaw( m_mouseSensitivity * p_xoffset );
		this->addPitch( m_mouseSensitivity * p_yoffset );

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if ( p_constrainPitch ) {
			if ( m_pitch > 89.0f ) {
				m_pitch = 89.0f;
			}
			if ( m_pitch < -89.0f ) {
				m_pitch = -89.0f;
			}
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors( );
	}

	void Camera::processMouseScroll( float p_yoffset ) {
		m_distance += ( m_distance * p_yoffset * 0.1f );

		if ( m_distance <= 1.0f ) {
			m_distance = 1.0f;
		}
	}

	void Camera::setCameraMode( CameraMode p_mode ) {
		m_mode = p_mode;
	}

	float Camera::yaw( ) const {
		return m_yaw;
	}

	void Camera::addYaw( float p_yaw ) {
		m_yaw += p_yaw;
	}

	void Camera::setYaw( float p_yaw ) {
		m_yaw = p_yaw;
	}

	float Camera::pitch( ) const {
		return m_pitch;
	}

	void Camera::addPitch( float p_pitch ) {
		m_pitch += p_pitch;
	}

	void Camera::setPitch( float p_pitch ) {
		m_pitch = p_pitch;
	}

	float Camera::distance( ) const {
		return m_distance;
	}

	void Camera::setDistance( float p_distance ) {
		m_distance = p_distance;
	}

	const glm::vec3& Camera::pivot( ) const {
		return m_pivot;
	}

	void Camera::pan( float p_x, float p_y ) {
		// Pan speed is based on distance from pivot, so the user doesn't have to move the mouse like
		// a madman for big meshes
		float panSpeed = 0.001f * m_distance;

		// Perform the panning
		m_right = m_right * ( p_x * panSpeed );
		m_up = m_up * ( p_y * panSpeed );

		m_pivot = m_pivot + glm::vec3( m_right );
		m_pivot = m_pivot + glm::vec3( m_up );

		this->updateCameraVectors( );
	}

	void Camera::setPivot( const glm::vec3& p_pivot ) {
		m_pivot = p_pivot;
	}

	const glm::vec3& Camera::position( ) const {
		return m_position;
	}

	void Camera::setPosition( const glm::vec3& p_position ) {
		m_position = p_position;
	}

	void Camera::setMouseSensitivity( float p_sensitivity ) {
		m_mouseSensitivity = p_sensitivity;
	}

	void Camera::updateCameraVectors( ) {
		if ( m_mode == ORBITALCAM ) {
			glm::mat4 pitchMatrix;
			glm::mat4 yawMatrix;
			pitchMatrix = glm::rotate( pitchMatrix, m_pitch, glm::vec3( -1.0f, 0.0f, 0.0f ) );
			yawMatrix = glm::rotate( yawMatrix, m_yaw, glm::vec3( 0.0f, 1.0f, 0.0f ) );
			auto rotationMatrix = pitchMatrix * yawMatrix;

			m_position = glm::vec3( rotationMatrix * glm::vec4( 0.0f, 0.0f, m_distance, 0.0f ) ) + m_pivot;

			// Re-calculate right and up vector
			m_right = rotationMatrix * glm::vec4( -1.0f, 0.0f, 0.0f, 0.0f );
			m_up = rotationMatrix * glm::vec4( m_worldup, 0.0f );
		} else if ( m_mode == FPSCAM ) {
			// Calculate the new Front vector
			glm::vec3 front;
			front.x = cos( glm::radians( m_yaw ) ) * cos( glm::radians( m_pitch ) );
			front.y = sin( glm::radians( m_pitch ) );
			front.z = sin( glm::radians( m_yaw ) ) * cos( glm::radians( m_pitch ) );
			m_pivot = glm::normalize( front );
			// Also re-calculate the Right and Up vector
			m_right = glm::normalize( glm::cross( m_pivot, m_worldup ) );  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			m_up = glm::normalize( glm::cross( m_right, m_pivot ) );
		}
	}

}; // namespace gw2b
