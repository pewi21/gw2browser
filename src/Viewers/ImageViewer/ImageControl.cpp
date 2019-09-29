/** \file       Viewers/ImageViewer/ImageControl.cpp
 *  \brief      Contains definition of the image control.
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
#include <wx/dcbuffer.h>

#include "Data.h"
#include "ImageControl.h"

namespace gw2b {

    ImageControl::ImageControl( wxWindow* p_parent, const wxPoint& p_position, const wxSize& p_size )
        : wxScrolledWindow( p_parent, wxID_ANY, p_position, p_size )
        , m_channels( IC_All ) {
        m_backdrop = data::loadImage("../data/interface/checkers.png");
        this->SetBackgroundStyle( wxBG_STYLE_CUSTOM );
        this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
        this->Bind( wxEVT_PAINT, &ImageControl::OnPaintEvt, this );
    }

    ImageControl::~ImageControl( ) {
    }

    void ImageControl::SetImage( wxImage p_image ) {
        m_image = p_image;
        this->UpdateBitmap( );
    }

    void ImageControl::UpdateBitmap( ) {
        m_bitmap = wxBitmap( );

        if ( m_image.IsOk( ) ) {
            m_bitmap.Create( m_image.GetWidth( ), m_image.GetHeight( ) );
            wxMemoryDC dc( m_bitmap );

            // New image so we don't mess with the actual image
            wxImage image( m_image );
            bool imageHasAlpha = image.HasAlpha( );

            // Skip backdrop if image has no alpha, or if it's not visible
            if ( imageHasAlpha && !!( m_channels & IC_Alpha ) ) {
                for ( uint y = 0; y < ( uint ) m_bitmap.GetHeight( ); y += m_backdrop.GetHeight( ) ) {
                    for ( uint x = 0; x < ( uint ) m_bitmap.GetWidth( ); x += m_backdrop.GetWidth( ) ) {
                        dc.DrawBitmap( m_backdrop, x, y );
                    }
                }
            }

            // Check if any channels have been toggled off
            if ( m_channels != IC_All ) {
                uint numPixels = image.GetWidth( ) * image.GetHeight( );
                uint8* alphaCache = nullptr;

                // Any colors toggled off?
                if ( !( m_channels & IC_Red ) || !( m_channels & IC_Green ) || !( m_channels & IC_Blue ) ) {
                    uint8* colors = allocate<uint8>( numPixels * 3 );
                    ::memcpy( colors, image.GetData( ), numPixels * 3 );

                    // Setting colors clears the alpha, so cache it for restoration later,
                    // if the image has an alpha channel and it's visible
                    if ( imageHasAlpha && ( m_channels & IC_Alpha ) ) {
                        alphaCache = allocate<uint8>( numPixels );
                        ::memcpy( alphaCache, image.GetAlpha( ), numPixels );
                    }

                    // If all colors are off, but alpha is on, alpha should be made white
                    bool noColors = !( ( m_channels & IC_Red ) || ( m_channels & IC_Green ) || ( m_channels & IC_Blue ) );
                    bool whiteAlpha = noColors && !!( m_channels & IC_Alpha );

                    // Loop through the pixels
                    for ( uint i = 0; i < numPixels; i++ ) {
                        if ( whiteAlpha ) {
                            if ( alphaCache ) {
                                ::memset( &colors[i * 3], alphaCache[i], sizeof( uint8 ) * 3 );
                            } else {
                                ::memset( &colors[i * 3], 0xff, sizeof( uint8 ) * 3 );
                            }
                        } else {
                            // Red turned off?
                            if ( !( m_channels & IC_Red ) ) {
                                colors[i * 3 + 0] = 0x00;
                            }
                            // Green turned off?
                            if ( !( m_channels & IC_Green ) ) {
                                colors[i * 3 + 1] = 0x00;
                            }
                            // Blue turned off?
                            if ( !( m_channels & IC_Blue ) ) {
                                colors[i * 3 + 2] = 0x00;
                            }
                        }
                    }

                    // If alpha should be white, it should not be alpha too
                    if ( whiteAlpha ) {
                        freePointer( alphaCache );
                    }

                    // Update colors
                    image.SetData( colors );
                }

                // Was alpha turned off?
                if ( imageHasAlpha && !( m_channels & IC_Alpha ) ) {
                    uint8* alpha = allocate<uint8>( numPixels );
                    ::memset( alpha, 0xff, numPixels );
                    image.SetAlpha( alpha );
                } else if ( alphaCache ) {
                    image.SetAlpha( alphaCache );
                }
            }

            // Draw the image
            dc.DrawBitmap( wxBitmap( image ), 0, 0 );
        }

        this->Refresh( );
    }

    void ImageControl::ToggleChannel( ImageChannels p_channel, bool p_toggled ) {
        bool isDirty = false;

        // Red
        if ( p_channel == IC_Red && !!( m_channels & IC_Red ) != p_toggled ) {
            m_channels = ( ImageChannels ) ( p_toggled ? ( m_channels | IC_Red ) : ( m_channels & ~IC_Red ) );
            isDirty = true;

            // Green
        } else if ( p_channel == IC_Green && !!( m_channels & IC_Green ) != p_toggled ) {
            m_channels = ( ImageChannels ) ( p_toggled ? ( m_channels | IC_Green ) : ( m_channels & ~IC_Green ) );
            isDirty = true;

            // Blue
        } else if ( p_channel == IC_Blue && !!( m_channels & IC_Blue ) != p_toggled ) {
            m_channels = ( ImageChannels ) ( p_toggled ? ( m_channels | IC_Blue ) : ( m_channels & ~IC_Blue ) );
            isDirty = true;

            // Alpha
        } else if ( p_channel == IC_Alpha && !!( m_channels & IC_Alpha ) != p_toggled ) {
            m_channels = ( ImageChannels ) ( p_toggled ? ( m_channels | IC_Alpha ) : ( m_channels & ~IC_Alpha ) );
            isDirty = true;
        }

        if ( isDirty ) {
            this->UpdateBitmap( );
        }
    }

    void ImageControl::OnDraw( wxDC& p_DC, wxRect& p_region ) {
        this->UpdateScrollbars( p_DC );
        if ( m_bitmap.IsOk( ) ) {
            p_DC.DrawBitmap( m_bitmap, 0, 0 );
        }
    }

    void ImageControl::UpdateScrollbars( wxDC& p_DC ) {
        if ( m_bitmap.IsOk( ) ) {
            this->SetVirtualSize( m_bitmap.GetWidth( ), m_bitmap.GetHeight( ) );
            this->SetScrollRate( 0x20, 0x20 );
        } else {
            this->SetVirtualSize( 0, 0 );
        }
    }

    void ImageControl::OnPaintEvt( wxPaintEvent& p_event ) {
        wxAutoBufferedPaintDC dc( this );
        dc.Clear( );

        int vX, vY, pX, pY;
        this->GetViewStart( &vX, &vY );
        this->GetScrollPixelsPerUnit( &pX, &pY );
        vX *= pX;
        vY *= pY;

        wxPoint pt = dc.GetDeviceOrigin( );
        dc.SetDeviceOrigin( pt.x - vX, pt.y - vY );
        dc.SetUserScale( this->GetScaleX( ), this->GetScaleY( ) );

        for ( wxRegionIterator upd( this->GetUpdateRegion( ) ); upd; upd++ ) {
            wxRect updRect( upd.GetRect( ) );
            updRect.x += vX;
            updRect.y += vY;
            this->OnDraw( dc, updRect );
        }
    }

}; // namespace gw2b
