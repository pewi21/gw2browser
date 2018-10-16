/* \file       Viewers/SoundPlayer/SoundDecoder.cpp
*  \brief      Contains definition of the SoundDecoder class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2017 Khral Steelforge <https://github.com/kytulendu>

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

#include "Exception.h"

#include "SoundDecoder.h"

namespace gw2b {

    SoundDecoder::SoundDecoder( Array<byte>& p_data )
        : m_data( p_data ) {

        auto data = m_data.GetPointer( );
        auto size = m_data.GetSize( );

        auto const fourcc = *reinterpret_cast<uint32*>( data );
        if ( fourcc == FCC_OggS ) {
            m_codec = DEC_OGG;

            if ( !this->loadOggs( reinterpret_cast<char*>( data ), size, &m_oggFile, m_oggStream, m_oggCallbacks ) ) {
                throw exception::Exception( "Error loading Oggs data." );
            }
        } else if ( ( fourcc & 0xffff ) == FCC_MP3 ) {
            m_codec = DEC_MP3;

            int error;
            // Initialize mpg123 library
            mpg123_init( );
            // Create new mpg123 handle
            m_mpg123Handle = mpg123_new( NULL, &error );
            if ( m_mpg123Handle ) {
                if ( !this->loadMp3( reinterpret_cast<char*>( data ), size, m_mpg123Handle ) ) {
                    throw exception::Exception( "Error loading MP3 data." );
                }
            }
        }
    }

    SoundDecoder::~SoundDecoder( ) {
        if ( m_codec == DEC_OGG ) {
            ov_clear( &m_oggFile );
        } else if ( m_codec == DEC_MP3 ) {
            if ( mpg123_close( m_mpg123Handle ) != MPG123_OK ) {
                wxLogMessage( wxT( "Error closing mpg123 handle." ) );
            }
            mpg123_delete( m_mpg123Handle );
            mpg123_exit( );
        }
    }

    size_t SoundDecoder::read( char* p_data, const ALsizei p_count, ALuint p_buffer ) {
        if ( m_codec == DEC_OGG ) {
            return ( this->readOggs( p_data, p_count, &m_oggFile, p_buffer ) );
        } else if ( m_codec == DEC_MP3 ) {
            return ( this->readMp3( p_data, p_count, m_mpg123Handle, p_buffer ) );
        }
        return 0;
    }

    ALsizei SoundDecoder::getFrequency( ) {
        return m_frequency;
    }

    ALenum SoundDecoder::getFormat( ) {
        return m_format;
    }

    bool SoundDecoder::loadOggs( char* p_data, const size_t p_size, OggVorbis_File* p_oggFile, ogg_file& p_oggStream, ov_callbacks& p_oggCallbacks ) {
        p_oggStream.curPtr = p_oggStream.filePtr = p_data;
        p_oggStream.fileSize = p_size;

        p_oggCallbacks.read_func = readOgg;
        p_oggCallbacks.seek_func = seekOgg;
        p_oggCallbacks.close_func = closeOgg;
        p_oggCallbacks.tell_func = tellOgg;

        auto ret = ov_open_callbacks( static_cast<void*>( &p_oggStream ), p_oggFile, NULL, -1, p_oggCallbacks );
        if ( ret ) {
            wxLogMessage( wxT( "ov_open_callbacks error :(" ) );
            switch ( ret ) {
            case OV_EREAD:
                wxLogMessage( wxT( "A read from media returned an error." ) );
                break;
            case OV_ENOTVORBIS:
                wxLogMessage( wxT( "Bitstream does not contain any Vorbis data." ) );
                break;
            case OV_EVERSION:
                wxLogMessage( wxT( "Vorbis version mismatch." ) );
                break;
            case OV_EBADHEADER:
                wxLogMessage( wxT( "Invalid Vorbis bitstream header." ) );
                break;
            case OV_EFAULT:
                wxLogMessage( wxT( "Internal logic fault; indicates a bug or heap/stack corruption." ) );
                break;
            default:
                wxLogMessage( wxT( "Unknown error." ) );
            }
            return false;
        }

        // Get some information about the OGG file
        vorbis_info* oggInfo = ov_info( p_oggFile, -1 );
        if ( !oggInfo ) {
            ov_clear( p_oggFile );
            wxLogMessage( wxT( "Can't get vorbis information :(" ) );
            return false;
        }

        vorbis_comment* oggComments = ov_comment( p_oggFile, -1 );
        char **ptr = oggComments->user_comments;
        while ( *ptr ) {
            wxLogMessage( wxT( "%s" ), *ptr );
            ++ptr;
        }
        wxLogMessage( wxT( "Bitstream is Ogg Vorbis, %d channel, %ld Hz" ), oggInfo->channels, oggInfo->rate );
        wxLogMessage( wxT( "Bitrate (variable): %ld kb/s" ), oggInfo->bitrate_nominal / 1000 );
        wxLogMessage( wxT( "Decoded length: %ld samples" ), static_cast<long>( ov_pcm_total( p_oggFile, -1 ) ) );
        wxLogMessage( wxT( "Encoded by: %s" ), oggComments->vendor );

        // Check the number of channels
        if ( oggInfo->channels == 1 ) {
            m_format = AL_FORMAT_MONO16;
        } else {
            m_format = AL_FORMAT_STEREO16;
        }

        // The frequency of the sampling rate
        m_frequency = oggInfo->rate;

        return true;
    }

    size_t SoundDecoder::readOggs( char* p_data, const ALsizei p_count, OggVorbis_File* p_oggFile, ALuint p_buffer ) {
        size_t size = p_count * sizeof( ALshort );
        size_t bytes = 0;
        while ( bytes < size ) {
            auto read = ov_read( p_oggFile, p_data + bytes, size - bytes, 0, 2, 1, 0 );
            if ( read > 0 ) {
                bytes += read;
            } else {
                break;
            }
        }

        //wxLogMessage( wxT( "Read %d bytes" ), bytes / 2 );
        return bytes;
    }

    bool SoundDecoder::loadMp3( char* p_data, const size_t p_size, mpg123_handle* p_handle ) {
        auto data = reinterpret_cast<unsigned char*>( p_data );

        long sampleRate;
        int channels;
        int encoding;

        if ( mpg123_open_feed( p_handle ) != MPG123_OK ) {
            mpg123_delete( p_handle );
            return false;
        }

        if ( mpg123_feed( p_handle, data, p_size ) != MPG123_OK ) {
            mpg123_delete( p_handle );
            return false;
        }

        if ( mpg123_getformat( p_handle, &sampleRate, &channels, &encoding ) != MPG123_OK ) {
            mpg123_close( p_handle );
            mpg123_delete( p_handle );
            return false;
        }

        if ( ( channels == 1 || channels == 2 ) && sampleRate > 0 ) {
            if ( mpg123_format_none( p_handle ) == MPG123_OK ) {
                if ( mpg123_format( p_handle, sampleRate, channels, MPG123_ENC_SIGNED_16 ) == MPG123_OK ) {
                    // Set sample rate
                    m_frequency = sampleRate;
                    // set number of channels
                    if ( channels == 1 ) {
                        m_format = AL_FORMAT_MONO16;
                    } else {
                        m_format = AL_FORMAT_STEREO16;
                    }

                    mpg123_scan( p_handle );

                    // Get some information about the Mp3 file
                    mpg123_frameinfo info;
                    mpg123_info( p_handle, &info );

                    wxString versionString;
                    switch ( info.version ) {
                    case MPG123_1_0:
                        versionString = wxT( "MPEG Version 1.0" );
                        break;
                    case MPG123_2_0:
                        versionString = wxT( "MPEG Version 2.0" );
                        break;
                    case MPG123_2_5:
                        versionString = wxT( "MPEG Version 2.5" );
                        break;
                    }

                    wxString channelString;
                    switch ( info.mode ) {
                    case MPG123_M_STEREO:
                        channelString = wxT( "Standard Stereo" );
                        break;
                    case MPG123_M_JOINT:
                        channelString = wxT( "Joint Stereo" );
                        break;
                    case MPG123_M_DUAL:
                        channelString = wxT( "Dual Channel" );
                        break;
                    case MPG123_M_MONO:
                        channelString = wxT( "Single Channel" );
                        break;
                    }

                    wxLogMessage( wxT( "Bitstream is %s Layer %d, %s, %ld Hz " ), versionString, info.layer, channelString, info.rate );

                    wxString modeString;
                    switch ( info.vbr ) {
                    case MPG123_CBR:
                        modeString = wxT( "Constant Bitrate" );
                        break;
                    case MPG123_VBR:
                        modeString = wxT( "Variable Bitrate" );
                        break;
                    case MPG123_ABR:
                        modeString = wxT( "Average Bitrate" );
                        break;
                    }
                    wxLogMessage( wxT( "Bitrate: %s mode, %d kb/s" ), modeString, info.bitrate );
                    wxLogMessage( wxT( "Duration: %ld seconds" ), ( mpg123_length( p_handle ) / info.rate ) );

                    return true;
                }
            }
        }

        mpg123_close( p_handle );
        mpg123_delete( p_handle );

        return false;
    }

    size_t SoundDecoder::readMp3( char* p_data, const ALsizei p_count, mpg123_handle* p_handle, ALuint p_buffer ) {
        auto buffer = reinterpret_cast<unsigned char*>( p_data );
        size_t bytes = 0;

        int ret = mpg123_decode( p_handle, NULL, 0, buffer, p_count, &bytes );
        if ( ret > MPG123_OK && ret != MPG123_NEED_MORE ) {
            wxLogMessage( wxT( "Decoding error %d" ), ret );
            return 0;
        }

        //wxLogMessage( wxT( "Read %d bytes" ), bytes );
        return bytes;
    }

}; // namespace gw2b
