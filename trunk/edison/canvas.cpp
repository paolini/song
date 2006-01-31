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
  
  //  frame->SetStatusText("drawing... ");
  wxMedia media(dc,h);
 
  try {
    const Song* song=frame->list->CurrentSong();
    if (song) {
      PrintSong(song,media,&opt,frame->cursor);
    } else {
      std::cerr<<"MyCanvas::OnDraw: Error in song to be drawn\n";
      if (frame->list->CurrentFile() 
	  && frame->list->CurrentFile()->status==FileItem::COMPILED_ERROR) {
	PlugError &e=frame->list->CurrentFile()->compilation_error;
	wxString message;
	message<<"Line: "<<e.line<<" Col: "<<e.col<<"\n"<<e.what();
	std::cerr<<"Canvas: error caught\n";
	//	frame->tabs->SetSelection(1); // seleziona l'editor
	//	frame->editor->
	//	  SetInsertionPoint(frame->editor->XYToPosition(e.col,e.line));
	std::cerr<<"Error message: "<<message<<"\n";
	//	wxMessageBox(message, "error", wxOK|wxICON_INFORMATION);
      }
    }
  } catch (std::runtime_error &e) {
    wxMessageBox(e.what(), "error", wxOK|wxICON_INFORMATION);
  }
  //  PrintSongs(frame->songlist,media,frame->cursor);
  //  frame->SetStatusText("drawing... done!");
}
