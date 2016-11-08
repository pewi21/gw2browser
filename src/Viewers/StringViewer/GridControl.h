/* \file       Viewers/StringViewer/GridControl.h
*  \brief      Contains declaration of the grid view control.
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

#pragma once

#ifndef VIEWERS_STRINGVIEWER_GRIDCONTROL_H_INCLUDED
#define VIEWERS_STRINGVIEWER_GRIDCONTROL_H_INCLUDED

#include <wx/scrolwin.h>
#include <wx/grid.h>

#include "Readers/StringReader.h"

namespace gw2b {

	class GridControl : public wxScrolledWindow {
		std::vector<StringStruct>	m_string;
		wxGrid*						m_grid;
	public:
		GridControl( wxWindow* p_parent, const wxPoint& p_position = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
		void setString( const std::vector<StringStruct> p_string );
	private:
		void updateGrid(  );
	};

};

#endif // VIEWERS_STRINGVIEWER_GRIDCONTROL_H_INCLUDED
