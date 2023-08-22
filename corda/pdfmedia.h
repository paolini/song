#ifndef _pdfmedia_hh_
#define _pdfmedia_hh_
#include <hpdf.h>
#include "media.h"

// questa classe astrae il formato di stampa
// dovrebbe adattarsi a txt a PS e altro...

class PdfException: public std::exception {
  std::string msg;
public:
  PdfException(const std::string &m): msg(m) {};
  virtual ~PdfException() throw () {};
  virtual const char* what() const throw() {return msg.c_str();};
};

class PdfMedia: public VectorMedia {
protected:
  // dimensioni in centesimi di punto
  int x,y,chord_x;
  string filename;
  int page_no;
  HPDF_Doc doc;
  HPDF_Page page;
  //HPDF_Contents *canvas;
  HPDF_Font font_ptr[4];

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

  virtual ~PdfMedia();
  PdfMedia(const string &filename, int start_page=1);
  virtual void frame(int dx,int dy) const;
};



#endif
