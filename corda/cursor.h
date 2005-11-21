#ifndef _MYCURSOR_H_
#define _MYCURSOR_H_

#include "song.h"

class Cursor {
 public:
  PhraseItem *item;
  int pos;
 public:
  Cursor(Song &song);
};

#endif
