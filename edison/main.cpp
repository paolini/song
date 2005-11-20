#include "song.h"

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

#include "cursor.h"
#include "print.h"
#include "plug.h"
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
    return TRUE;
}

