/* \file       Viewers/ImageViewer/ImageControl.h
*  \brief      Contains declaration of the image control.
*  \author     Rhoot
*/

/*
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

#pragma once

#ifndef VIEWERS_IMAGEVIEWER_IMAGECONTROL_H_INCLUDED
#define VIEWERS_IMAGEVIEWER_IMAGECONTROL_H_INCLUDED

#include <wx/scrolwin.h>

namespace gw2b {

	class ImageControl : public wxScrolledWindow {
	public:
		enum ImageChannels {
			IC_None = 0,
			IC_Red = 1,
			IC_Green = 2,
			IC_Blue = 4,
			IC_Alpha = 8,
			IC_All = 15,
		};
	private:
		wxImage         m_image;
		wxBitmap        m_bitmap;
		wxBitmap        m_backdrop;
		ImageChannels   m_channels;
	public:
		ImageControl( wxWindow* p_parent, const wxPoint& p_position = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
		virtual ~ImageControl( );
		void SetImage( wxImage p_image );
		void OnDraw( wxDC& p_DC, wxRect& p_region );
		void ToggleChannel( ImageChannels p_channel, bool p_toggled );
	private:
		void UpdateBitmap( );
		void UpdateScrollbars( wxDC& p_DC );
		void OnPaintEvt( wxPaintEvent& p_event );
	};

}; // namespace gw2b

#endif // VIEWERS_IMAGEVIEWER_IMAGECONTROL_H_INCLUDED
