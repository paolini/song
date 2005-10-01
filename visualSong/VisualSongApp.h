//---------------------------------------------------------------------------
//
// Name:        VisualSongApp.h
// Author:      Matteo
// Created:     04/09/2005 10.06.35
//
//---------------------------------------------------------------------------

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

class VisualSongFrmApp:public wxApp
{
public:
	bool OnInit();
	int OnExit();
};

 
