/** \file       Data.cpp
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

#include "stdafx.h"

#include <wx/mstream.h>
#include <wx/stdpaths.h>

#include "Data.h"

namespace gw2b {

    wxBitmap loadPNG(const byte* p_data, size_t p_size)
    {
        wxMemoryInputStream stream(p_data, p_size);
        return wxBitmap(wxImage(stream, wxBITMAP_TYPE_PNG, -1), wxBITMAP_SCREEN_DEPTH);
    }

    wxBitmap loadImage(wxString p_path)
    {
        return wxBitmap(wxImage(p_path), wxBITMAP_SCREEN_DEPTH);
    }

    wxString getPath(const char* p_filePath)
    {
#ifdef _WIN32
        wxString dataPath(wxT("../data"));
#else
        wxStandardPathsBase& stdp = wxStandardPaths::Get( );
        auto dataPath = stdp.GetDataDir( );
#endif
        dataPath << wxT("/") << wxString::FromUTF8(p_filePath);
        return dataPath;
    }

}; // namespace gw2b
