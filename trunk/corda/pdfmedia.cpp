#include <fstream>
#include <cassert>
#include <libharu.h>
#include <libharu_ISO8859.h>
#include <iostream>
#include <sstream>
#include "pdfmedia.h"

#include "iso.h"

static const char *font_name[4]={"FR","FI","FB","FC"};
// roman, italic, bold

PdfMedia::PdfMedia(const string &name, int start_page): filename(name) {
  doc=new PdfDoc();
  doc->NewDoc();
  PdfEncodingDef* encoding=new PdfEncoding_ISO8859_3();
//   doc->AddType1Font(new PdfHelveticaFontDef(),font_name[0],encoding);
//   doc->AddType1Font(new PdfHelveticaObliqueFontDef(),font_name[1],encoding);
//   doc->AddType1Font(new PdfHelveticaBoldFontDef(),font_name[2],encoding);

  doc->AddType1Font(new PdfTimesRomanFontDef(),font_name[0],encoding);
  doc->AddType1Font(new PdfTimesItalicFontDef(),font_name[1],encoding);
  doc->AddType1Font(new PdfTimesBoldFontDef(),font_name[2],encoding);
  doc->AddType1Font(new PdfCourierFontDef(),font_name[3],encoding);
  
  page=0;
  canvas=0;
  page_no=start_page-1;
  newPage();
};

PdfMedia::~PdfMedia() {
  doc->WriteToFile(filename.c_str());
  doc->FreeDoc();
  delete doc;
};

void PdfMedia::newPage() {
  if (page) closePage();
  page=doc->AddPage();
  page_no++;
  canvas=page->Canvas();
  x=-1;
  y=-1;
  stringstream s;
  s<<page_no;
  goto_xy((my_width-wordWidth(s.str().c_str(),AUTHOR))/2,
	  -100*(font_size[NORMAL]+font_size[AUTHOR]));
  wordWrite(s.str().c_str(),AUTHOR);
  goto_xy(0,0);
};

void PdfMedia::goto_xy(int xx,int yy) {
  assert(canvas);
  if (x!=xx || y!=yy) {
    x=xx; y=yy;
  };
};

void PdfMedia::move_xy(int xx,int yy) {
  goto_xy(x+xx,y+yy);
};

int PdfMedia::get_x() {return x;};

int PdfMedia::get_y() {return y;};

int PdfMedia::wordWidth(const string &s, font f) const {
  string name;
  canvas->SetFontAndSize(font_name[font_font[f]], 
			 font_size[f]);
  return int(100.0*canvas->TextWidth(iso(s).c_str()));
};

void PdfMedia::wordWrite(const string &s, font f) {
  //  cerr<<"WRITE "<<s<<"\n";
  canvas->BeginText();
  canvas->SetFontAndSize(font_name[font_font[f]],
			 font_size[f]);
  canvas->MoveTextPos(double(x)/100.0+left, double(y)/100.0+bottom);
  canvas->ShowText(iso(s).c_str());
  x+=int(100.0*canvas->TextWidth(s.c_str()));
  canvas->EndText();
};

void PdfMedia::chordWrite(const string &s) {
  canvas->BeginText();
  if (chord_x<x) chord_x=x;
  canvas->SetFontAndSize(font_name[font_font[CHORD]],
			 font_size[CHORD]);
  canvas->MoveTextPos(double(chord_x)/100.0+left,
		      double(y)/100.0+double(lineSkip(NORMAL))*0.008+bottom);
  canvas->ShowText(s.c_str());
  chord_x+=int(100.0*canvas->TextWidth(s.c_str()))+chord_sep;
  canvas->EndText();
};

void PdfMedia::chordReset() {
  chord_x=x;
};

int PdfMedia::lineSkip(font f) {return 100*font_size[f];};

void PdfMedia::frame(int dx,int dy) const {
  canvas->Rectangle(double(x)/100.0+left,double(y)/100.0+bottom,
		    double(dx)/100.0,double(dy)/100.0);
  canvas->Stroke();
};

void PdfMedia::closePage() {
  if (page) {
    assert(canvas);
    page=0;canvas=0;
  } else assert(canvas!=0);
};

