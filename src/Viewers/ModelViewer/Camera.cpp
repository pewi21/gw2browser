/* \file       Viewers/ModelViewer/Camera.cpp
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

// todo : need correction in this!

#include "stdafx.h"
#include "Camera.h"

namespace gw2b {

	Camera::Camera( )
		: m_pivot( 0, 0, 0 )
		, m_distance( 200 )
		, m_yaw( 0 )
		, m_pitch( 0 ) {
	}

	Camera::~Camera( ) {
	}

	glm::mat4 Camera::calculateViewMatrix( ) const {
		// Calculate camera distance
		glm::vec4 distance = glm::vec4( 0.0f, 0.0f, -m_distance, 0.0f );

		auto rotViewMat = this->calculateRotationMatrix( );

		// Create the view matrix
		auto pivotVector = glm::vec3( m_pivot );

		// Uugh...
		auto eyePositionVector = glm::vec3( /*rotViewMat * */ distance ) + pivotVector;




		auto upVector = glm::vec3( 0, 1, 0 );

		glm::mat4 ViewMatrix = glm::lookAt(
			eyePositionVector,	// the position of your camera, in world space 
			pivotVector,		// where you want to look at, in world space
			upVector			// Head is up (set to 0,-1,0 to look upside-down)
			);

		return ViewMatrix;
	}

	glm::mat4 Camera::calculateRotationMatrix( ) const {
		glm::mat4 rotViewMat;
		rotViewMat = glm::rotate( rotViewMat, m_pitch, glm::vec3( 1.0f, 0.0f, 0.0f ) );
		rotViewMat = glm::rotate( rotViewMat, m_yaw, glm::vec3( 0.0f, 1.0f, 0.0f ) );
		return rotViewMat;
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
		m_pitch = this->clampPitch( m_pitch + p_pitch );
	}

	void Camera::setPitch( float p_pitch ) {
		m_pitch = this->clampPitch( p_pitch );
	}

	float Camera::clampPitch( float p_pitch ) {
		float rotationLimit = ( 89.0f * glm::pi<float>( ) ) / 180.0f;
		return wxMin( rotationLimit, wxMax( rotationLimit, p_pitch ) );
	}

	float Camera::distance( ) const {
		return m_distance;
	}

	void Camera::multiplyDistance( float p_multiplier ) {
		m_distance += ( m_distance * p_multiplier * 0.1f );
	}

	void Camera::setDistance( float p_distance ) {
		m_distance = p_distance;
	}

	const glm::vec3& Camera::pivot( ) const {
		return m_pivot;
	}

	void Camera::pan( float p_x, float p_y ) {

		auto rotationMatrix = this->calculateRotationMatrix( );

		// Pan speed is based on distance from pivot, so the user doesn't have to move the mouse like
		// a madman for big meshes
		float panSpeed = 0.001f * m_distance;

		// X axis
		glm::vec4 rightVector( 1, 0, 0, 0 );
		rightVector = rotationMatrix * rightVector; // transfrom matrices

		// Y axis
		glm::vec4 upVector( 0, 1, 0, 0 );
		upVector = rotationMatrix * upVector; // transfrom matrices

		// Perform the panning
		glm::vec3 pivotVector = m_pivot;

		rightVector = rightVector * ( p_x * panSpeed );
		upVector = upVector *( p_y * panSpeed );

		pivotVector = pivotVector + glm::vec3( rightVector );
		pivotVector = pivotVector + glm::vec3( upVector );

		m_pivot = pivotVector;
	}

	void Camera::setPivot( const glm::vec3& p_pivot ) {
		m_pivot = p_pivot;
	}

}; // namespace gw2b
