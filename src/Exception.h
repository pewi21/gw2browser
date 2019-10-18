/* \file       Exception.h
*  \brief      Contains declaration of the Exception class.
*  \author     Khralkatorrix
*/

/*
Copyright (C) 2016 Khralkatorrix <https://github.com/kytulendu>

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

#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED

#include <exception>
#include <string>

namespace gw2b {
    namespace exception {

        class Exception : public std::exception {
        private:
            std::string m_errmsg;
        public:
            /** Constructor. */
            explicit Exception( const char* p_reason );
            /** Overriden what() method from exception class. */
            virtual const char *what( ) const throw( );
        };

    }
}

#endif // EXCEPTION_H_INCLUDED
