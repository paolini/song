#ifndef EDITOR_H
#define EDITOR_H
#include <wx/wx.h>

class MyFrame;

class MyEditor: public wxTextCtrl {
  MyFrame *frame;
  int freeze;
 public:
  MyEditor(wxWindow *parent,MyFrame *frame,const wxString &content);
  void OnText(wxCommandEvent& WXUNUSED(event));
  void Set(const wxString &val);
  void InsertHeader();
  void InsertStanza();
 private:
  DECLARE_EVENT_TABLE()
};

#endif
