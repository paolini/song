#include <cassert>
#include "wxmedia.h"
#include <iso.h>

wxString convert(const std::string &utf8) {
  wxString ret;
  for (size_t i=0;i<utf8.size();) {
    ret+=unicode(utf8,i);
  }
  return ret;
};


static const char *font_name[4]={"FR","FI","FB","FC"};
// roman, italic, bold

wxMedia::wxMedia(wxDC &canvas, int height, int start_page): dc(canvas), h(height) {
  x=10*100;
  y=10*100;
  chord_x=10*100;
};

wxMedia::~wxMedia() {
};

void wxMedia::newPage() {
};

void wxMedia::goto_xy(int xx,int yy) {
  x=xx;y=yy;
};

void wxMedia::move_xy(int xx,int yy) {
  goto_xy(x+xx,y+yy);
};

int wxMedia::get_x() {return x;};

int wxMedia::get_y() {return y;};

int wxMedia::wordWidth(const string &s, font f) const {
  int x,y;
  wxString str=convert(s);
  dc.GetTextExtent(str,&x,&y);
  return int(100.0*x);
};

void wxMedia::wordWrite(const string &s, font f) {
  //  cerr<<"WRITE "<<s<<"\n";
  wxString str=convert(s);
  dc.DrawText(str,x/100,h-y/100);
  //  cerr<<"DrawText"<<s.c_str()<<","<<x/100<<","<<y/100<<")\n";
  x+=wordWidth(s,f);
};

void wxMedia::chordWrite(const string &s) {
  wxString str=convert(s);
  if (chord_x<x) chord_x=x;
  dc.DrawText(str,chord_x/100,h-y/100-lineSkip(NORMAL)*10/1000);
  int a,b;
  dc.GetTextExtent(str,&a,&b);
  chord_x+=a*100+chord_sep;
};

void wxMedia::chordReset() {
  chord_x=x;
};

int wxMedia::lineSkip(font f) {return 100*12;};

void wxMedia::frame(int dx,int dy) const {
};

void wxMedia::closePage() {
};

