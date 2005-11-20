#ifndef _wxmedia_hh_
#define _wxmedia_hh_
#include "media.h"

#include "wx/wx.h"

// questa classe astrae il formato di stampa
// dovrebbe adattarsi a txt a PS e altro...

class wxMedia: public VectorMedia {
protected:
  // dimensioni in centesimi di punto
  int x,y,chord_x;
  string filename;
  int page_no;

  wxDC &dc;
  int h; // height in pixel
  wxColour cursorColor;
  wxColour fontColor[Media::NOFONT];
  wxFont fontFont[Media::NOFONT];
  int fontSize[Media::NOFONT];
public:
  
  virtual void goto_xy(int x, int y);
  virtual void move_xy(int x, int y);
  virtual int get_x();
  virtual int get_y();

  virtual int wordWidth(const string &s, font f) const; 
  virtual void wordWrite(const string &s, font f); 
  virtual void chordWrite(const string &s);
  virtual void chordReset(); //nuova riga di accordi
  virtual void cursorWrite(font f);

  virtual int lineSkip(font f); 

  virtual void newPage();
  void closePage();

  virtual ~wxMedia()  ;
  wxMedia(wxDC &canvas, int height, int start_page=1);
  virtual void frame(int dx,int dy) const;
};



#endif
