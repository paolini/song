#ifndef _pdfmedia_hh_
#define _pdfmedia_hh_
#include <libharu.h>
#include "media.h"

// questa classe astrae il formato di stampa
// dovrebbe adattarsi a txt a PS e altro...

class PdfMedia: public VectorMedia {
protected:
  // dimensioni in centesimi di punto
  int x,y,chord_x;
  string filename;
  int page_no;
  PdfDoc* doc;
  PdfPage *page;
  PdfContents *canvas;


public:
  
  virtual void goto_xy(int x, int y);
  virtual void move_xy(int x, int y);
  virtual int get_x();
  virtual int get_y();

  virtual int wordWidth(const string &s, font f) const; 
  virtual void wordWrite(const string &s, font f); 
  virtual void chordWrite(const string &s);
  virtual void chordReset(); //nuova riga di accordi

  virtual int lineSkip(font f); 

  virtual void newPage();
  void closePage();

  virtual ~PdfMedia()  ;
  PdfMedia(const string &filename, int start_page=1);
  virtual void frame(int dx,int dy) const;
};



#endif
