#include "editor.h"
#include "frame.h"
//#include <iostream>


BEGIN_EVENT_TABLE(MyEditor, wxTextCtrl)
  EVT_TEXT(-1,MyEditor::OnText)
END_EVENT_TABLE()

MyEditor::MyEditor(wxWindow *parent, MyFrame *f)
  :wxTextCtrl(parent,-1,_T(""),wxDefaultPosition,
			  wxDefaultSize, wxTE_MULTILINE)
{
  frame=f;
};

void MyEditor::OnText(wxCommandEvent& WXUNUSED(event)) {
  //  std::cerr<<"ONText\n";
  frame->modified=true;
  frame->saved=false;
  frame->resetTitle();
};
