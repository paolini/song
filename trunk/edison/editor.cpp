#include "editor.h"
#include "frame.h"
#include <iostream>


BEGIN_EVENT_TABLE(MyEditor, wxTextCtrl)
  EVT_TEXT(-1,MyEditor::OnText)
END_EVENT_TABLE()

MyEditor::MyEditor(wxWindow *parent, MyFrame *f, const wxString &content)
  :wxTextCtrl(parent,-1,content,wxDefaultPosition,
	      wxDefaultSize, wxTE_MULTILINE),
   freeze(0)
{
  std::cerr<<"MyEditor: content="<<GetValue().size()<<"\n";
 frame=f;
};

void MyEditor::OnText(wxCommandEvent& WXUNUSED(event)) {
  if (freeze) {std::cerr<<"OnText freezed \n";return;}
  std::cerr<<"OnText: content="<<GetValue().size()<<"\n";
  //  std::cerr<<"ONText\n";
  /*  frame->modified=true;
      frame->saved=false;*/
  frame->file.setContent(GetValue());
  frame->resetTitle();
};

void MyEditor::Set(const wxString &val) {
  freeze++;
  SetValue(val);
  freeze--;
};
