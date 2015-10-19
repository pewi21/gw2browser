/* \file       INeedDatFile.h
*  \brief      Contains declaration of the viewer base class.
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

#ifndef INEEDDATFILE_H_INCLUDED
#define INEEDDATFILE_H_INCLUDED

namespace gw2b {
	class DatFile;

	class INeedDatFile {
		DatFile* m_datFile;
	protected:
		INeedDatFile( ) {
		}
	public:
		virtual ~INeedDatFile( ) {
		}
		// I know interfaces per definition aren't supposed to have implementation, but screw that. /rebelyell
		DatFile* datFile( ) {
			return m_datFile;
		}
		const DatFile* datFile( ) const {
			return m_datFile;
		}
		void setDatFile( DatFile* p_datFile ) {
			m_datFile = p_datFile;
		}
	};

}; // namespace gw2b

#endif // INEEDDATFILE_H_INCLUDED
