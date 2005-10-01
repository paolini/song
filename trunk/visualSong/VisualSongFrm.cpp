//---------------------------------------------------------------------------
//
// Name:        VisualSongFrm.cpp
// Author:      Matteo
// Created:     04/09/2005 10.06.35
//
//---------------------------------------------------------------------------

#include "VisualSongFrm.h"

//Do not add custom headers.
//wx-dvcpp designer will remove them
////Header Include Start
////Header Include End


//----------------------------------------------------------------------------
// VisualSongFrm
//----------------------------------------------------------------------------
   //Add Custom Events only in the appropriate Block.
   // Code added in  other places will be removed by wx-dvcpp 
  ////Event Table Start
BEGIN_EVENT_TABLE(VisualSongFrm,wxFrame)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(VisualSongFrm::VisualSongFrmClose)
	EVT_MENU(ID_MNU_SONG_EDITOR_1003, VisualSongFrm::Mnusongeditor1003Click)
END_EVENT_TABLE()
  ////Event Table End



VisualSongFrm::VisualSongFrm( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style )
    : wxFrame( parent, id, title, position, size, style)
{
    CreateGUIControls();
}

VisualSongFrm::~VisualSongFrm() {} 

void VisualSongFrm::CreateGUIControls(void)
{
    //Do not add custom Code here
    //wx-devcpp designer will remove them.
    //Add the custom code before or after the Blocks
    ////GUI Items Creation Start

	WxPanel1 = new wxPanel(this, ID_WXPANEL1, wxPoint(0,0), wxSize(580,486));

	WxMenuBar1 =  new wxMenuBar();
	wxMenu *ID_MNU__FILE_1001_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(ID_MNU__FILE_1001_Mnu_Obj, wxT("&File"));
	
	wxMenu *ID_MNU__TOOLS_1002_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(ID_MNU__TOOLS_1002_Mnu_Obj, wxT("&Tools"));
	
	ID_MNU__TOOLS_1002_Mnu_Obj->Append(ID_MNU_SONG_EDITOR_1003, wxT("Song&Editor"), wxT(""), wxITEM_NORMAL);
	
	
	this->SetMenuBar(WxMenuBar1);

	this->SetSize(8,8,588,520);
	this->SetTitle(wxT("VisualSong"));
	this->Center();
	this->SetIcon(wxNullIcon);
	
    ////GUI Items Creation End
}

void VisualSongFrm::VisualSongFrmClose(wxCloseEvent& event)
{
    // --> Don't use Close with a Frame,
    // use Destroy instead.
    Destroy();
}
 


/*
 * Mnusongeditor1003Click
 */
void VisualSongFrm::Mnusongeditor1003Click(wxCommandEvent& event)
{
	// insert your code here
    SEFrm = new SongEditorFrm(NULL);
	SEFrm->Show(TRUE);
}
