/** \file       Data.h
 *  \brief      Data related functions.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2014-2019 Khralkatorrix <https://github.com/kytulendu>
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

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

namespace gw2b {

    /** Loads a PNG from memory and returns a new wxBitmap containing it.
    *  \param[in]  p_data   PNG data to load.
    *  \param[in]  p_size   Size of the PNG data.
    *  \return wxBitmap    Bitmap containing the loaded PNG. */
    wxBitmap loadPNG(const byte* p_data, size_t p_size);

    /** Loads an image from an image file and returns a new wxBitmap containing it.
    *  \param[in]  p_path   Path of image file to load.
    *  \return wxBitmap    Bitmap containing the loaded PNG. */
    wxBitmap loadImage(wxString p_path);

    /** Get full path of a file that was in gw2browser's data directory.
    *  \param[in]  p_path   File path in relation to gw2browser's data directory.
    *  \return wxString    Full file path. */
    wxString getPath(const char* p_filePath);

}; // namespace gw2b

#endif // DATA_H_INCLUDED
