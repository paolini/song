#include <fstream>
#include <cassert>
#include <hpdf.h>
//#include <libharu.h>

//#include <libharu_ISO8859.h>

#include <iostream>
#include <sstream>
#include "pdfmedia.h"

#include "iso.h"

void  
error_handler (HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void         *user_data)
{
    /* throw exception when an error has occured */
    printf ("ERROR: error_no=%04X, detail_no=%d\n", (unsigned int)error_no,    
        (int)detail_no);
    throw PdfException("libharu error");
}

PdfMedia::PdfMedia(const string &name, int start_page): filename(name) {
  doc = HPDF_New(error_handler, NULL);
  const char* encoding = "ISO8859-9";
  // HPDF_Encoder encoding=HPDF_GetEncoder(doc, "ISO8859");
  //   doc->AddType1Font(new PdfHelveticaFontDef(),font_name[0],encoding);
  //   doc->AddType1Font(new PdfHelveticaObliqueFontDef(),font_name[1],encoding);
  //   doc->AddType1Font(new PdfHelveticaBoldFontDef(),font_name[2],encoding);
  font_ptr[0] = HPDF_GetFont(doc, "Times-Roman", encoding);
  font_ptr[1] = HPDF_GetFont(doc, "Times-Italic", encoding);
  font_ptr[2] = HPDF_GetFont(doc, "Times-Bold", encoding);
  font_ptr[3] = HPDF_GetFont(doc, "Courier", encoding);
  
  // page=0;
  // canvas=0;
  page_no=start_page-1;
  newPage();
};

PdfMedia::~PdfMedia() {
  HPDF_SaveToFile(doc, filename.c_str());
  HPDF_Free(doc);
};

void PdfMedia::newPage() {
  page = HPDF_AddPage(doc);
  page_no++;
  //canvas=page->Canvas();
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
  // assert(canvas);
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
  HPDF_Page_SetFontAndSize(page, font_ptr[font_font[f]], font_size[f]);
  return int(100.0*HPDF_Page_TextWidth(page, iso(s).c_str()));
};

void PdfMedia::wordWrite(const string &s, font f) {
  //  cerr<<"WRITE "<<s<<"\n";
  HPDF_Page_BeginText(page);
  HPDF_Page_SetFontAndSize(page, font_ptr[font_font[f]], font_size[f]);
  HPDF_Page_MoveTextPos(page, double(x)/100.0+left, double(y)/100.0+bottom);
  HPDF_Page_ShowText(page, iso(s).c_str());
  x+=int(100.0*HPDF_Page_TextWidth(page, s.c_str()));
  HPDF_Page_EndText(page);
};

void PdfMedia::chordWrite(const string &s) {
  HPDF_Page_BeginText(page);
  if (chord_x<x) chord_x=x;
  HPDF_Page_SetFontAndSize(page, font_ptr[font_font[CHORD]],
         font_size[CHORD]);
  HPDF_Page_MoveTextPos(page, double(chord_x)/100.0+left,
      double(y)/100.0+double(lineSkip(NORMAL))*0.008+bottom);
  HPDF_Page_ShowText(page, iso(s).c_str());
  chord_x+=int(100.0*HPDF_Page_TextWidth(page, s.c_str()))+chord_sep;
  HPDF_Page_EndText(page);
};

void PdfMedia::chordReset() {
  chord_x=x;
};

int PdfMedia::lineSkip(font f) {return 100*font_size[f];};

void PdfMedia::frame(int dx,int dy) const {
  HPDF_Page_SetLineWidth(page, 1);
  HPDF_Page_Rectangle(page, double(x)/100.0+left,double(y)/100.0+bottom,
          double(dx)/100.0,double(dy)/100.0);
  HPDF_Page_Stroke(page);
};

