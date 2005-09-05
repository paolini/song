#ifndef _TEXTMEDIA_HH_
#define _TEXTMEDIA_HH_

#include "media.h"

class TextMedia: public Media {
private:
  ostream *my_out;
  char *my_map;
  int x,y;
  int my_width,my_height;
  int chord_x;

protected:
  bool valid(int x, int y) const;
  char get(int x, int y) const;
  void set(int x, int y, char c);
  void clear();
  virtual void out() const;

public:
  virtual int wordWidth(const string &s, font f=NORMAL) const {return s.size();};
  virtual void goto_xy(int xx, int yy) {x=xx;y=yy;};
  virtual void move_xy(int xx, int yy) {x+=xx;y+=yy;};
  virtual int get_x() {return x;};
  virtual int get_y() {return y;};
  TextMedia(ostream &the_out, int width, int height);
  virtual int page_width() const {return my_width;};
  virtual int page_height() const {return my_height;};
  virtual ~TextMedia();
  virtual void wordWrite(const string &s, font f=NORMAL);
  virtual void chordWrite(const string &s);
  virtual void chordReset();
  virtual int lineSkip(font f=NORMAL) {return 1;};
  virtual void newPage();
};

#endif
