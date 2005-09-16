#include <iostream>
#include "textmedia.h"
#include "iso.h"

bool TextMedia::valid(int x, int y) const {
    return x>=0 && x<my_width && y>=0 && y<my_height;};

char TextMedia::get(int x, int y) const {
  if (valid(x,y)) return my_map[y*my_width+x];
  else return 0;
};

void TextMedia::set(int x, int y, char c) {
  if (valid(x,y)) {
    if (get(x,y)!=' ') c='\\';
    my_map[y*my_width+x]=c;
  }
};

void TextMedia::clear() {
  memset(my_map,' ',my_width*my_height);
  chordReset();
  x=0;
  y=0;
};

void TextMedia::newPage() {
  out();
  (*my_out)<<"[NEWPAGE]\n";
  clear();
};

void TextMedia::out() const {
  ostream &o(*my_out);
  int k;
  for (k=my_height-1;k>=0;--k) {
    int i;
    for (i=0;i<my_width && isspace(get(i,my_height-k-1));++i);
    if (i<my_width) break;
  }

  bool beginning=true;
  for (int i=0;i<=k;++i) {
    int j;
    for (j=my_width-1;j>=0 && isspace(get(j,my_height-i-1));--j); 
    if (beginning && j==-1) continue;
    beginning=false;
    o.write(my_map+(my_height-i-1)*my_width,j+1);
    o<<"\n";
  }
}

TextMedia::TextMedia(ostream &the_out, int width, int height): 
  Media(true),
  my_out(&the_out), x(0), y(0),
  my_width(width), my_height(height)
{
  stanza_sep=1;
  column_sep=2;
  body_sep=1;
  song_sep=2;

  my_map=new char[my_width*my_height];
  clear();
};

TextMedia::~TextMedia() {
  out();
  delete[] my_map;
};

int TextMedia::wordWidth(const string &s, font f) const {
  size_t i;
  int n=0;
  for (i=0;i<s.size();) {
    iso(s,i);
    n++;
    }
  return n;
}

void TextMedia::wordWrite(const string &ss, font ) {
  string s=iso(ss);
  for (unsigned int i=0;i<s.size();++i)
    set(x++,y,s[i]);
};

void TextMedia::chordWrite(const string &s) {
  int savex=x, savey=y;
  y++;
  //cout<<"writing chord "<<s<<" x,y="<<x<<","<<y<<" chord_x="<<chord_x<<"\n";
  if (x<chord_x) x=chord_x;
  wordWrite(s);
  chord_x=x+1;
  x=savex, y=savey;
};

void TextMedia::chordReset() {
  chord_x=-1;
};

