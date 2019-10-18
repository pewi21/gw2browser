/** \file       Viewers/TextViewer/TextViewer.h
 *  \brief      Contains declaration of the text viewer.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2016 Khralkatorrix <https://github.com/kytulendu>
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

#ifndef VIEWERS_TEXTVIEWER_TEXTVIEWER_H_INCLUDED
#define VIEWERS_TEXTVIEWER_TEXTVIEWER_H_INCLUDED

#include <vector>
#include <wx/grid.h>

#include "Viewer.h"

#include "Readers/EulaReader.h"
#include "Readers/TextReader.h"

namespace gw2b {

    class TextViewer : public Viewer {
        std::vector<wxString>       m_string;
        wxTextCtrl*                 m_text;
        wxChoice*                   m_textEntry;
    public:
        /** Constructor. Creates the model viewer with the given parent.
        *  \param[in]  p_parent     Parent of the control.
        *  \param[in]  p_pos        Optional position of the control.
        *  \param[in]  p_size       Optional size of the control. */
        TextViewer( wxWindow* p_parent, const wxPoint& p_pos = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
        /** Destructor. */
        virtual ~TextViewer( );

        /** Clear the viewer. */
        virtual void clear( ) override;
        virtual void setReader( FileReader* p_reader ) override;

        /** Gets the string reader containing the data displayed by this viewer.
        *  \return TextReader*    Reader containing the data. */
        TextReader* textReader( ) {
            return reinterpret_cast<TextReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast
        /** Gets the string reader containing the data displayed by this viewer.
        *  \return TextReader*    Reader containing the data. */
        const TextReader* textReader( ) const {
            return reinterpret_cast<const TextReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast

        /** Gets the eula reader containing the data displayed by this viewer.
        *  \return EulaReader*    Reader containing the data. */
        EulaReader* eulaReader( ) {
            return reinterpret_cast<EulaReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast
        /** Gets the eula reader containing the data displayed by this viewer.
        *  \return EulaReader*    Reader containing the data. */
        const EulaReader* eulaReader( ) const {
            return reinterpret_cast<const EulaReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast

    private:
        void updateText( size_t p_entry );
        wxToolBar* buildToolbar( );
        void onToolbarEntrySelectEvt( wxCommandEvent& p_event );
    }; // class TextViewer

}; // namespace gw2b

#endif // VIEWERS_TEXTVIEWER_TEXTVIEWER_H_INCLUDED
