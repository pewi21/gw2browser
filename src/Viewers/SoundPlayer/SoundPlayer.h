/** \file       Viewers/SoundPlayer.h
 *  \brief      Contains declaration of the sound player.
 *  \author     Khral Steelforge
 */

/**
 * Copyright (C) 2016-2017 Khral Steelforge <https://github.com/kytulendu>
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

#ifndef VIEWERS_SOUNDPLAYER_SOUNDPLAYER_H_INCLUDED
#define VIEWERS_SOUNDPLAYER_SOUNDPLAYER_H_INCLUDED

#include <thread>
#include <vector>

#include <wx/listctrl.h>
#include <wx/slider.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "Readers/PackedSoundReader.h"
#include "Readers/SoundBankReader.h"
#include "Readers/asndMP3Reader.h"

#include "Viewer.h"

namespace gw2b {

    class SoundPlayer : public Viewer {
        // Data
        std::vector<SoundBank>      m_sound;
        wxListCtrl*                 m_listCtrl;
        wxSlider*                   m_volSlider;
        //wxSlider*                 m_slider;
        std::thread                 m_thread;

        // Constraint
        #define NUM_BUFFERS 3                                           // Number of buffers
        ALuint                      m_bufferSize = 4096;                // Buffer size

        // Internal status
        bool                        m_alInitialized = false;            // Is OpenAL is initialized?
        bool                        m_isPlaying = false;                // Is playing the sound?
        bool                        m_isThreadEnded = true;             // Is sound player thread ended?

        // OpenAL stuff
        ALCdevice*                  m_device;
        ALCcontext*                 m_context;
        ALuint                      m_buffers[NUM_BUFFERS];             // OpenAL sound buffer ID
        ALuint                      m_source;                           // OpenAL sound source

        // Sound orientation
        glm::vec3                   m_listenerPos = glm::vec3( 0.0f, 0.0f, 1.0f ); // Listener position
        glm::vec3                   m_listenerVel = glm::vec3( 1.0f, 0.0f, 0.0f ); // listener velocity
        ALfloat                     m_listenerOri[6] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f }; // Listener orientation

        // Sound source
        ALfloat                     m_sourcePitch = 1.0f;               // Source pitch
        ALfloat                     m_sourceGain = 1.0f;                // Source gain
        glm::vec3                   m_sourcePos = glm::vec3( 0.0f, 0.0f, 0.0f ); // Source position
        glm::vec3                   m_sourceVel = glm::vec3( 0.0f, 0.0f, 0.0f ); // Source velocity

    public:
        SoundPlayer( wxWindow* p_parent, const wxPoint& p_pos = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
        virtual ~SoundPlayer( );

        virtual void clear( ) override;
        virtual void setReader( FileReader* p_reader ) override;

        /** Gets the sound bank reader containing the data displayed by this viewer.
        *  \return SoundBankReader*    Reader containing the data. */
        SoundBankReader* sndBankReader( ) {
            return reinterpret_cast<SoundBankReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast
        /** Gets the sound bank reader containing the data displayed by this viewer.
        *  \return SoundBankReader*    Reader containing the data. */
        const SoundBankReader* sndBankReader( ) const {
            return reinterpret_cast<const SoundBankReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast

        /** Gets the image reader containing the data displayed by this viewer.
        *  \return PackedSoundReader*    Reader containing the data. */
        PackedSoundReader* pfSoundReader( ) {
            return reinterpret_cast<PackedSoundReader*>( this->reader( ) );
        }       // already asserted with a dynamic_cast
        /** Gets the image reader containing the data displayed by this viewer.
        *  \return PackedSoundReader*    Reader containing the data. */
        const PackedSoundReader* pfSoundReader( ) const {
            return reinterpret_cast<const PackedSoundReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast

        /** Gets the image reader containing the data displayed by this viewer.
        *  \return asndMP3Reader*    Reader containing the data. */
        asndMP3Reader* asndSoundReader( ) {
            return reinterpret_cast<asndMP3Reader*>( this->reader( ) );
        } // already asserted with a dynamic_cast
        /** Gets the image reader containing the data displayed by this viewer.
        *  \return asndMP3Reader*    Reader containing the data. */
        const asndMP3Reader* asndSoundReader( ) const {
            return reinterpret_cast<const asndMP3Reader*>( this->reader( ) );
        } // already asserted with a dynamic_cast

    private:
        void printDeviceList( const char *p_list );
        bool initOAL( );
        void populateListCtrl( );
        void insertItem( const int p_index );
        void selectEntry( const long p_index );
        bool playSoundThread( const int p_index );
        void playSound( const int p_index );
        void stopSound( );
        bool playing( );
        void setVolume( const ALfloat p_vol );
        void onListItemDoubleClickedEvt( wxListEvent& p_event );
        void onButtonEvt( wxCommandEvent& p_event );
        void onPlay( );
        void onPause( );
        void onStop( );
        void onNext( );
        void onPrev( );
        void onVolChange( wxCommandEvent& WXUNUSED( p_event ) );

    }; // class SoundPlayer

}; // namespace gw2b

#endif // VIEWERS_SOUNDPLAYER_SOUNDPLAYER_H_INCLUDED
