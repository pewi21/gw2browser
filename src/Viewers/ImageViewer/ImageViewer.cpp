/** \file       Viewers/ImageViewer/ImageViewer.cpp
 *  \brief      Contains definition of the image viewer.
 *  \author     Rhoot
 */

/**
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

#include "stdafx.h"

#include "ImageViewer.h"

#include "ImageControl.h"
#include "Readers/ImageReader.h"
#include "Data.h"

namespace gw2b {

    ImageViewer::ImageViewer( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
        : Viewer( p_parent, p_pos, p_size )
        , m_imageControl( nullptr ) {
        auto sizer = new wxBoxSizer( wxVERTICAL );

        // Toolbar
        auto toolbar = this->buildToolbar( );
        sizer->Add( toolbar, wxSizerFlags( ).Expand( ).FixedMinSize( ) );

        // Image control
        m_imageControl = new ImageControl( this );
        sizer->Add( m_imageControl, wxSizerFlags( ).Expand( ).Proportion( 1 ) );

        // Layout
        this->SetSizer( sizer );
        this->Layout( );
    }

    ImageViewer::~ImageViewer( ) {
    }

    void ImageViewer::clear( ) {
        m_imageControl->SetImage( wxImage( ) );
        Viewer::clear( );
    }

    void ImageViewer::setReader( FileReader* p_reader ) {
        Ensure::isOfType<ImageReader>( p_reader );
        Viewer::setReader( p_reader );

        if ( p_reader ) {
            m_image = imageReader( )->getImage( );
            m_imageControl->SetImage( m_image );
        }
    }

    wxPanel* ImageViewer::buildToolbar( ) {
        auto toolbar = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( 170, 40 ), wxBORDER_SIMPLE );
        auto flex = new wxFlexGridSizer( 1, 4, 0, 0 );
        auto id = this->NewControlId( 4 );

        // Add the newly generated IDs
        for ( uint i = 0; i < 4; i++ ) {
            m_toolbarButtonIds.Add( id++ );
        }

        // Load all toolbar button icons
        m_toolbarButtonIcons.push_back(data::loadImage("../data/interface/toggle_red.png"));
        m_toolbarButtonIcons.push_back(data::loadImage("../data/interface/toggle_green.png"));
        m_toolbarButtonIcons.push_back(data::loadImage("../data/interface/toggle_blue.png"));
        m_toolbarButtonIcons.push_back(data::loadImage("../data/interface/toggle_alpha.png"));

        // Toggle channel buttons
        for ( uint i = 0; i < 4; i++ ) {
            wxToggleButton* button = new wxToggleButton( toolbar, m_toolbarButtonIds[i], wxEmptyString, wxDefaultPosition, wxSize( 25, 25 ) );
            button->SetBitmap( m_toolbarButtonIcons[i] );
            button->SetValue( true );
            flex->Add( button, 1, wxALL | wxALIGN_CENTRE, 5 );
            m_toolbarButtons.Add( button );
            this->Bind( wxEVT_TOGGLEBUTTON, &ImageViewer::onToolbarClickedEvt, this, m_toolbarButtonIds[i] );
        }

        toolbar->SetSizer( flex );

        return toolbar;
    }

    void ImageViewer::onToolbarClickedEvt( wxCommandEvent& p_event ) {
        auto id = p_event.GetId( );

        // Toggle red
        if ( id == m_toolbarButtonIds[0] ) {
            m_imageControl->ToggleChannel( ImageControl::IC_Red, m_toolbarButtons[0]->GetValue( ) );
            // Toggle green
        } else if ( id == m_toolbarButtonIds[1] ) {
            m_imageControl->ToggleChannel( ImageControl::IC_Green, m_toolbarButtons[1]->GetValue( ) );
            // Toggle blue
        } else if ( id == m_toolbarButtonIds[2] ) {
            m_imageControl->ToggleChannel( ImageControl::IC_Blue, m_toolbarButtons[2]->GetValue( ) );
            // Toggle alpha
        } else if ( id == m_toolbarButtonIds[3] ) {
            m_imageControl->ToggleChannel( ImageControl::IC_Alpha, m_toolbarButtons[3]->GetValue( ) );
        } else {
            p_event.Skip( );
        }
    }

}; // namespace gw2b
