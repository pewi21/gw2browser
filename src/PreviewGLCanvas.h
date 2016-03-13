/* \file       PreviewGLCanvas.cpp
*  \brief      Contains declaration of the preview GLCanvas control.
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

#ifndef PREVIEWGLCANVAS_H_INCLUDED
#define PREVIEWGLCANVAS_H_INCLUDED

#include "FileReader.h"
#include "ViewerGLCanvas.h"

namespace gw2b {
	class DatFile;
	class DatIndexEntry;

	/** Panel control used to preview files from the .dat. */
	class PreviewGLCanvas : public wxGLCanvas {
		ViewerGLCanvas*			m_currentView;
		FileReader::DataType	m_currentDataType;
	public:
		/** Constructor. Creates the preview panel with the given parent.
		*  \param[in]  p_parent     Parent of the control.
		*  \param[in]  p_pos        Optional position of the control.
		*  \param[in]  p_size       Optional size of the control. */
		PreviewGLCanvas( wxWindow* p_parent, const int *p_attrib = ( const int* ) 0, const wxPoint& p_pos = wxDefaultPosition, const wxSize& p_size = wxDefaultSize, long p_style = 0L );
		/** Destructor. */
		~PreviewGLCanvas( );
		/** Tells this panel to preview a file.
		*  \param[in]  p_datFile    .dat file containing the file to preview.
		*  \param[in]  p_entry      Entry to preview.
		*  \return bool    true if successful, false if not. */
		bool previewFile( DatFile& p_datFile, const DatIndexEntry& p_entry );
	private:
		/** Helper method to create a viewer control to handle the given data type.
		*  The caller is responsible for freeing the viewer.
		*  \param[in]  p_dataType   Type of data to create a viewer for.
		*  \param[in]  p_datFile    Reference to an instance of DatFile.
		*  \return Viewer* Newly created viewer. */
		ViewerGLCanvas* createViewerForDataType( FileReader::DataType p_dataType, DatFile& p_datFile );
	}; // class PreviewGLCanvas

}; // namespace gw2b

#endif // PREVIEWGLCANVAS_H_INCLUDED
