/** \file       Viewers/StringViewer/StringViewer.h
 *  \brief      Contains declaration of the string viewer.
 *  \author     Khral Steelforge
 */

/**
 * Copyright (C) 2016 Khral Steelforge <https://github.com/kytulendu>
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

#pragma once

#ifndef VIEWERS_STRINGVIEWER_STRINGVIEWER_H_INCLUDED
#define VIEWERS_STRINGVIEWER_STRINGVIEWER_H_INCLUDED

#include <wx/grid.h>

#include "Viewer.h"

#include "Readers/StringReader.h"

namespace gw2b {

    class StringViewer : public Viewer {
        std::vector<StringStruct>   m_string;
        wxGrid*                     m_grid;
    public:
        /** Constructor. Creates the model viewer with the given parent.
        *  \param[in]  p_parent     Parent of the control.
        *  \param[in]  p_pos        Optional position of the control.
        *  \param[in]  p_size       Optional size of the control. */
        StringViewer( wxWindow* p_parent, const wxPoint& p_pos = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
        /** Destructor. */
        virtual ~StringViewer( );

        /** Clear the viewer. */
        virtual void clear( ) override;
        virtual void setReader( FileReader* p_reader ) override;
        /** Gets the string reader containing the data displayed by this viewer.
        *  \return StringReader*    Reader containing the data. */
        StringReader* stringReader( ) {
            return reinterpret_cast<StringReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast
        /** Gets the string reader containing the data displayed by this viewer.
        *  \return StringReader*    Reader containing the data. */
        const StringReader* stringReader( ) const {
            return reinterpret_cast<const StringReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast

    private:
        void updateGrid( );

    }; // class StringViewer

}; // namespace gw2b

#endif // VIEWERS_STRINGVIEWER_STRINGVIEWER_H_INCLUDED
