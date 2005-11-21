#include "cursor.h"

Cursor::Cursor(Song &song) {
  if (song.body() && (*song.body()).size()
    && (*song.body())[0]->size() 
    && (*(*song.body())[0])[0]->size()) {
    item=(*(*(*song.body())[0])[0])[0];
    pos=0;
  } else item=0;
};
