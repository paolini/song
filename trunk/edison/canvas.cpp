#include "song.h"

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include "wx/colordlg.h"
//#include "wx/image.h"
//#include "wx/artprov.h"

#ifdef BUGGY
// dovrebbe essere già definito da cursor.h!!
class Cursor {
 public:
  PhraseItem *item;
  int pos;
 public:
  Cursor(Song &song);
};
#endif


#include "cursor.h"
#include "print.h"
#include "plug.h"
#include "wxmedia.h"



// dovrebbe essere già definita in print.h!
void PrintSongs(const SongList &,Media &, const Cursor* cursor);


#include "canvas.h"
#include "frame.h"

MyCanvas::MyCanvas(wxWindow *parent, MyFrame *fr)
  : wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
  frame = fr;
  SetScrollbars(0,10,0,10);
}

void MyCanvas::OnDraw(wxDC &dc)
{
  frame->compile();
  wxBrush brushHatch(*wxRED, wxFDIAGONAL_HATCH);
  dc.SetBrush(brushHatch);
  wxBrush whiteBrush(*wxWHITE,wxSOLID);
  dc.SetBackgroundMode(wxTRANSPARENT);
  dc.SetBackground(whiteBrush);
  dc.Clear();
  
  int w=400,h=750;
  
  SetVirtualSize(w,int(h*1.1));
  
  frame->SetStatusText("drawing... ");
  wxMedia media(dc,h);
  
  PrintSongs(frame->list,media,frame->cursor);
  frame->SetStatusText("drawing... done!");
}

