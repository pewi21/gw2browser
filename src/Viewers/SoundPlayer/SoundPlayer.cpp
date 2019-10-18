/** \file       Viewers/SoundPlayer/SoundPlayer.cpp
 *  \brief      Contains definition of the sound player.
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

#include "SoundPlayer.h"
#include "Exception.h"
#include "EventId.h"
#include "SoundDecoder.h"

#include "Data.h"

namespace gw2b {

    SoundPlayer::SoundPlayer( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
        : Viewer( p_parent, p_pos, p_size ) {

        // Initialize OpenAL
        if ( !m_alInitialized ) {
            if ( !this->initOAL( ) ) {
                throw exception::Exception( "Could not initialize OpenAL." );
            }
            m_alInitialized = true;
        }

        auto sizer = new wxBoxSizer( wxVERTICAL );
        //auto vsizer1 = new wxBoxSizer( wxVERTICAL );
        auto hsizer1 = new wxBoxSizer( wxHORIZONTAL );

        // List control
        m_listCtrl = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_THEME );
        sizer->Add( m_listCtrl, wxSizerFlags( ).Expand( ).Proportion( 1 ) );

        // Playback slider
        //m_slider = new wxSlider( this, ID_SliderPlayback, 0, 0, 1, wxDefaultPosition, wxSize( 400, 25) );
        //vsizer1->Add( m_slider );
        //sizer->Add( vsizer1 );

        // Left sound control
        auto back = new wxButton( this, ID_BtnBack, wxT( "|<" ) );
        auto play = new wxButton( this, ID_BtnPlay, wxT( ">" ) );
        auto stop = new wxButton( this, ID_BtnStop, wxT( "[]" ) );
        auto forward = new wxButton( this, ID_BtnForward, wxT( ">|" ) );
        m_volSlider = new wxSlider( this, ID_SliderVolume, 100, 0, 100, wxDefaultPosition, wxSize( 100, 25 ) );

        hsizer1->Add( back, 0, wxLEFT | wxTOP | wxBOTTOM, 5 );
        hsizer1->Add( play, 0, wxLEFT | wxTOP | wxBOTTOM, 5 );
        hsizer1->Add( stop, 0, wxLEFT | wxTOP | wxBOTTOM, 5 );
        hsizer1->Add( forward, 0, wxLEFT | wxTOP | wxBOTTOM, 5 );
        hsizer1->Add( m_volSlider, 0, wxALL, 5 );

        sizer->Add( hsizer1 );

        // Layout
        this->SetSizer( sizer );
        this->Layout( );

        // List control events
        this->Bind( wxEVT_LIST_ITEM_ACTIVATED, &SoundPlayer::onListItemDoubleClickedEvt, this );
        // Button events
        this->Bind( wxEVT_BUTTON, &SoundPlayer::onButtonEvt, this );
        // Volume events
        this->Bind( wxEVT_SLIDER, &SoundPlayer::onVolChange, this );
    }

    SoundPlayer::~SoundPlayer( ) {
        this->stopSound( );

        // Delete buffers
        alDeleteBuffers( NUM_BUFFERS, m_buffers );

        // Close OpenAL
        alcMakeContextCurrent( NULL );
        alcDestroyContext( m_context );
        alcCloseDevice( m_device );
    }

    void SoundPlayer::clear( ) {
        this->stopSound( );

        // Clear list control content
        m_listCtrl->ClearAll( );

        m_sound.clear( );
        Viewer::clear( );
    }

    void SoundPlayer::setReader( FileReader* p_reader ) {
        if ( !m_alInitialized ) {
            // Could not initialize OpenAL
            return;
        }

        Viewer::setReader( p_reader );
        if ( p_reader ) {
            if ( isOfType<SoundBankReader>( p_reader ) ) {
                if ( p_reader ) {
                    auto reader = this->sndBankReader( );
                    m_sound = reader->getSoundData( );
                }
            } else if ( isOfType<PackedSoundReader>( p_reader ) ) {
                if ( p_reader ) {
                    auto reader = this->pfSoundReader( );
                    auto sound = reader->getSoundData( );

                    SoundBank tmp;
                    tmp.voiceId = 0;
                    tmp.data = sound;

                    m_sound.push_back( tmp );
                }
            } else if ( isOfType<asndMP3Reader>( p_reader ) ) {
                if ( p_reader ) {
                    auto reader = this->asndSoundReader( );
                    auto sound = reader->getMP3Data( );

                    SoundBank tmp;
                    tmp.voiceId = 0;
                    tmp.data = sound;

                    m_sound.push_back( tmp );
                }
            }
            // Populate the list control with data list
            this->populateListCtrl( );

            if ( !m_sound.empty( ) ) {
                // Select the first entry
                this->selectEntry( 0 );
                //Play the sound
                this->playSound( 0 );
            }
        }
    }

    void SoundPlayer::printDeviceList( const char *p_list ) {
        if ( !p_list || *p_list == '\0' ) {
            wxLogMessage( wxT( "    !!! NONE !!!" ) );
        } else {
            do {
                wxLogMessage( wxT( "    %s" ), p_list );
                p_list += strlen( p_list ) + 1;
            } while ( *p_list != '\0' );
        }
    }

    bool SoundPlayer::initOAL( ) {
        wxLogMessage( wxT( "Initializing OpenAL..." ) );

        if ( !alcIsExtensionPresent( NULL, "ALC_ENUMERATION_EXT" ) ) {
            wxLogMessage( wxT( "Enumeration extension not available." ) );
        }

        wxLogMessage( wxT( "Available playback devices:" ) );
        if ( !alcIsExtensionPresent( NULL, "ALC_ENUMERATE_ALL_EXT" ) ) {
            this->printDeviceList( alcGetString( NULL, ALC_ALL_DEVICES_SPECIFIER ) );
        } else {
            this->printDeviceList( alcGetString( NULL, ALC_DEVICE_SPECIFIER ) );
        }

        if ( !alcIsExtensionPresent( NULL, "ALC_ENUMERATE_ALL_EXT" ) ) {
            wxLogMessage( wxT( "Default playback device: %s" ), alcGetString( NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER ) );
        } else {
            wxLogMessage( wxT( "Default playback device: %s" ), alcGetString( NULL, ALC_DEFAULT_DEVICE_SPECIFIER ) );
        }

        const ALCchar* defaultDeviceName = alcGetString( NULL, ALC_DEFAULT_DEVICE_SPECIFIER );

        m_device = alcOpenDevice( defaultDeviceName );
        if ( !m_device ) {
            wxLogMessage( wxT( "Unable to open default device." ) );
            return false;
        }

        // Clear error code
        alGetError( );

        m_context = alcCreateContext( m_device, NULL );
        if ( !alcMakeContextCurrent( m_context ) ) {
            alcCloseDevice( m_device );
            wxLogMessage( wxT( "Failed to make default context." ) );
            return false;
        }

        wxLogMessage( wxT( "OpenAL vendor string: %s" ), alGetString( AL_VENDOR ) );
        wxLogMessage( wxT( "OpenAL renderer string: %s" ), alGetString( AL_RENDERER ) );
        wxLogMessage( wxT( "OpenAL version string: %s" ), alGetString( AL_VERSION ) );

        // Generate buffers
        alGenBuffers( NUM_BUFFERS, m_buffers );

        return true;
    }

    void SoundPlayer::populateListCtrl( ) {
        wxListItem itemCol;
        itemCol.SetText( wxT( "" ) );
        m_listCtrl->InsertColumn( 0, itemCol );

        itemCol.SetText( wxT( "Entry" ) );
        itemCol.SetAlign( wxLIST_FORMAT_CENTRE );
        m_listCtrl->InsertColumn( 1, itemCol );

        //itemCol.SetText( wxT( "Lenght" ) );
        //itemCol.SetAlign( wxLIST_FORMAT_LEFT );
        //m_listCtrl->InsertColumn( 2, itemCol );

        // to speed up inserting we hide the control temporarily
        m_listCtrl->Hide( );

        for ( size_t i = 0; i < m_sound.size( ); i++ ) {
            this->insertItem( i );
        }

        m_listCtrl->Show( );

        m_listCtrl->SetColumnWidth( 0, 20 );
        m_listCtrl->SetColumnWidth( 1, 100 );
        //m_listCtrl->SetColumnWidth( 2, wxLIST_AUTOSIZE );

    }

    void SoundPlayer::insertItem( const int p_index ) {
        wxString buf;
        buf.Printf( wxT( "*" ) );
        long tmp = m_listCtrl->InsertItem( p_index, buf, 0 );
        m_listCtrl->SetItemData( tmp, p_index );

        buf.Printf( wxT( "%u" ), m_sound[p_index].voiceId );
        m_listCtrl->SetItem( tmp, 1, buf );

        //buf.Printf( wxT( "Item %d" ), p_index );
        //m_listCtrl->SetItem( tmp, 2, buf );
    }

    void SoundPlayer::selectEntry( const long p_index ) {
        m_listCtrl->SetItemState( p_index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED );

        auto sel = m_listCtrl->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
        if ( sel != -1 ) {
            m_listCtrl->SetItemState( sel, 0, wxLIST_STATE_SELECTED );
        }
        m_listCtrl->SetItemState( p_index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    }

    bool SoundPlayer::playSoundThread( const int p_index ) {
        // Copy the data to this thread function, since some times the thread isn't finish
        // but the data already destroyed when play new file
        auto localData = m_sound[p_index].data;

        if ( p_index < m_listCtrl->GetItemCount( ) ) {
            // Set first column text to indicate playing
            m_listCtrl->SetItem( p_index, 0, wxT( ">" ) );
        }

        m_isPlaying = true;
        m_isThreadEnded = false;

        // Set sound orientation
        {
            // listener position
            alListener3f( AL_POSITION, m_listenerPos.x, m_listenerPos.y, m_listenerPos.z );
            // listener velocity
            alListener3f( AL_VELOCITY, m_listenerVel.x, m_listenerVel.y, m_listenerVel.z );
            // listener orientation
            alListenerfv( AL_ORIENTATION, m_listenerOri );
        }

        // Generate source
        alGenSources( 1, &m_source );

        // Set sound source
        {
            // source pitch
            alSourcef( m_source, AL_PITCH, m_sourcePitch );
            // source gain
            alSourcef( m_source, AL_GAIN, m_sourceGain );
            // source position
            alSource3f( m_source, AL_POSITION, m_sourcePos.x, m_sourcePos.y, m_sourcePos.z );
            // source velocity
            alSource3f( m_source, AL_VELOCITY, m_sourceVel.x, m_sourceVel.y, m_sourceVel.z );
            // source looping
            alSourcei( m_source, AL_LOOPING, AL_FALSE );
        }

        SoundDecoder* decoder;
        try {
            // Initialize the sound decoder
            decoder = new SoundDecoder( localData );
        } catch ( const exception::Exception& err ) {
            wxLogMessage( wxT( "Can't initialize sound decoder : %s" ), wxString( err.what( ) ) );
            return false;
        }

        // Allocate buffer
        auto buf = allocate<char>( m_bufferSize * sizeof( ALshort ) );

        size_t bytes = 0;

        // Buffer audio data to OpenAL
        for ( int i = 0; i < NUM_BUFFERS; i++ ) {
            bytes = decoder->read( buf, m_bufferSize, m_buffers[i] );
            // Buffer decoded sound data to OpenAL buffer
            alBufferData( m_buffers[i], decoder->getFormat( ), buf, bytes, decoder->getFrequency( ) );

            if ( alGetError( ) != AL_NO_ERROR ) {
                freePointer( buf );
                wxLogMessage( wxT( "Error buffering sound data." ) );
                return false;
            }
        }

        // Queue the buffers onto the source
        alSourceQueueBuffers( m_source, NUM_BUFFERS, m_buffers );

        // Start playback
        alSourcePlay( m_source );
        if ( alGetError( ) != AL_NO_ERROR ) {
            freePointer( buf );
            wxLogMessage( wxT( "Error start playing sound." ) );
            return false;
        }

        ALint state;
        alGetSourcei( m_source, AL_SOURCE_STATE, &state );

        while ( ( bytes != 0 ) && m_isPlaying ) {
            ALuint buffer;
            ALint bufferProcessed;
            ALint queuedBuffers;

            // sleep 10ms to make it not use too much CPU time
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

            // Check if OpenAL is processed the buffer
            alGetSourcei( m_source, AL_BUFFERS_PROCESSED, &bufferProcessed );

            if ( bufferProcessed ) {
                // For each processed buffer
                while ( bufferProcessed-- ) {
                    // Remove processed buffer from the source queue
                    alSourceUnqueueBuffers( m_source, 1, &buffer );
                    // Read the next chunk of decoded audio data and fill the old buffer with new data
                    bytes = decoder->read( buf, m_bufferSize, buffer );
                    // Buffer decoded sound data to OpenAL buffer
                    alBufferData( buffer, decoder->getFormat( ), buf, bytes, decoder->getFrequency( ) );

                    if ( bytes != 0 ) {
                        // Insert the buffer to the source queue
                        alSourceQueueBuffers( m_source, 1, &buffer );
                        if ( alGetError( ) != AL_NO_ERROR ) {
                            wxLogMessage( wxT( "Error buffering sound data." ) );
                            break;
                        }
                    }
                }
            }

            alGetSourcei( m_source, AL_SOURCE_STATE, &state );
            if ( state != AL_PLAYING ) {
                alGetSourcei( m_source, AL_BUFFERS_QUEUED, &queuedBuffers );
                if ( queuedBuffers ) {
                    alSourcePlay( m_source );
                } else {
                    break;
                }
            }
        }

        // destroy the decoder
        delete decoder;

        // Delete source
        alDeleteSources( 1, &m_source );

        // Clean up
        freePointer( buf );

        m_isPlaying = false;
        m_isThreadEnded = true;

        if ( p_index < m_listCtrl->GetItemCount( ) ) {
            // Reset first column text
            m_listCtrl->SetItem( p_index, 0, wxT( "*" ) );
        }

        return true;
    }

    void SoundPlayer::playSound( const int p_index ) {
        // Stop playing sound, if still playing it
        this->stopSound( );
        // Play the first entry
        m_thread = std::thread( &SoundPlayer::playSoundThread, this, p_index );
        m_thread.detach( );
    }

    void SoundPlayer::stopSound( ) {
        // Is the sound still playing?
        if ( this->playing( ) ) {
            // Stop OpenAL to play sound
            alSourceStop( m_source );
            // Signal player thread to stop playing
            m_isPlaying = false;
        }
        // Wait until the thread is finished
        while ( !m_isThreadEnded ) {
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        }
    }

    bool SoundPlayer::playing( ) {
        return m_isPlaying;
    }

    void SoundPlayer::setVolume( const ALfloat p_vol ) {
        alListenerf( AL_GAIN, p_vol );
    }

    void SoundPlayer::onListItemDoubleClickedEvt( wxListEvent& p_event ) {
        auto index = p_event.m_itemIndex;
        this->playSound( index );
    }

    void SoundPlayer::onButtonEvt( wxCommandEvent& p_event ) {
        auto id = p_event.GetId( );
        switch ( id ) {
        case ID_BtnBack:
            this->onPrev( );
            break;
        case ID_BtnPlay:
            this->onPlay( );
            // Todo:change play button label
            break;
        case ID_BtnStop:
            this->onStop( );
            break;
        case ID_BtnForward:
            this->onNext( );
            break;
        }
    }

    void SoundPlayer::onPlay( ) {
        long index = -1;
        if ( m_listCtrl->GetItemCount( ) == 0 ) {
            return;
        }
        while ( ( index = m_listCtrl->GetNextItem( index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) ) != wxNOT_FOUND ) {
            this->playSound( index );
        }
    }

    void SoundPlayer::onPause( ) {
        // Pause the sound
        alSourcePause( m_source );
    }

    void SoundPlayer::onStop( ) {
        this->stopSound( );
    }

    void SoundPlayer::onNext( ) {
        long index = -1;

        auto count = m_listCtrl->GetItemCount( );
        if ( count == 0 ) {
            return;
        }

        while ( ( index = m_listCtrl->GetNextItem( index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) ) != wxNOT_FOUND ) {
            index += 1;
            if ( index > ( count - 1 ) ) {
                index = 0;
            }
            this->selectEntry( index );
            this->playSound( index );
        }
    }

    void SoundPlayer::onPrev( ) {
        long index = -1;

        auto count = m_listCtrl->GetItemCount( );
        if ( count == 0 ) {
            return;
        }

        while ( ( index = m_listCtrl->GetNextItem( index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) ) != wxNOT_FOUND ) {
            index -= 1;
            if ( index < 0 ) {
                index = count - 1;
            }
            this->selectEntry( index );
            this->playSound( index );
        }
    }

    void SoundPlayer::onVolChange( wxCommandEvent& WXUNUSED( p_event ) ) {
        this->setVolume( m_volSlider->GetValue( ) / 100.0 );
    }

}; // namespace gw2b
