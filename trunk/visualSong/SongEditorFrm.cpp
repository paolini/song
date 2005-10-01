//---------------------------------------------------------------------------
//
// Name:        SongEditorFrm.cpp
// Author:      Matteo
// Created:     05/09/2005 11.53.30
//
//---------------------------------------------------------------------------

#include "SongEditorFrm.h"
#include <wx/textfile.h>
#include <wx/file.h>
//Do not add custom headers.
//wx-dvcpp designer will remove them
////Header Include Start
////Header Include End


//----------------------------------------------------------------------------
// SongEditorFrm
//----------------------------------------------------------------------------
     //Add Custom Events only in the appropriate Block.
    // Code added in  other places will be removed by wx-dvcpp 
    ////Event Table Start
BEGIN_EVENT_TABLE(SongEditorFrm,wxFrame)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(SongEditorFrm::SongEditorFrmClose)
	EVT_MENU(ID_MNU_OPENFILE_1006, SongEditorFrm::Mnuopenfile1006Click)
	EVT_MENU(ID_MNU_SAVEFILE_1008, SongEditorFrm::Mnusavefile1008Click)
	EVT_MENU(ID_MNU_NEWFILE_1009, SongEditorFrm::Mnunewfile1009Click)
	EVT_BUTTON(ID_WXBUTTON1,SongEditorFrm::WxButton1Click)
END_EVENT_TABLE()
    ////Event Table End



SongEditorFrm::SongEditorFrm( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style )
    : wxFrame( parent, id, title, position, size, style)
{
    CreateGUIControls();
}

SongEditorFrm::~SongEditorFrm()
{
    
} 

void SongEditorFrm::CreateGUIControls(void)
{
    //Do not add custom Code here
    //wx-devcpp designer will remove them.
    //Add the custom code before or after the Blocks
    ////GUI Items Creation Start

	WxPanel1 = new wxPanel(this, ID_WXPANEL1, wxPoint(0,0), wxSize(613,516));

	WxMemo1 = new wxTextCtrl(WxPanel1, ID_WXMEMO1, wxT(""), wxPoint(21,60), wxSize(578,424), wxTE_MULTILINE, wxDefaultValidator, wxT("WxMemo1"));
	WxMemo1->SetMaxLength(0);
	WxMemo1->AppendText(wxT(""));
	WxMemo1->SetFont(wxFont(10, wxSWISS, wxNORMAL,wxNORMAL, FALSE));

	WxButton1 = new wxButton(WxPanel1, ID_WXBUTTON1, wxT("Check Syntax"), wxPoint(21,12), wxSize(94,34), 0, wxDefaultValidator, wxT("WxButton1"));

	WxMemo2 = new wxTextCtrl(WxPanel1, ID_WXMEMO2, wxT(""), wxPoint(132,14), wxSize(464,32), wxTE_MULTILINE, wxDefaultValidator, wxT("WxMemo2"));
	WxMemo2->SetMaxLength(0);

	WxOpenFileDialog1 =  new wxFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), "*.*", wxOPEN);

	WxMenuBar1 =  new wxMenuBar();
	wxMenu *ID_MNU_FILE_1001_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(ID_MNU_FILE_1001_Mnu_Obj, wxT("File"));
	
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_OPENFILE_1006, wxT("Open File"), wxT(""), wxITEM_NORMAL);
	
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_SAVEFILE_1008, wxT("Save File"), wxT(""), wxITEM_NORMAL);
	
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_NEWFILE_1009, wxT("New File"), wxT(""), wxITEM_NORMAL);
	
	
	this->SetMenuBar(WxMenuBar1);

	WxSaveFileDialog1 =  new wxFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), "*.*", wxSAVE);

	this->SetSize(8,0,621,550);
	this->SetTitle(wxT("Song Editor"));
	this->Center();
	this->SetIcon(wxNullIcon);
	
    ////GUI Items Creation End
}

void SongEditorFrm::SongEditorFrmClose(wxCloseEvent& event)
{
    // --> Don't use Close with a Frame,
    // use Destroy instead.
    Destroy();
}
 

/*
 * Mnuopenfile1006Click
 */
void SongEditorFrm::Mnuopenfile1006Click(wxCommandEvent& event)
{
    wxString fname;
    wxString directory;
    wxString str;


	WxOpenFileDialog1->ShowModal();
	fname = WxOpenFileDialog1->GetFilename();
	directory = WxOpenFileDialog1->GetDirectory();
	fname = directory + "\\" + fname;
	WxMemo1->LoadFile(fname);
}

/*
 * Mnusavefile1007Click
 */
void SongEditorFrm::Mnusavefile1007Click(wxCommandEvent& event)
{
	// insert your code here
	WxSaveFileDialog1->ShowModal();
}

/*
 * Mnusavefile1008Click
 */
void SongEditorFrm::Mnusavefile1008Click(wxCommandEvent& event)
{
	// insert your code here
    wxString fname;
    wxString directory;
    wxString str;
    wxTextFile *file;

	WxSaveFileDialog1->ShowModal();
	fname = WxSaveFileDialog1->GetFilename();
	directory = WxSaveFileDialog1->GetDirectory();
	fname = directory + "\\" + fname;
	file = new wxTextFile(fname);
	if(!file->Exists())
	{
      file->Create();
    }
	WxMemo1->SaveFile(fname);
	delete file;
}

/*
 * WxButton1Click
 */

#ifndef PATH_TO_SONG_EXE

/// Qui bisogna mettere il PATH corretto del file SNG2PS.EXE:
#define PATH_TO_SONG_EXE "C:\\song\\sng2ps.exe"
#endif
void SongEditorFrm::WxButton1Click(wxCommandEvent& event)
{
  // il codice viene eseguito quando viene schiacciato il bottone check syntax

  // per matteo:
  // ho bisogno di avere su file il testo da controllare
  // se l'utente non ha ancora salvato il file si potrebbe chiedergli
  // di salvarlo, prima di fare il CHECK.
  // Se invece vuoi dare la possibilita` di controllare il testo
  // senza doverlo salvare, bisogna utilizzare un file temporaneo.

  // !!!!
  // qui bisogna inserire il nome del file da controllare
  wxString filename="metti_qui_il_nome_del_file.sng"; 

  wxString tmp_file_out="visualSongOut.tmp";
  wxString tmp_file_err="visualSongErr.tmp";
  wxString cmd;
  wxString result;
  
  cmd="echo 2 | " PATH_TO_SONG_EXE " " + filename 
    + " -output " + tmp_file_out
    + " -pag 1 > "+tmp_file_err;
  std::cerr<<"system("<<cmd<<")\n"; // DEBUG
  int ok=system(cmd);
  if (ok) {
    FILE *fp=fopen(tmp_file_err,"rt");
    if (fp) {
      char line[4096];
      while(fgets(line,sizeof(line),fp)) {
	char *p=line;
	while (isspace(*p)) p++;
	if (p[0]!='1' && p[0]!='2')
	  result+=p;
      }
      fclose(fp);
    } else result="cannot open temp file "+tmp_file_err;
    //    remove(tmp_file_err);
    //remove(tmp_file_out);
  } else {
    result="non riesco ad eseguire il comando "+cmd;
  }

  // Qui scrive correttamente il risultato:
  std::cerr<<"result="<<result<<"\n";  //DEBUG

  // per scrivere il risultato usa WxMemo2->SetLabel("bla bla bla");
  // Non funziona!!!!
  WxMemo2->SetLabel(result);
}

/*
 * Mnunewfile1009Click
 */
void SongEditorFrm::Mnunewfile1009Click(wxCommandEvent& event)
{
	// insert your code here
	WxMemo1->SetLabel("");
}
