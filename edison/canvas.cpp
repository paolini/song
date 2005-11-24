#include "corda/song.h"

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <iostream>

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


#include "corda/cursor.h"
#include "corda/print.h"
#include "corda/plug.h"
#include "wxmedia.h"
#include "list.h"

#ifdef BUGGY
// dovrebbe essere già definita in print.h!
void PrintSongs(const SongList &,Media &, const Cursor* cursor);
#endif


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

  FileItem *f=frame->list->CurrentFile();
  if (f && f->getSong())
    PrintSong(f->getSong(),media,frame->cursor);
  else
    std::cerr<<"OnDraw: empty song\n";
  //  PrintSongs(frame->songlist,media,frame->cursor);
  frame->SetStatusText("drawing... done!");
}


