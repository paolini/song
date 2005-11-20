#ifndef EDITOR_H
#define EDITOR_H
#include <wx/wx.h>

class MyFrame;

class MyEditor: public wxTextCtrl {
  MyFrame *frame;
 public:
  MyEditor(wxWindow *parent,MyFrame *frame);
  void OnText(wxCommandEvent& WXUNUSED(event));
 private:
  DECLARE_EVENT_TABLE()
};

#endif
