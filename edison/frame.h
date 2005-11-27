#ifndef FRAME_H
#define FRAME_H
#include "corda/song.h"
#include "list.h"

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
class MyList;

class MyFrame: public wxFrame
{
public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

  //  void Load(const wxString &fileName);
  
  void OnQuit(wxCommandEvent&);
  void OnAbout(wxCommandEvent&);
  void OnLoad(wxCommandEvent&);
  void OnSave(wxCommandEvent&);
  void OnSaveAs(wxCommandEvent&);
  void OnExport(wxCommandEvent&);
  void OnNew(wxCommandEvent&);
  void OnInsert(wxCommandEvent &);

  void resetTitle();
  
  Cursor *cursor;
  wxNotebook *tabs;
  MyCanvas *canvas;
  MyEditor *editor;
  MyList *list;
  
  wxMenu *menuInsert;


  //  FileItem file;

  int AskSave();

private:
  DECLARE_EVENT_TABLE()
};
#endif
