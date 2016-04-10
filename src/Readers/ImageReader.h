/* \file       Readers/ImageReader.h
*  \brief      Contains the declaration of the image reader class.
*  \author     Rhoot
*/

/*
Copyright (C) 2014-2016 Khral Steelforge <https://github.com/kytulendu>
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

#ifndef READERS_IMAGEREADER_H_INCLUDED
#define READERS_IMAGEREADER_H_INCLUDED

#include "FileReader.h"

#ifdef RGB
#  undef RGB    // GOD DAMN MICROSOFT WITH YOUR GOD DAMN MACROS
#endif

namespace gw2b {

	class ImageReader : public FileReader {
		union BGRA;
		union RGBA;
		struct BGR;
		struct RGB;
		union DXTColor;
		struct DXT1Block;
		struct DXT3Block;
		struct DCXBlock;
		struct DDSPixelFormat;
		struct DDSHeader;
		struct ANetAtexHeader;

	public:
		/** Constructor.
		*  \param[in]  p_data       Data to be handled by this reader.
		*  \param[in]  p_datFile    Reference to an instance of DatFile.
		*  \param[in]  p_fileType   File type of the given data. */
		ImageReader( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType );
		/** Destructor. Clears all data. */
		virtual ~ImageReader( );

		/** Gets the type of data contained in this file. Not to be confused with
		*  file type.
		*  \return DataType    type of data. */
		virtual DataType dataType( ) const override {
			return DT_Image;
		}
		/** Gets the image contained in the data owned by this reader.
		*  \return wxImage     Newly created image. */
		wxImage getImage( ) const;
		/** Determines whether the header of this image is valid.
		*  \return bool    true if valid, false if not. */
		static bool isValidHeader( const byte* p_data, size_t p_size );

	private:
		bool readDDS( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const;
		bool readATEX( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const;
		bool readWebP( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const;

		bool processLuminanceDDS( const DDSHeader* p_header, RGB*& po_colors ) const;
		bool processUncompressedDDS( const DDSHeader* p_header, RGB*& po_colors, uint8*& po_alphas ) const;

		void processDXTColor( BGR* p_colors, uint8* p_alphas, const DXTColor& p_blockColor, bool p_isDXT1 ) const;
		void processDXT1( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const;
		void processDXT1Block( BGR* p_colors, uint8* p_alphas, const DXT1Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
		void processDXT3( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const;
		void processDXT3Block( BGR* p_colors, uint8* p_alphas, const DXT3Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
		void processDXT5( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const;
		void processDXT5Block( BGR* p_colors, uint8* p_alphas, const DXT3Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
		void processDXTA( const uint64* p_data, uint p_width, uint p_height, BGR*& po_colors ) const;
		void processDXTABlock( BGR* p_colors, uint64 p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
		void process3DCX( const RGBA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const;
		void process3DCXBlock( RGB* p_colors, const DCXBlock& p_block, uint p_blockX, uint p_blockY, uint p_width ) const;
	}; // class ImageReader

}; // namespace gw2b

#endif // READERS_IMAGEREADER_H_INCLUDED
