//---------------------------------------------------------------------------
//
// Name:        VisualSongApp.cpp
// Author:      Matteo
// Created:     04/09/2005 10.06.35
//
//---------------------------------------------------------------------------

#include "VisualSongApp.h"
#include "VisualSongFrm.h"

IMPLEMENT_APP(VisualSongFrmApp)

bool VisualSongFrmApp::OnInit()
{
    VisualSongFrm *myFrame = new  VisualSongFrm(NULL);
    SetTopWindow(myFrame);
    myFrame->Show(TRUE);		
    return TRUE;
}
 
int VisualSongFrmApp::OnExit()
{
	return 0;
}
