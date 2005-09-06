#ifndef _PSMEDIA_HH_
#define _PSMEDIA_HH_
#include "media.h"

class PsMedia: public VectorMedia {
  // tutte le dimensioni in centesimi di punto
protected:
  //  int my_width,my_height;
  int x,y;
  int chord_x;
  ostream *out;
  font font_type;

  int page;

  void set_font(font f);
  int char_width(char x) const;
  void char_out(char x) const;
  
public:

  virtual int wordWidth(const string &s, font f) const;
  virtual void goto_xy(int xx, int yy);
  virtual void move_xy(int xx, int yy);
  virtual int get_x() {return x;};
  virtual int get_y() {return y;};
  PsMedia(ostream &out, int start_page=1);
  //  virtual int page_width() const {return my_width;};
  //  virtual int page_height() const {return my_height;};
  virtual ~PsMedia();
  virtual void wordWrite(const string &s, font f);
  virtual void chordWrite(const string &s);
  virtual void chordReset();
  virtual int lineSkip(font f);
  virtual void newPage();
  virtual void pageStart();
  virtual void pageEnd();
  virtual void frame(int dx, int dy) const;

};

#endif
