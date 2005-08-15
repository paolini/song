#include "media.h"

// normal, refrain, chord, title, author, nofont

//enum font {NORMAL=0, REFRAIN,CHORD, TITLE, AUTHOR, NOFONT};
const int VectorMedia::font_size[]={10,10,8,20,12,0};
const int VectorMedia::font_font[]={ 0, 1, 0, 2, 2,0}; // roman, italic, bold



VectorMedia::VectorMedia() {
  static const int a4x=594, a4y=842;
  left=30;
  bottom=60;
  top=30;
  my_width=100*(a4x-2*left);
  my_height=100*(a4y-bottom-top);
  
  stanza_sep=font_size[NORMAL]*100;
  column_sep=3*stanza_sep;
  body_sep=font_size[AUTHOR]*30;
  song_sep=4*stanza_sep;
  
  chord_sep=font_size[CHORD]*30; //40%
};
