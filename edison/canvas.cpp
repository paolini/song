#include "corda/song.h"

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <iostream>


#include "corda/cursor.h"
#include "corda/print.h"
#include "corda/plug.h"
#include "wxmedia.h"
#include "list.h"


#include "canvas.h"
#include "frame.h"
#include "editor.h"

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

  Item *f=frame->list->CurrentItem();
  try {
    if (f) {
      const Song *song=f->getSong();
      if (song)
	PrintSong(song,media,&opt,frame->cursor);
    }
    else
      std::cerr<<"OnDraw: empty song\n";
  } catch (PlugError &e) {
    wxString message;
    message<<"Line: "<<e.line<<" Col: "<<e.col<<"\n"<<e.what();
    std::cerr<<"Canvas: error caught\n";
    frame->tabs->SetSelection(1); // seleziona l'editor
    frame->editor->
      SetInsertionPoint(frame->editor->XYToPosition(e.col,e.line));
    wxMessageBox(message, "error", wxOK|wxICON_INFORMATION);
    frame->editor->SetInsertionPoint(frame->editor->XYToPosition(e.col,e.line));
  } catch (std::runtime_error &e) {
    wxMessageBox(e.what(), "error", wxOK|wxICON_INFORMATION);
  }
//  PrintSongs(frame->songlist,media,frame->cursor);
frame->SetStatusText("drawing... done!");
}
