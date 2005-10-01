//---------------------------------------------------------------------------
//
// Name:        VisualSongFrm.h
// Author:      Matteo
// Created:     04/09/2005 10.06.35
//
//---------------------------------------------------------------------------
#ifndef __VisualSongFrm_HPP_
#define __VisualSongFrm_HPP_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif
#include "SongEditorFrm.h"

//Do not add custom headers.
//wx-dvcpp designer will remove them
////Header Include Start
#include <wx/menu.h>
#include <wx/panel.h>
////Header Include End

#include <wx/frame.h>

////Dialog Style Start
	#undef VisualSongFrm_STYLE
	#define VisualSongFrm_STYLE wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

class VisualSongFrm : public wxFrame
{
private:
    DECLARE_EVENT_TABLE()
public:
    VisualSongFrm( wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("VisualSong"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = VisualSongFrm_STYLE);
    virtual ~VisualSongFrm();
public:
  //Do not add custom Control Declarations here.
  //wx-devcpp will remove them. Try adding the custom code 
  //after the block.
  ////GUI Control Declaration Start
	wxMenuBar *WxMenuBar1;
	wxPanel *WxPanel1;
  ////GUI Control Declaration End
  SongEditorFrm *SEFrm;
public:
	//Note: if you receive any error with these enums, then you need to
    //change your old form code that are based on the #define control ids.
	//It may replace a numeric value in the enums names.
	enum {
////GUI Enum Control ID Start
ID_MNU__FILE_1001 = 1001,
ID_MNU__TOOLS_1002 = 1002,
ID_MNU_SONG_EDITOR_1003 = 1003,

ID_WXPANEL1 = 1009,
////GUI Enum Control ID End
   ID_DUMMY_VALUE_ //Dont Delete this DummyValue
   }; //End of Enum
public:
    void VisualSongFrmClose(wxCloseEvent& event);
    void CreateGUIControls(void);
	void Mnusongeditor1003Click(wxCommandEvent& event);

};

#endif
 
 
 
 
