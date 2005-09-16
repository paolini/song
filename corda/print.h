#ifndef _PRINT_H_
#define _PRINT_H_

#include <iostream>

#include "media.h"
#include "layout.h"

Box* SongBox(Media &m, Song* p);

void PrintSongs(const SongList &,Media &);
#endif
