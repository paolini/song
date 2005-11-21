#include "corda/song.h"

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

#ifdef BUGGY
// dovrebbe essere già definito da cursor.h!!
hclass Cursor {
 public:
  PhraseItem *item;
  int pos;
 public:
  Cursor(Song &song);
};
#endif

#include "corda/cursor.h"
#include "corda/print.h"
#include "corda/plug.h"
#include "wxmedia.h"
#include "canvas.h"


#ifdef BUGGY
// dovrebbe essere già definita in print.h!
void PrintSongs(const SongList &,Media &, const Cursor* cursor=0);
#endif

#include "frame.h"

using namespace std;
#include <iostream>


class MyApp: public wxApp
{
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
    MyFrame *frame = new MyFrame( "EDISON Corda Song Editor", wxPoint(50,50), 
				  wxSize(450,340) );
    frame->Show( TRUE );
    SetTopWindow( frame );
    if (argc>1) {
      std::cerr<<"OnInit: argv[1]="<<argv[1]<<"\n";
      frame->Load(argv[1]);
    }
    return TRUE;
}

