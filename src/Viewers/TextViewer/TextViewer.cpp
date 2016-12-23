/* \file       Viewers/TextViewer/TextViewer.cpp
*  \brief      Contains definition of the text viewer.
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

#include "TextViewer.h"

#include "FileReader.h"

namespace gw2b {

	TextViewer::TextViewer( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
		: Viewer( p_parent, p_pos, p_size ) {
		auto sizer = new wxBoxSizer( wxVERTICAL );

		// Toolbar
		auto toolbar = this->buildToolbar( );
		sizer->Add( toolbar, wxSizerFlags( ).Expand( ).FixedMinSize( ) );

		// Text control
		m_text = new wxTextCtrl( this, wxID_ANY, wxT( "" ), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );
		sizer->Add( m_text, wxSizerFlags( ).Expand( ).Proportion( 1 ) );

		// Layout
		this->SetSizer( sizer );
		this->Layout( );
	}

	TextViewer::~TextViewer( ) {
	}

	void TextViewer::clear( ) {
		// Clear text control content
		m_text->Clear( );
		// Clear text entry selector content
		m_textEntry->Clear( );

		m_string.clear( );
		Viewer::clear( );
	}

	void TextViewer::setReader( FileReader* p_reader ) {
		if ( isOfType<TextReader>( p_reader ) ) {
			Viewer::setReader( p_reader );

			if ( p_reader ) {
				auto reader = this->textReader( );
				auto str = reader->getString( );

				m_string.push_back( str );
			}

		} else if ( isOfType<EulaReader>( p_reader ) ) {
			Viewer::setReader( p_reader );

			if ( p_reader ) {
				auto reader = this->eulaReader( );
				auto eula = reader->getString( );

				m_string = eula;
			}
		} else {
			// Hopefully, there aren't things gones here.
			return;
		}

		for ( int i = 0; i < m_string.size( ); i++ ) {
			m_textEntry->AppendString( wxString::Format( wxT( "%d" ), i ) );
		}

		// Select entry 0
		m_textEntry->SetSelection( 0 );
		this->updateText( 0 );
	}

	void TextViewer::updateText( size_t p_entry ) {
		m_text->Clear( );
		m_text->SetValue( m_string[p_entry] );
	}

	wxToolBar* TextViewer::buildToolbar( ) {
		auto toolbar = new wxToolBar( this, wxID_ANY );
		m_textEntry = new wxChoice( toolbar, wxID_ANY );
		this->Bind( wxEVT_CHOICE, &TextViewer::onToolbarEntrySelectEvt, this );

		toolbar->Realize( );
		return toolbar;
	}

	void TextViewer::onToolbarEntrySelectEvt( wxCommandEvent& p_event ) {
		auto sel = p_event.GetSelection( );

		this->updateText( sel );
	}

}; // namespace gw2b
