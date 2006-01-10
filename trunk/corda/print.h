#ifndef _PRINT_H_
#define _PRINT_H_

#include <iostream>

#include "media.h"
#include "layout.h"
#include "song.h"
#include "cursor.h"
#include "plug.h"

Box* SongBox(Media &m, Song* p, const PlugoutOptions *options);

void PrintSong(const Song *, Media &, 
	       const PlugoutOptions *options,
	       const Cursor *cur=0);
void PrintSongs(const SongArray &,Media &, 
		const PlugoutOptions *options,
		const Cursor *cursor=0);
#endif
