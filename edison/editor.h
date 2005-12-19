#ifndef EDITOR_H
#define EDITOR_H
#include <wx/wx.h>

class MyFrame;

class MyEditor: public wxTextCtrl {
  MyFrame *frame;
  int freeze;
 public:
  MyEditor(wxWindow *parent,MyFrame *frame,const wxString &content);
  void OnTextVoid();
  void OnText(wxCommandEvent& WXUNUSED(event));
  void Set(const wxString &val);
  bool InsertHeader();
  bool InsertStanza();
  void OnChar(wxKeyEvent&);
 private:
  DECLARE_EVENT_TABLE()
};

#endif
