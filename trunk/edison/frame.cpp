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
#include "plug.h"

#include <string>
#include <sstream>
#include <cassert>

enum
{
    ID_Quit = 1,
    ID_Load, 
    ID_About,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Quit,  MyFrame::OnQuit)
  EVT_MENU(ID_Load,  MyFrame::OnLoad)
  EVT_MENU(ID_About, MyFrame::OnAbout)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_Load, "&Load..." );
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
    editor=new wxTextCtrl(tabs,-1,_T(""),wxDefaultPosition,
			  wxDefaultSize, wxTE_MULTILINE);
    tabs->AddPage(canvas,"view");
    tabs->AddPage(editor,"edit");
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
    list.clear(); // cancella le canzoni caricate
    
    std::string buf=editor->GetValue().c_str();
    std::stringstream in(buf);

    //reader->Read(std::string(filename),list);
        reader->Read(in,list);
 
    if (list.size()) 
      cursor=new Cursor(*list[0]);
    else
      wxMessageBox("No song in file","warning", wxOK|wxICON_INFORMATION);
    // inserisce il cursore:
  } catch(std::runtime_error &e) {
    wxMessageBox(e.what(), "error", wxOK|wxICON_INFORMATION);
    //    cerr<<e.what()<<"\n";
    Close(TRUE);
  }
  modified=false;
}

void MyFrame::load(const wxString &filename) {
  SetStatusText("reading file "+filename+"...");

  editor->LoadFile(filename);
  saved=true;
  modified=true;
  compile();
};

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox( "This is edison corda song editor",
                  "About Edison", wxOK | wxICON_INFORMATION );
}

void MyFrame::OnLoad(wxCommandEvent & WXUNUSED(event)) {
  wxFileDialog dia(this,"Choose file",
		   "","","*.sng");
  if (dia.ShowModal()==wxID_OK) {
    SetStatusText("Load file : "+dia.GetPath());
    load(dia.GetPath());
    Refresh();
  };
}

