#ifndef _PRINT_H_
#define _PRINT_H_

#include <iostream>

#include <libxml/tree.h>
#include "media.hh"
#include "layout.hh"

extern bool stanza_debug, verse_debug, song_debug, body_debug;

Box* SongBox(Media &m, xmlNodePtr p);

void PrintSongs(vector<xmlNodePtr> &,Media &);
#endif
