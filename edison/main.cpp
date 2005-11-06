
// from Hello World sample:
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include "wx/colordlg.h"
//#include "wx/image.h"
//#include "wx/artprov.h"

#include "print.h"
#include "song.h"
#include "plug.h"
#include "wxmedia.h"


class MyApp: public wxApp
{
    virtual bool OnInit();
};

class MyFrame;

class MyCanvas: public wxScrolledWindow {
  MyFrame *frame;
public:
  MyCanvas(MyFrame *parent);
  
  //void OnPaint(wxPaintEvent &event);
  virtual void MyCanvas::OnDraw(wxDC &dc);
  void OnMouseMove(wxMouseEvent &event);
  
  //  void Show(ScreenToShow show) { m_show = show; Refresh(); }
  
};

class MyFrame: public wxFrame
{
public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnLoad(wxCommandEvent& envent);
  
  MyCanvas *canvas;

  SongList list;
  
  void load(const wxString &filename) {
    Plugin* reader=Plugin::Construct("sng");
    if (!reader) {
      cerr<<"cannot create sng reader\n";
    abort();
    }
    try {
      list.clear(); // cancella le canzoni caricate
      cerr<<"reading file "<<filename<<"\n";
      reader->Read(string(filename),list);
    } catch(runtime_error &e) {
      cerr<<e.what()<<"\n";
      abort();
    };
  };
  
private:
  DECLARE_EVENT_TABLE()
};

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

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( "EDISON Corda Song Editor", wxPoint(50,50), wxSize(450,340) );
    frame->Show( TRUE );
    SetTopWindow( frame );
    return TRUE;
}

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
    SetStatusText( "Welcome to wxWidgets!" );

    canvas=new MyCanvas(this);

}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close( TRUE );
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox( "This is a wxWidgets' Hello world sample",
                  "About Hello World", wxOK | wxICON_INFORMATION );
}

void MyFrame::OnLoad(wxCommandEvent & WXUNUSED(event)) {
  wxFileDialog dia(this,"Choose file",
		   "","","*.sng");
  if (dia.ShowModal()==wxID_OK) {
    cerr<<"Load file : "<<dia.GetPath()<<"\n";
    load(dia.GetPath());
  };
}

MyCanvas::MyCanvas(MyFrame *parent)
  : wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
  frame = parent;
  SetScrollbars(0,10,0,10);
}

void MyCanvas::OnDraw(wxDC &dc)
{
    wxBrush brushHatch(*wxRED, wxFDIAGONAL_HATCH);
    dc.SetBrush(brushHatch);
    wxBrush whiteBrush(*wxWHITE,wxSOLID);
    dc.SetBackgroundMode(wxSOLID);
    dc.SetBackground(whiteBrush);
    dc.Clear();
    
    int w=400,h=750;

    SetVirtualSize(w,h);

    cerr<<"DRAW\n";
    wxMedia media(dc,h);

    PrintSongs(frame->list,media);
}

