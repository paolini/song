#include <stdlib.h>

typedef void freetype(void*);
typedef void* malloctype(size_t);

freetype * _imp__xmlFree=&free;
malloctype * _imp__xmlMalloc=&malloc;

