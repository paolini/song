//---------------------------------------------------------------------------
//
// Name:        SongEditorFrm.h
// Author:      Matteo
// Created:     05/09/2005 11.53.30
//
//---------------------------------------------------------------------------
#ifndef __SONGEDITORFRM_HPP_
#define __SONGEDITORFRM_HPP_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

//Do not add custom headers.
//wx-dvcpp designer will remove them
////Header Include Start
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
////Header Include End

#include <wx/frame.h>
#include <wx/filename.h>

////Dialog Style Start
	#undef SongEditorFrm_STYLE
	#define SongEditorFrm_STYLE wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

class SongEditorFrm : public wxFrame
{
private:
    DECLARE_EVENT_TABLE()
public:
    SongEditorFrm( wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Untitled1"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = SongEditorFrm_STYLE);
    virtual ~SongEditorFrm();
public:

  //Do not add custom Control Declarations here.
  //wx-devcpp will remove them. Try adding the custom code 
  //after the block.
  ////GUI Control Declaration Start
	wxFileDialog *WxSaveFileDialog1;
	wxMenuBar *WxMenuBar1;
	wxFileDialog *WxOpenFileDialog1;
	wxTextCtrl *WxMemo2;
	wxButton *WxButton1;
	wxTextCtrl *WxMemo1;
	wxPanel *WxPanel1;
  ////GUI Control Declaration End

public:
    //Note: if you receive any error with these enums, then you need to
    //change your old form code that are based on the #define control ids.
    //#defines may replace a numeric value for the enums names.
    //Try copy pasting the below block in your old Form header Files.
	enum {
////GUI Enum Control ID Start
ID_MNU_FILE_1001 = 1001,
ID_MNU_OPENFILE_1006 = 1006,
ID_MNU_SAVEFILE_1008 = 1008,
ID_MNU_NEWFILE_1009 = 1009,

ID_WXMEMO2 = 1005,
ID_WXBUTTON1 = 1004,
ID_WXMEMO1 = 1003,
ID_WXPANEL1 = 1002,
////GUI Enum Control ID End
   ID_DUMMY_VALUE_ //Dont Delete this DummyValue
   }; //End of Enum

public:
    void SongEditorFrmClose(wxCloseEvent& event);
    void CreateGUIControls(void);
	void Mnusavefile1008Click(wxCommandEvent& event);
	void Mnuopenfile1006Click(wxCommandEvent& event);
	void Mnusavefile1007Click(wxCommandEvent& event);
	void WxButton1Click(wxCommandEvent& event);
	void Mnunewfile1009Click(wxCommandEvent& event);

};


#endif
 
 
 
 
