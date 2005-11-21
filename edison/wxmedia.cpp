#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include <cassert>
#include "wxmedia.h"
#include "corda/iso.h"

wxString convert(const std::string &utf8) {
  wxString ret;
  for (size_t i=0;i<utf8.size();) {
    ret+=unicode(utf8,i);
  }
  return ret;
};


// static const char *font_name[4]={"FR","FI","FB","FC"};
// roman, italic, bold

wxMedia::wxMedia(wxDC &canvas, int height, int start_page): dc(canvas), h(height) {
  x=10*100;
  y=10*100;
  chord_x=10*100;
  for (int i=0;i<Media::NOFONT;++i) {
    fontSize[i]=14;
    fontColor[i]=*wxBLACK;
  }
  fontSize[Media::TITLE]=20;
  fontSize[Media::AUTHOR]=16;
  fontSize[Media::CHORD]=10;
  fontColor[Media::CHORD]=*wxGREEN;
  fontColor[Media::TITLE]=*wxBLUE;
  fontColor[Media::AUTHOR]=*wxBLUE;

  for (int i=0;i<Media::NOFONT;++i) {
    int style=wxFONTSTYLE_NORMAL;
    if (i==REFRAIN) style=wxFONTSTYLE_SLANT;
    fontFont[i]=wxFont(fontSize[i],
    wxFONTFAMILY_DEFAULT,
    style,
    wxFONTWEIGHT_NORMAL);
  }

  cursorColor=*wxRED;
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
  dc.SetFont(fontFont[f]);
  dc.GetTextExtent(str,&x,&y);
  return int(100.0*x);
};

void wxMedia::wordWrite(const string &s, font f) {
  //  cerr<<"WRITE "<<s<<"\n";
  wxString str=convert(s);
  dc.SetTextForeground(fontColor[f]);
  dc.SetFont(fontFont[f]);
  dc.DrawText(str,x/100,h-(y+lineSkip(f))/100);
  //  cerr<<"DrawText"<<s.c_str()<<","<<x/100<<","<<y/100<<")\n";
  x+=wordWidth(s,f);
};

void wxMedia::cursorWrite(font f) {
  dc.SetTextForeground(cursorColor);
  dc.DrawText("|",x/100,y/100);
    wxMessageBox( "This is edison corda song editor",
                  "About Edison", wxOK | wxICON_INFORMATION );
 }

void wxMedia::chordWrite(const string &s) {
  wxString str=convert(s);
  if (chord_x<x) chord_x=x;
  dc.SetTextForeground(fontColor[CHORD]);
  dc.SetFont(fontFont[CHORD]);
  dc.DrawText(str,chord_x/100,h-(y+lineSkip(CHORD))/100-lineSkip(NORMAL)*8/1000);
  int a,b;
  dc.GetTextExtent(str,&a,&b);
  chord_x+=a*100+chord_sep;
};

void wxMedia::chordReset() {
  chord_x=x;
};

int wxMedia::lineSkip(font f) {return 150*fontSize[f];};

void wxMedia::frame(int dx,int dy) const {
};

void wxMedia::closePage() {
};

