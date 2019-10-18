/** \file       Readers/ContentReader.h
 *  \brief      Contains the declaration of the content manifest reader class.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2018 Khralkatorrix <https://github.com/kytulendu>
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

#ifndef READERS_CONTENTREADER_H_INCLUDED
#define READERS_CONTENTREADER_H_INCLUDED

#include "FileReader.h"

namespace gw2b {

    class ContentReader : public FileReader {
    public:
        /** Constructor.
        *  \param[in]  p_data       Data to be handled by this reader.
        *  \param[in]  p_datFile    Reference to an instance of DatFile.
        *  \param[in]  p_fileType   File type of the given data. */
        ContentReader( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType );
        /** Destructor. Clears all data. */
        virtual ~ContentReader( );

        /** Gets the type of data contained in this file. Not to be confused with
        *  file type.
        *  \return DataType    type of data. */
        virtual DataType dataType( ) const override {
            return DT_Content;
        }
        /** Gets the pointer to converted xml object owned by this reader.
        *  \return std::unique_ptr<tinyxml2::XMLDocument>    pointer to xml formatted game content data. */
        std::unique_ptr<tinyxml2::XMLDocument> getContentData( ) const;

    }; // class ContentReader

}; // namespace gw2b

#endif // READERS_CONTENTREADER_H_INCLUDED
