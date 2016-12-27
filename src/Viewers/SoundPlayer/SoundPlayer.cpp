/* \file       Viewers/SoundPlayer/SoundPlayer.cpp
*  \brief      Contains definition of the sound player.
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

#include "SoundPlayer.h"
#include "Exception.h"
#include "OggCallback.h"

#include "Readers/ImageReader.h"
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

			//m_renderTimer = new RenderTimer( this );
			//m_renderTimer->start( );
		}

		auto sizer = new wxBoxSizer( wxHORIZONTAL );

		// List control
		m_listCtrl = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_THEME );
		sizer->Add( m_listCtrl, wxSizerFlags( ).Expand( ).Proportion( 1 ) );

		// Layout
		this->SetSizer( sizer );
		this->Layout( );

		this->Bind( wxEVT_LIST_ITEM_ACTIVATED, &SoundPlayer::onListItemDoubleClickedEvt, this );
	}

	SoundPlayer::~SoundPlayer( ) {
		// Delete buffers
		alDeleteBuffers( NUM_BUFFERS, m_buffers );

		// Close OpenAL
		alcMakeContextCurrent( NULL );
		alcDestroyContext( m_context );
		alcCloseDevice( m_device );
	}

	void SoundPlayer::clear( ) {
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
			} else {
				// Hopefully, there aren't things gones here.
				return;
			}

			this->populateListCtrl( );

			// Select the first entry
			this->selectEntry( 0 );
			// Play the first entry
			this->playSound( 0 );
		}
	}

	void SoundPlayer::printDeviceList( const char *p_list ) {
		if ( !p_list || *p_list == '\0' ) {
			printf( "    !!! none !!!\n" );
			wxLogMessage( wxT( "    !!! NONE !!!" ) );
		} else
			do {
				wxLogMessage( wxT( "    %s" ), p_list );
				p_list += strlen( p_list ) + 1;
			} while ( *p_list != '\0' );
	}

	int SoundPlayer::initOAL( ) {
		wxLogMessage( wxT( "Initializing OpenAL..." ) );

		ALboolean enumeration = alcIsExtensionPresent( NULL, "ALC_ENUMERATION_EXT" );
		if ( enumeration == AL_FALSE ) {
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

		const ALCchar *defaultDeviceName = alcGetString( NULL, ALC_DEFAULT_DEVICE_SPECIFIER );

		m_device = alcOpenDevice( defaultDeviceName );
		if ( !m_device ) {
			wxLogMessage( wxT( "Unable to open default device." ) );
			return false;
		}

		alGetError( );

		m_context = alcCreateContext( m_device, NULL );
		if ( !alcMakeContextCurrent( m_context ) ) {
			wxLogMessage( wxT( "Failed to make default context." ) );
			return false;
		}

		wxLogMessage( wxT( "OpenAL vendor string: %s" ), alGetString( AL_VENDOR ) );
		wxLogMessage( wxT( "OpenAL renderer string: %s" ), alGetString( AL_RENDERER ) );
		wxLogMessage( wxT( "OpenAL version string: %s" ), alGetString( AL_VERSION ) );

		// set sound orientation
		{
			// listener position
			alListener3f( AL_POSITION, m_listenerPos.x, m_listenerPos.y, m_listenerPos.z );
			// listener velocity
			alListener3f( AL_VELOCITY, m_listenerVel.x, m_listenerVel.y, m_listenerVel.z );
			// listener orientation
			alListenerfv( AL_ORIENTATION, m_listenerOri );
		}

		// Generate buffers
		alGenBuffers( NUM_BUFFERS, m_buffers );

		return true;
	}

	void SoundPlayer::populateListCtrl( ) {
		// note that under MSW for SetColumnWidth() to work we need to create the
		// items with images initially even if we specify dummy image id
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

	bool SoundPlayer::readOggs( char* p_databuffer, ALuint p_buffer, ALsizei p_count, ALenum p_format, ALsizei p_freqency ) {
		// 'count' should not be > MAX_AUDIO_SAMPLES
		ALsizei acc = 0;
		ALsizei size = p_count * sizeof( ALshort );
		while ( acc < size ) {
			ALsizei read = ov_read( &m_oggFile, p_databuffer + acc, size - acc, 0, 2, 1, 0 );
			if ( read > 0 ) {
				acc += read;
			} else {
				break;
			}
		}

		if ( acc > 0 ) {
			alBufferData( p_buffer, p_format, p_databuffer, acc, p_freqency );
			wxLogMessage( wxT( "Read %d bytes" ), acc / 2 );
		} else {
			return false;
		}

		return true;
	}

	void SoundPlayer::playSound( const int p_index ) {
		auto data = m_sound[p_index].data.GetPointer( );
		auto size = m_sound[p_index].data.GetSize( );

		// Generate source
		alGenSources( ( ALuint ) 1, &m_source );

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

		auto const fourcc = *reinterpret_cast<uint32*>( data );
		if ( fourcc == FCC_OggS ) {
			ov_callbacks callbacks;
			ogg_file t;
			t.curPtr = t.filePtr = reinterpret_cast<char*>( data );
			t.fileSize = size;

			callbacks.read_func = readOgg;
			callbacks.seek_func = seekOgg;
			callbacks.close_func = closeOgg;
			callbacks.tell_func = tellOgg;

			int ret1 = ov_open_callbacks( static_cast<void*>( &t ), &m_oggFile, NULL, -1, callbacks );

			// Get some information about the OGG file
			vorbis_info* oggInfo = ov_info( &m_oggFile, -1 );
			assert( oggInfo );

			// Check the number of channels... always use 16-bit samples
			if ( oggInfo->channels == 1 ) {
				m_format = AL_FORMAT_MONO16;
			} else {
				m_format = AL_FORMAT_STEREO16;
			}

			// The frequency of the sampling rate
			m_frequency = oggInfo->rate;
		}

		// Allocate buffer
		auto buf = allocate<char>( m_bufferSize * sizeof( ALshort ) );

		bool ret = false;

		// Buffer audio data to OpenAL
		for ( int i = 0; i < NUM_BUFFERS; i++ ) {
			if ( fourcc == FCC_OggS ) {
				ret = readOggs( buf, m_buffers[i], m_bufferSize, m_format, m_frequency );
			}

			if ( alGetError( ) != AL_NO_ERROR ) {
				freePointer( buf );
				wxLogMessage( wxT( "Error loading :(" ) );
				return;
			}
		}

		// Queue the buffers onto the source, and start playback
		alSourceQueueBuffers( m_source, NUM_BUFFERS, m_buffers );
		alSourcePlay( m_source );
		if ( alGetError( ) != AL_NO_ERROR ) {
			freePointer( buf );
			wxLogMessage( wxT( "Error starting :(" ) );
			return;
		}

		ALint processed;
		// while (!thread_finish)
		while ( ret ) {
			ALuint buffer;

			// make it not use too much CPU time
			//_sleep( 1 ); // Sleep 1 sec

			// Check if OpenAL is processed the buffer
			alGetSourcei( m_source, AL_BUFFERS_PROCESSED, &processed );
			if ( processed ) {
				// For each processed buffer
				while ( processed-- ) {
					// Remove processed buffer from the source queue
					alSourceUnqueueBuffers( m_source, 1, &buffer );
					// Read the next chunk of decoded audio data and fill the old buffer with new data
					if ( fourcc == FCC_OggS ) {
						ret = readOggs( buf, buffer, m_bufferSize, m_format, m_frequency );
					}
					// Insert the buffer to the source queue
					alSourceQueueBuffers( m_source, 1, &buffer );
					if ( alGetError( ) != AL_NO_ERROR ) {
						freePointer( buf );
						wxLogMessage( wxT( "Error buffering :(" ) );
						return;
					}
				}
			}
		}

		//alSourceStop( m_source );

		// Clean up
		freePointer( buf );

		if ( fourcc == FCC_OggS ) {
			ov_clear( &m_oggFile );
		}

		// Delete source
		alDeleteSources( 1, &m_source );
	}

	void SoundPlayer::onListItemDoubleClickedEvt( wxListEvent& p_event ) {
		auto index = p_event.m_itemIndex;

		// stop current sound()

		this->playSound( index );
	}

}; // namespace gw2b
