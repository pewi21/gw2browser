/* \file       ViewerGLCanvas.h
*  \brief      Contains declaration of the ViewerGLCanvas base class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2015 Khral Steelforge <https://github.com/kytulendu>
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

#ifndef VIEWERGLCANVAS_H_INCLUDED
#define VIEWERGLCANVAS_H_INCLUDED

namespace gw2b {
	class DatFile;
	class FileReader;
	class INeedDatFile;

	/** Panel used to view the contents of a file. */
	class ViewerGLCanvas : public wxGLCanvas {
		FileReader*     m_reader;
	public:
		/** Constructor. Creates the viewer with the given parent.
		*  \param[in]  p_parent Parent of the control.
		*  \param[in]  p_pos    Optional position of the control.
		*  \param[in]  p_size   Optional size of the control. */
		ViewerGLCanvas( wxWindow* p_parent, const int *p_attrib = ( const int* ) 0, const wxPoint& p_pos = wxDefaultPosition, const wxSize& p_size = wxDefaultSize, long p_style = 0L );
		/** Destructor. */
		virtual ~ViewerGLCanvas( );

		/** Clears all data in this viewer. */
		virtual void clear( );

		/** Sets the reader containing the data displayed by this viewer.
		*  \param[in]  p_reader     Reader to get data from. */
		virtual void setReader( FileReader* p_reader );
		/** Gets the reader containing the data displayed by this viewer.
		*  \return FileReader*     Reader containing the data. */
		FileReader* reader( ) {
			return m_reader;
		}
		/** Gets the reader containing the data displayed by this viewer.
		*  \return FileReader*     Reader containing the data. */
		const FileReader* reader( ) const {
			return m_reader;
		}
	}; // class Viewer

}; // namespace gw2b

#endif // VIEWERGLCANVAS_H_INCLUDED
