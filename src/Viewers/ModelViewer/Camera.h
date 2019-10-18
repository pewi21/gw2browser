/** \file       Viewers/ModelViewer/Camera.h
 *  \brief      Contains the declaration of the camera class.
 *  \author     Rhoot
 */

/**
 * Copyright (C) 2015-2016 Khralkatorrix <https://github.com/kytulendu>
 * Copyright (C) 2012 Rhoot <https://github.com/rhoot>
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
        /** Constructor. Create camera. */
        Camera( );
        /** Destructor. Clears all data. */
        ~Camera( );

        /** Calculate view matrix.
        *  \return glm::mat4&        View matrix. */
        glm::mat4 calculateViewMatrix( );

        /** Processes input received from keyboard.
        *  \param[in]  p_direction   Direction to move the camera.
        *  \param[in]  p_deltaTime   Time between frame. */
        void processKeyboard( CameraMovement p_direction, float p_deltaTime );
        /** Processes input received from a mouse.
        *  \param[in]  p_xoffset     X offset.
        *  \param[in]  p_yoffset     Y offset.
        *  \param[in]  p_constrainPitch  Limit pitch, so screen doesn't flipped when pitch is < 90.0f. */
        void processMouseMovement( float p_xoffset, float p_yoffset, GLboolean p_constrainPitch = true );
        /** Processes input received from a mouse scroll-wheel.
        *  \param[in]  p_yoffset     Vertical wheel-axis offset. */
        void processMouseScroll( float p_yoffset );

        /** Set camera mode.
        *  \param[in]  p_mode        Camera mode. */
        void setCameraMode( CameraMode p_mode );

        /** Get yaw angle.
        *  \return float             Yaw angle. */
        float yaw( ) const;
        /** Add yaw angle.
        *  \param[in]  p_yaw         Angle to add to yaw angle. */
        void addYaw( float p_yaw );
        /** Set yaw angle.
        *  \param[in]  p_yaw         Yaw angle to set. */
        void setYaw( float p_yaw );

        /** Get pitch angle.
        *  \return float             Pitch angle. */
        float pitch( ) const;
        /** Add pitch angle.
        *  \param[in]  p_pitch       Angle to add to pitch angle. */
        void addPitch( float p_pitch );
        /** Set pitch angle.
        *  \param[in]  p_pitch       Pitch angle to set. */
        void setPitch( float p_pitch );

        /** Get camera distance.
        *  \return float             Camera distance. */
        float distance( ) const;
        /** Set camera distance.
        *  \param[in]  p_distance    Camera distance to set. */
        void setDistance( float p_distance );

        /** Get pivot.
        *  \return glm::vec3&        Pivot. */
        const glm::vec3& pivot( ) const;
        /** Pan the camera.
        *  \param[in]  p_x           X offset.
        *  \param[in]  p_y           Y offset. */
        void pan( float p_x, float p_y );
        /** Set pivot.
        *  \param[in]  p_pivot       Pivot to set. */
        void setPivot( const glm::vec3& p_pivot );

        /** Get camera position.
        *  \return glm::vec3&        Camera position. */
        const glm::vec3& position( ) const;
        /** Set camera position.
        *  \param[in]  p_position    Camera position to set. */
        void setPosition( const glm::vec3& p_position );

        /** Set mouse sensitivity.
        *  \param[in]  p_sensitivity Mouse sensitivity to set. */
        void setMouseSensitivity( float p_sensitivity );

    private:
        /** Calculates the front vector from the Camera's (updated) Eular Angles. */
        void updateCameraVectors( );

    }; // class Camera

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_CAMERA_H_INCLUDED
