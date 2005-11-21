#ifndef _PRINT_H_
#define _PRINT_H_

#include <iostream>

#include "media.h"
#include "layout.h"
#include "song.h"
#include "cursor.h"

Box* SongBox(Media &m, Song* p);

void PrintSong(const Song *, Media &, const Cursor *cur=0);
void PrintSongs(const SongList &,Media &, const Cursor *cursor=0);
#endif
