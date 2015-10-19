/* \file       ViewerGLCanvas.cpp
*  \brief      Contains definition of the ViewerGLCanvas base class.
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

#include "stdafx.h"

#include "FileReader.h"

#include "ViewerGLCanvas.h"

namespace gw2b {

	ViewerGLCanvas::ViewerGLCanvas( wxWindow* p_parent, const wxPoint& p_pos, const wxSize& p_size )
		: wxGLCanvas( p_parent, wxID_ANY, nullptr, p_pos, p_size )
		, m_reader( nullptr ) {
	}

	ViewerGLCanvas::~ViewerGLCanvas( ) {
		deletePointer( m_reader );
	}

	void ViewerGLCanvas::clear( ) {
		deletePointer( m_reader );
	}

	void ViewerGLCanvas::setReader( FileReader* p_reader ) {
		this->clear( );
		m_reader = p_reader;
	}

}; // namespace gw2b
