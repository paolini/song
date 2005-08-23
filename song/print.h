#ifndef _PRINT_H_
#define _PRINT_H_

#include <iostream>

#include <libxml/tree.h>
#include "media.h"
#include "layout.h"

Box* SongBox(Media &m, xmlNodePtr p);

void PrintSongs(const vector<xmlNodePtr> &,Media &);
#endif
