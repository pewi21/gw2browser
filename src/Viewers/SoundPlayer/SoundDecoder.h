/** \file       Viewers/SoundPlayer/SoundDecoder.h
 *  \brief      Contains declaration of the SoundDecoder class.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2017 Khralkatorrix <https://github.com/kytulendu>
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

#ifndef VIEWERS_SOUNDPLAYER_SOUNDDECODER_H_INCLUDED
#define VIEWERS_SOUNDPLAYER_SOUNDDECODER_H_INCLUDED

// For AL* data types
#include <AL/al.h>

#define MPG123_ENUM_API
#include <src/include/mpg123.h>
#include <vorbis/vorbisfile.h>

#include "OggCallback.h"
#include "ANetStructs.h"

namespace gw2b {

    class SoundDecoder : public wxPanel {

        enum DecoderType {
            DEC_OGG = 0,
            DEC_MP3 = 1,
        };

        Array<byte>                 m_data;                             // Encoded sound data
        DecoderType                 m_codec;                            // Type of sound CODEC
        ALsizei                     m_frequency;                        // Frequency of the sound data
        ALenum                      m_format;                           // Sound data format
        OggVorbis_File              m_oggFile;
        ogg_file                    m_oggStream;
        ov_callbacks                m_oggCallbacks;
        mpg123_handle*              m_mpg123Handle = nullptr;

    public:
        /** Constructor. Creates the sound decoder according to given data.
        *  \param[in]  p_data       Array of data. */
        SoundDecoder( Array<byte>& p_data );
        /** Destructor. */
        virtual ~SoundDecoder( );

        /** Decode the given sound data.
        *  \param[in]  p_data       Pointer to data buffer.
        *  \param[in]  p_count      Size of buffer.
        *  \param[in]  p_buffer     OpenAL buffer to read into.
        *  \return size_t           Size of readed bytes. */
        size_t read( char* p_data, const ALsizei p_count, ALuint p_buffer );

        /** Get sound frequency.
        *  \return ALsizei          Sound frequency. */
        ALsizei getFrequency( );
        /** Get sound format.
        *  \return ALenum           Sound format. */
        ALenum getFormat( );

    private:
        bool loadOggs( char* p_data, const size_t p_size, OggVorbis_File* p_oggFile, ogg_file& p_oggStream, ov_callbacks& p_oggCallbacks );
        size_t readOggs( char* p_data, const ALsizei p_count, OggVorbis_File* p_oggFile, ALuint p_buffer );
        bool loadMp3( char* p_data, const size_t p_size, mpg123_handle* p_handle );
        size_t readMp3( char* p_data, const ALsizei p_count, mpg123_handle* p_handle, ALuint p_buffer );

    }; // class Decoder

}; // namespace gw2b

#endif // VIEWERS_SOUNDPLAYER_SOUNDDECODER_H_INCLUDED
