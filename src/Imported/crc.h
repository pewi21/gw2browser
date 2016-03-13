// File:    crc.h
// Author:  David Dantowitz
// Origin:  http://wiki.wxwidgets.org/Development:_Small_Table_CRC

#pragma once

#ifndef CRC_H_INCLUDED
#define CRC_H_INCLUDED

#define INITIAL_CRC 0xFFFFFFFF

uint32_t compute_crc( uint32_t old_crc, const char* s, size_t len );

#endif // CRC_H_INCLUDED
