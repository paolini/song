#include "song.h"
#include "cursor.h"

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


#include "plug.h"

#include <string>
#include <sstream>
#include <cassert>

enum
{
    ID_Quit = 1,
    ID_Load, 
    ID_About,
    ID_Save,
    ID_Export
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Quit,  MyFrame::OnQuit)
  EVT_MENU(ID_Load,  MyFrame::OnLoad)
  EVT_MENU(ID_About, MyFrame::OnAbout)
  EVT_MENU(ID_Save, MyFrame::OnSave)
  EVT_MENU(ID_Export, MyFrame::OnExport)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
  : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  wxMenu *menuExport = new wxMenu;
  menuExport->Append(ID_Export, "&PDF");

  wxMenu *menuFile = new wxMenu;
  
  menuFile->Append( ID_Load, "&Load..." );
  menuFile->Append( ID_Save, "&Save");
  menuFile->Append(ID_Export,"&Export",menuExport);
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
    editor=new MyEditor(tabs,this);
    list=new MyList(tabs,this);
    tabs->AddPage(canvas,"view");
    tabs->AddPage(editor,"edit");
    tabs->AddPage(list,"list");
    cursor=0;
    //    filename="";
    modified=false;
    saved=true;
}

void MyFrame::compile() {
  if (editor->IsModified()) modified=true;
  editor->DiscardEdits();
  if (!modified) return;
  try {
    // visualizza
    Plugin* reader=Plugin::Construct("sng");
    if (!reader) {
      wxMessageBox("cannot create SNG reader","error", wxOK|wxICON_INFORMATION);
      //    cerr<<e.what()<<"\n";
      Close(TRUE);
    }
    songlist.clear(); // cancella le canzoni caricate
    
    std::string buf=editor->GetValue().c_str();
    std::stringstream in(buf);

    //reader->Read(std::string(filename),songlist);
        reader->Read(in,songlist);
 
    if (songlist.size()) 
      cursor=new Cursor(*songlist[0]);
    else
      wxMessageBox("No song in file","warning", wxOK|wxICON_INFORMATION);
    // inserisce il cursore:
  } catch(std::runtime_error &e) {
    wxMessageBox(e.what(), "error", wxOK|wxICON_INFORMATION);
    //    cerr<<e.what()<<"\n";
    Close(TRUE);
  }
  modified=false;
  resetTitle();
  canvas->Update();
}

void MyFrame::load(const wxString &name) {
  SetStatusText("reading file "+name+"...");
  filename=name;
  editor->LoadFile(filename);
  saved=true;
  modified=true;
  compile();
};

void MyFrame::save() {
  if (filename==wxString()) {
    wxMessageBox("no file loaded", "warning", wxOK | wxICON_INFORMATION);
    return;
  }
  if (saved) {
    wxMessageBox("file has not been modified", "warning", 
		 wxOK | wxICON_INFORMATION);
    return;
  }
  editor->SaveFile(filename);
  saved=true;
  resetTitle();
};

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
    SetStatusText("Load file : "+dia.GetPath());
    load(dia.GetPath());
    Refresh();
  };
}

void MyFrame::OnSave(wxCommandEvent & WXUNUSED(event)) {
  save();
}

void MyFrame::OnExport(wxCommandEvent &event) {
  std::cerr<<"export: "<<event.GetInt()<<"\n";
};

void MyFrame::resetTitle() {
  wxString title="edison ";
  title+=filename;
  if (!saved) title+=" (modified) ";
  if (modified) title+=" (edited) ";
  SetTitle(title);
};


int MyFrame::AskSave() {
  bool cancel=false;
  if (saved) return cancel;
  wxMessageDialog *ask;
  ask=new wxMessageDialog(this,"File has been modified. Save?",
			  "save?",wxCANCEL|wxYES_NO);
  switch(ask->ShowModal()) {
  case wxID_CANCEL:
    std::cerr<<"cancel\n";
    cancel=true;
    goto fine;
  case wxID_YES:
    save();
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
