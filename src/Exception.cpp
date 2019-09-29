/** \file       Exception.cpp
 *  \brief      Contains the declaration of the Exception class.
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

#include "stdafx.h"

#include "Exception.h"

namespace gw2b {
    namespace exception {

        Exception::Exception( const char* p_reason )
            : m_errmsg( p_reason ) {
        }

        const char *Exception::what( ) const throw( ) {
            return m_errmsg.c_str( );
        }

    }
}
