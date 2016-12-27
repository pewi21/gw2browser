#include <vorbis/vorbisfile.h>

struct ogg_file {
	char* curPtr;
	char* filePtr;
	size_t fileSize;
};

size_t readOgg( void* dst, size_t size1, size_t size2, void* fh );
int seekOgg( void *fh, ogg_int64_t to, int type );
int closeOgg( void* fh );
long tellOgg( void *fh );
