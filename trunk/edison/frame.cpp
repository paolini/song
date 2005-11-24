#include "corda/song.h"
#include "corda/cursor.h"
#include <iostream>
#ifdef BUGGY
// questo dovrebbe essere già incluso da cursor.h
class Cursor {
 public:
  PhraseItem *item;
  int pos;
 public:
  Cursor(Song &song);
};
#endif

#include "frame.h"
#include "canvas.h"
#include "editor.h"
#include "list.h"


#include "corda/plug.h"

#include <string>
#include <sstream>
#include <cassert>

enum
{
    ID_Quit = 1,
    ID_Load, 
    ID_About,
    ID_Save,
    ID_SaveAs,
    ID_Export,
    ID_New
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Quit,  MyFrame::OnQuit)
  EVT_MENU(ID_Load,  MyFrame::OnLoad)
  EVT_MENU(ID_About, MyFrame::OnAbout)
  EVT_MENU(ID_Save, MyFrame::OnSave)
  EVT_MENU(ID_SaveAs, MyFrame::OnSaveAs)
  EVT_MENU(ID_Export, MyFrame::OnExport)
  EVT_MENU(ID_New, MyFrame::OnNew)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
  : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  wxMenu *menuExport = new wxMenu;
  menuExport->Append(ID_Export, "&PDF");

  wxMenu *menuFile = new wxMenu;
  
  menuFile->Append( ID_New, "&New" );
  menuFile->Append( ID_Load, "&Load..." );
  menuFile->Append( ID_Save, "&Save");
  menuFile->Append( ID_SaveAs, "Save as...");
  //  menuFile->Append(ID_Export,"&Export",menuExport);
  menuFile->AppendSeparator();
  menuFile->Append( ID_About, "&About..." );
  menuFile->AppendSeparator();
  menuFile->Append( ID_Quit, "E&xit" );
  

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuFile, "&File" );
  
  SetMenuBar( menuBar );
  
  CreateStatusBar();
  SetStatusText( "Welcome to edison corda song editor!" );

  tabs=new wxNotebook(this,-1);
  canvas=new MyCanvas(tabs,this);
  editor=new MyEditor(tabs,this,wxString());
  list=new MyList(tabs,this);

  tabs->AddPage(canvas,"view");
  tabs->AddPage(editor,"edit");
  tabs->AddPage(list,"list");

  cursor=0;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  if (AskSave()) return;
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox( "This is edison corda song editor",
                  "About Edison", wxOK | wxICON_INFORMATION );
}

void MyFrame::OnLoad(wxCommandEvent & WXUNUSED(event)) {
  if (AskSave()) return;
  wxFileDialog dia(this,"Choose file",
		   "","","*.sng");
  if (dia.ShowModal()==wxID_OK) {
    SetStatusText("Loading file: "+dia.GetPath());
    list->Load(dia.GetPath());
    //    Load(dia.GetPath());
  };
}

void MyFrame::OnSave(wxCommandEvent &event) {
  if (list->CurrentFile()->FileName()==wxString()) OnSaveAs(event);
  else list->Save();
}

void MyFrame::OnSaveAs(wxCommandEvent & WXUNUSED(event)) {
  wxFileDialog dia(this,"Save File as...","","","*.sng");
  if (dia.ShowModal()==wxID_OK) {
    list->SaveAs(dia.GetPath());
  };
}

void MyFrame::OnExport(wxCommandEvent &event) {
  std::cerr<<"export: "<<event.GetInt()<<"\n";
};

void MyFrame::OnNew(wxCommandEvent &event) {
  list->Load("");
};

void MyFrame::resetTitle() {
  wxString title="edison: ";
  FileItem *f=list->CurrentFile();
  if (f) {
    title+=f->FileName();
    if (f->Modified()) title+=" (modified)";
  } else {
    title+=" no file";
  }
  SetTitle(title);
};


int MyFrame::AskSave() {
  bool cancel=false;
  if (!(list->CurrentFile() && list->CurrentFile()->Modified())) 
    return cancel;
  wxMessageDialog *ask;
  ask=new wxMessageDialog(this,"File has been modified. Save?",
			  "save?",wxCANCEL|wxYES_NO);
  switch(ask->ShowModal()) {
  case wxID_CANCEL:
    std::cerr<<"cancel\n";
    cancel=true;
    goto fine;
  case wxID_YES:
    list->Save();
    std::cerr<<"yes\n";
    goto fine;
  case wxID_NO:
    std::cerr<<"no\n";
    goto fine;
  };
 fine:
  ask->Destroy();
  return cancel;
};

