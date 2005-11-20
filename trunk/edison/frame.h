#ifndef FRAME_H
#define FRAME_H
#include "song.h"

/*
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif



#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
*/

#include "wx/wx.h"
#include "wx/notebook.h"

class MyCanvas;
class Cursor;
class MyEditor;

class MyFrame: public wxFrame
{
public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnLoad(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnExport(wxCommandEvent& event);

  void resetTitle();
  
  wxNotebook *tabs;
  MyCanvas *canvas;
  Cursor *cursor;
  MyEditor *editor;

  SongList list;
  
  wxString filename;
  bool modified; // editor e view non sono sincronizzati
  bool saved; // editor e file non sono sincronizzati

  void load(const wxString &fileName);
  void save();
  void compile();
  
  int AskSave();

private:
  DECLARE_EVENT_TABLE()
};
#endif
