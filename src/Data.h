/** \file       Data.h
 *  \brief      Embeds for the various files in the data folder. This is to
 *              make the exe self-sufficient, without platform dependency.
 *  \author     Rhoot
 */

/**
 * Copyright (C) 2014-2018 Khral Steelforge <https://github.com/kytulendu>
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

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

namespace gw2b {

    namespace data {

        /** Loads a PNG from memory and returns a new wxBitmap containing it.
        *  \param[in]  p_data   PNG data to load.
        *  \param[in]  p_size   Size of the PNG data.
        *  \return wxBitmap    Bitmap containing the loaded PNG. */
        wxBitmap loadPNG(const byte* p_data, size_t p_size);

        wxBitmap loadImage(const char* p_path);

    }; // namespace data

}; // namespace gw2b

#endif // DATA_H_INCLUDED
