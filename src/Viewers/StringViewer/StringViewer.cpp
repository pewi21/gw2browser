/* \file       Viewers/StringViewer/StringViewer.cpp
*  \brief      Contains definition of the string viewer.
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

#include "StringViewer.h"

#include "FileReader.h"

namespace gw2b {

    StringViewer::StringViewer( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
        : Viewer( p_parent, p_pos, p_size ) {
        auto sizer = new wxBoxSizer( wxHORIZONTAL );

        // Grid control
        m_grid = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize );
        // Create grid with 0 row, 2 cols
        m_grid->CreateGrid( 0, 2 );
        sizer->Add( m_grid, wxSizerFlags( ).Expand( ).Proportion( 1 ) );

        // Layout
        this->SetSizer( sizer );
        this->Layout( );
    }

    StringViewer::~StringViewer( ) {
    }

    void StringViewer::clear( ) {
        // Clear grid content
        m_grid->ClearGrid( );

        if ( m_grid->GetNumberRows( ) ) {
            m_grid->DeleteRows( 0, m_grid->GetNumberRows( ) );
        }

        m_string.clear( );
        Viewer::clear( );
    }

    void StringViewer::setReader( FileReader* p_reader ) {
        Ensure::isOfType<StringReader>( p_reader );
        Viewer::setReader( p_reader );

        if ( p_reader ) {
            auto reader = this->stringReader( );
            m_string = reader->getString( );

            this->updateGrid( );
        }
    }

    void StringViewer::updateGrid( ) {
        auto numstring = m_string.size( );

        if ( numstring ) {
            m_grid->SetColLabelValue( 0, wxT( "Entry" ) );
            m_grid->SetColLabelValue( 1, wxT( "String" ) );

            m_grid->AppendRows( numstring );

            for ( uint n = 0; n < numstring; n++ ) {
                m_grid->SetCellValue( n, 0, wxString::Format( wxT( "%i" ), m_string[n].id ) );
                m_grid->SetCellValue( n, 1, m_string[n].string );
            }
            m_grid->AutoSize( );
        }
        this->Layout( );
    }

}; // namespace gw2b
