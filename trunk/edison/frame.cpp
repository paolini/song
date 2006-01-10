#include "corda/song.h"
#include "corda/cursor.h"
#include <iostream>

#include "frame.h"
#include "canvas.h"
#include "editor.h"
#include "list.h"


#include "corda/plug.h"

#include <string>
#include <sstream>
#include <cassert>

#include "version.h"

enum {
    ID_Quit = 1,
    ID_Load, 
    ID_About,
    ID_Save,
    ID_SaveAs,
    ID_Export,
    ID_PDF,
    ID_PS,
    ID_TXT,
    ID_New,
    ID_InsertHeader,
    ID_InsertStanza,
    ID_InsertStrum,
    ID_InsertNotes
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Quit,  MyFrame::OnQuit)
  EVT_MENU(ID_Load,  MyFrame::OnLoad)
  EVT_MENU(ID_About, MyFrame::OnAbout)
  EVT_MENU(ID_Save, MyFrame::OnSave)
  EVT_MENU(ID_SaveAs, MyFrame::OnSaveAs)
  EVT_MENU(ID_PDF, MyFrame::OnExport)
  EVT_MENU(ID_PS, MyFrame::OnExport)
  EVT_MENU(ID_TXT, MyFrame::OnExport)
  EVT_MENU(ID_New, MyFrame::OnNew)
  EVT_MENU(ID_InsertHeader, MyFrame::OnInsert)
  EVT_MENU(ID_InsertStanza, MyFrame::OnInsert)
  EVT_MENU(ID_InsertStrum, MyFrame::OnInsert)
  EVT_MENU(ID_InsertNotes, MyFrame::OnInsert)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
  : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  wxMenu *menuExport = new wxMenu;
  menuExport->Append(ID_PDF, "&PDF");
  menuExport->Append(ID_PS, "P&S");
  menuExport->Append(ID_TXT, "&TXT");

  wxMenu *menuFile = new wxMenu;
  
  menuFile->Append( ID_New, "&New" );
  menuFile->Append( ID_Load, "&Load..." );
  menuFile->Append( ID_Save, "&Save");
  menuFile->Append( ID_SaveAs, "Save as...");
  menuFile->Append(ID_Export,"&Export",menuExport);
  menuFile->AppendSeparator();
  menuFile->Append( ID_About, "&About..." );
  menuFile->AppendSeparator();
  menuFile->Append( ID_Quit, "E&xit" );
  
  this->menuInsert = new wxMenu;
  
  menuInsert->Append(ID_InsertHeader,"Insert Song &Header");
  menuInsert->Append(ID_InsertStanza,"Insert Song &Stanza");
  menuInsert->Append(ID_InsertStrum,"Insert Stru&mental");
  menuInsert->Append(ID_InsertNotes,"Insert &Notes");

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File" );
  menuBar->Append(menuInsert, "&Insert");

  SetMenuBar( menuBar );
  
  CreateStatusBar();
  SetStatusText( "Welcome to edison corda song editor!" );

  tabs=new wxNotebook(this,-1);
  canvas=new MyCanvas(tabs,this);
  editor=new MyEditor(tabs,this,wxString());
  editor->Enable(false);
  menuInsert->Enable(-1,false);

  list=new MyList(tabs,this);
  
  //  wxListCtrl* debug=new wxListCtrl(tabs);

  tabs->AddPage(canvas,"view");
  tabs->AddPage(editor,"edit");
  tabs->AddPage(list,"list");

  //  tabs->AddPage(debug,"debug");

  cursor=0;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  if (AskSave()) return;
  Close(TRUE);
  exit(0);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(wxString("This is edison song editor "
			"(manu-fatto) 2005 -- VER. edison-" 
			VERSION " [")+SongVersion().c_str()+"]",
	       "About Edison", wxOK | wxICON_INFORMATION );
}

void MyFrame::OnLoad(wxCommandEvent & WXUNUSED(event)) {
  if (AskSave()) return;
  wxFileDialog dia(this,"Load file",
		   "","","SNG files (*.sng)|*.sng|ChordPro files (*.cho)|*.cho",wxOPEN|wxCHANGE_DIR);
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
  wxString plug;
  std::cerr<<"export: "<<event.GetId()<<"\n";
  switch(event.GetId()) {
  case ID_PDF: plug="pdf"; break;
  case ID_PS: plug="ps"; break;
  case ID_TXT: plug="txt"; break;
  default: assert(false);
  };
  wxFileDialog dia(this,"Export to file","","","*."+plug);
  if (dia.ShowModal()==wxID_OK) {
    list->Export(dia.GetPath(),plug);
  };
};

void MyFrame::OnNew(wxCommandEvent &event) {
  list->Load("");
  if (editor->InsertHeader())
    editor->InsertStanza();
  canvas->Refresh();
};

void MyFrame::OnInsert(wxCommandEvent &event) {
  switch(event.GetId()) {
  case ID_InsertHeader: 
    editor->InsertHeader();
    break;
  case ID_InsertStanza:
    editor->InsertStanza();
    break;
  case ID_InsertStrum:
    editor->WriteText("\\m{}");
    editor->SetInsertionPoint(editor->GetInsertionPoint()-1);
    break;
  case ID_InsertNotes:
    editor->WriteText("\\n{}");
    editor->SetInsertionPoint(editor->GetInsertionPoint()-1);
    break;
  default: assert(false);
  }
  canvas->Refresh();
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

