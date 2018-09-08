/** Read Oggs from memory using custom callbacks
*   by DubyaDubyaDubyaDot <https://stackoverflow.com/users/1134276/dubyadubyadubyadot>
*   https://stackoverflow.com/questions/13437422/libvorbis-audio-decode-from-memory-in-c
*
*   Usuage
*   ======
*   ov_callbacks callbacks;
*   ogg_file t;
*   t.curPtr = t.filePtr = compressedData;
*   t.fileSize = compressedDataSize;
*
*   OggVorbis_File* ov = new OggVorbis_File;
*   mOggFile = ov;
*   memset( ov, 0, sizeof( OggVorbis_File ) );
*
*   callbacks.read_func = AR_readOgg;
*   callbacks.seek_func = AR_seekOgg;
*   callbacks.close_func = AR_closeOgg;
*   callbacks.tell_func = AR_tellOgg;
*
*   int ret = ov_open_callbacks((void *)&t, ov, NULL, -1, callbacks);
*
*   vorbis_info* vi = ov_info(ov, -1);
*   assert(vi);
*
*   compressed data is available to use, to uncompress look into ov_read
*/

#include "stdafx.h"

#include <stdlib.h>

#include "OggCallback.h"

size_t readOgg( void* dst, size_t size1, size_t size2, void* fh ) {
    ogg_file* of = reinterpret_cast<ogg_file*>( fh );
    size_t len = size1 * size2;
    if ( of->curPtr + len > of->filePtr + of->fileSize ) {
        len = of->filePtr + of->fileSize - of->curPtr;
    }
    memcpy( dst, of->curPtr, len );
    of->curPtr += len;
    return len;
}

int seekOgg( void *fh, ogg_int64_t to, int type ) {
    ogg_file* of = reinterpret_cast<ogg_file*>( fh );

    switch ( type ) {
    case SEEK_CUR:
        of->curPtr += to;
        break;
    case SEEK_END:
        of->curPtr = of->filePtr + of->fileSize - to;
        break;
    case SEEK_SET:
        of->curPtr = of->filePtr + to;
        break;
    default:
        return -1;
    }
    if ( of->curPtr < of->filePtr ) {
        of->curPtr = of->filePtr;
        return -1;
    }
    if ( of->curPtr > of->filePtr + of->fileSize ) {
        of->curPtr = of->filePtr + of->fileSize;
        return -1;
    }
    return 0;
}

int closeOgg( void* fh ) {
    return 0;
}

long tellOgg( void *fh ) {
    ogg_file* of = reinterpret_cast<ogg_file*>( fh );
    return ( of->curPtr - of->filePtr );
}
