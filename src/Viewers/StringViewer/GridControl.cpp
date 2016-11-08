/* \file       Viewers/StringViewer/GridControl.cpp
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

#include "stdafx.h"
//#include <wx/dcbuffer.h>

#include "GridControl.h"

namespace gw2b {

	GridControl::GridControl( wxWindow* p_parent, const wxPoint& p_position, const wxSize& p_size )
		: wxScrolledWindow( p_parent, wxID_ANY, p_position, p_size, wxBORDER_THEME ) {
		m_grid = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize );
		// Create grid with 0 row, 2 cols
		m_grid->CreateGrid( 0, 2 );

		auto sizer = new wxBoxSizer( wxHORIZONTAL );
		sizer->Add( m_grid, 1, wxEXPAND );

		this->SetSizer( sizer );

		// This part makes the scrollbars show up
		this->FitInside( ); // Ask the sizer about the needed size
		this->SetScrollRate( 5, 5 );
	}

	void GridControl::setString( const std::vector<StringStruct> p_string ) {
		m_string = p_string;
		this->updateGrid( );
	}

	void GridControl::updateGrid( ) {
		auto numstring = m_string.size( );

		// Clear grid content
		m_grid->ClearGrid( );

		if ( m_grid->GetNumberRows( ) ) {
			m_grid->DeleteRows( 0, m_grid->GetNumberRows( ) );
		}

		if ( numstring ) {
			m_grid->SetColLabelValue( 0, wxT( "Entry" ) );
			m_grid->SetColLabelValue( 1, wxT( "String" ) );

			for ( int n = 0; n < m_string.size( ); n++ ) {
				m_grid->AppendRows( 1 );

				m_grid->SetCellValue( n, 0, wxString::Format( wxT( "%i" ), m_string[n].id ) );
				m_grid->SetCellValue( n, 1, m_string[n].string );
			}

			m_grid->AutoSize( );
		}
	}

}; // namespace gw2b
