/* \file       Data.cpp
*  \brief      Embeds for the various files in the data folder. This is to
*              make the exe self-sufficient, without platform dependency.
*  \author     Rhoot
*/

/*
Copyright (C) 2014 Khral Steelforge <https://github.com/kytulendu>
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

#include "stdafx.h"

#include <wx/mstream.h>

#include "Data.h"

namespace gw2b {

    namespace data {

        wxBitmap loadPNG(const byte* p_data, size_t p_size) {
            wxMemoryInputStream stream(p_data, p_size);
            return wxBitmap(wxImage(stream, wxBITMAP_TYPE_PNG, -1), wxBITMAP_SCREEN_DEPTH);
        }

        wxBitmap loadImage(const char* p_path) {
            return wxBitmap(wxImage(wxString::FromUTF8(p_path)), wxBITMAP_SCREEN_DEPTH);
        }

    }; // namespace data

}; // namespace gw2b
