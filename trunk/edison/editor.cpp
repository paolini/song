#include "frame.h"
#include "editor.h"
#include <iostream>


BEGIN_EVENT_TABLE(MyEditor, wxTextCtrl)
  EVT_TEXT(-1,MyEditor::OnText)
  EVT_CHAR(MyEditor::OnChar)
END_EVENT_TABLE()

MyEditor::MyEditor(wxWindow *parent, MyFrame *f, const wxString &content)
  :wxTextCtrl(parent,-1,content,wxDefaultPosition,
	      wxDefaultSize, wxTE_MULTILINE|wxTE_RICH),
   freeze(0)
{
  std::cerr<<"MyEditor: content="<<GetValue().size()<<"\n";
 frame=f;
};

void MyEditor::OnText(wxCommandEvent& event) {
  std::cerr<<"OnText: [size: "<<event.GetString().size()<<"] "<<event.GetInt()<<"\n";
  OnTextVoid();
}

void MyEditor::OnTextVoid() {
  if (freeze) {
    //  std::cerr<<"OnText freezed \n";
    return;
  }
  //  std::cerr<<"OnTextVoid: content="<<GetValue().size()<<"\n";
  //  std::cerr<<"ONText\n";
  /*  frame->modified=true;
      frame->saved=false;*/
  frame->list->CurrentFile()->setContent(GetValue());
  //  frame->list->Update();
  //  frame->resetTitle(); //lo dovrebbe fare Update
};

void MyEditor::Set(const wxString &val) {
  freeze++;
  std::cerr<<"MyEditor::Set\n";
  SetValue(val);
  freeze--;
};

static struct {
  int code;
  char *name;
} st_unicode_name[]={
    {0x00C0, "Agrave"},
    {0x00C1, "Aacute"},
    {0x00C2, "Acircumflex"},
    {0x00C3, "Atilde"},
    {0x00C4, "Adieresis"},
    {0x00C5, "Aring"},
    {0x00C6, "AE"},
    {0x00C7, "Ccedilla"},
    {0x00C8, "Egrave"},
    {0x00C9, "Eacute"},
    {0x00CA, "Ecircumflex"},
    {0x00CB, "Edieresis"},
    {0x00CC, "Igrave"},
    {0x00CD, "Iacute"},
    {0x00CE, "Icircumflex"},
    {0x00CF, "Idieresis"},
    {0x00D0, "Eth"},
    {0x00D1, "Ntilde"},
    {0x00D2, "Ograve"},
    {0x00D3, "Oacute"},
    {0x00D4, "Ocircumflex"},
    {0x00D5, "Otilde"},
    {0x00D6, "Odieresis"},
    {0x00D7, "multiply"},
    {0x00D8, "Oslash"},
    {0x00D9, "Ugrave"},
    {0x00DA, "Uacute"},
    {0x00DB, "Ucircumflex"},
    {0x00DC, "Udieresis"},
    {0x00DD, "Yacute"},
    {0x00DE, "Thorn"},
    {0x00DF, "germandbls"},
    {0x00E0, "agrave"},
    {0x00E1, "aacute"},
    {0x00E2, "acircumflex"},
    {0x00E3, "atilde"},
    {0x00E4, "adieresis"},
    {0x00E5, "aring"},
    {0x00E6, "ae"},
    {0x00E7, "ccedilla"},
    {0x00E8, "egrave"},
    {0x00E9, "eacute"},
    {0x00EA, "ecircumflex"},
    {0x00EB, "edieresis"},
    {0x00EC, "igrave"},
    {0x00ED, "iacute"},
    {0x00EE, "icircumflex"},
    {0x00EF, "idieresis"},
    {0x00F0, "eth"},
    {0x00F1, "ntilde"},
    {0x00F2, "ograve"},
    {0x00F3, "oacute"},
    {0x00F4, "ocircumflex"},
    {0x00F5, "otilde"},
    {0x00F6, "odieresis"},
    {0x00F7, "divide"},
    {0x00F8, "oslash"},
    {0x00F9, "ugrave"},
    {0x00FA, "uacute"},
    {0x00FB, "ucircumflex"},
    {0x00FC, "udieresis"},
    {0x00FD, "yacute"},
    {0x00FE, "thorn"},
    {0x00FF, "ydieresis"},
    {0x0100, "Amacron"},
    {0x0101, "amacron"},
    {0x0102, "Abreve"},
    {0x0103, "abreve"},
    {0x0104, "Aogonek"},
    {0x0105, "aogonek"},
    {0x0106, "Cacute"},
    {0x0107, "cacute"},
    {0x0108, "Ccircumflex"},
    {0x0109, "ccircumflex"},
    {0x010A, "Cdotaccent"},
    {0x010B, "cdotaccent"},
    {0x010C, "Ccaron"},
    {0x010D, "ccaron"},
    {0x010E, "Dcaron"},
    {0x010F, "dcaron"},
    {0x0110, "Dcroat"},
    {0x0111, "dcroat"},
    {0x0112, "Emacron"},
    {0x0113, "emacron"},
    {0x0114, "Ebreve"},
    {0x0115, "ebreve"},
    {0x0116, "Edotaccent"},
    {0x0117, "edotaccent"},
    {0x0118, "Eogonek"},
    {0x0119, "eogonek"},
    {0x011A, "Ecaron"},
    {0x011B, "ecaron"},
    {0x011C, "Gcircumflex"},
    {0x011D, "gcircumflex"},
    {0x011E, "Gbreve"},
    {0x011F, "gbreve"},
    {0x0120, "Gdotaccent"},
    {0x0121, "gdotaccent"},
    {0x0122, "Gcommaaccent"},
    {0x0123, "gcommaaccent"},
    {0x0124, "Hcircumflex"},
    {0x0125, "hcircumflex"},
    {0x0126, "Hbar"},
    {0x0127, "hbar"},
    {0x0128, "Itilde"},
    {0x0129, "itilde"},
    {0x012A, "Imacron"},
    {0x012B, "imacron"},
    {0x012C, "Ibreve"},
    {0x012D, "ibreve"},
    {0x012E, "Iogonek"},
    {0x012F, "iogonek"},
    {0x0130, "Idotaccent"},
    {0x0131, "dotlessi"},
    {0x0132, "IJ"},
    {0x0133, "ij"},
    {0x0134, "Jcircumflex"},
    {0x0135, "jcircumflex"},
    {0x0136, "Kcommaaccent"},
    {0x0137, "kcommaaccent"},
    {0x0138, "kgreenlandic"},
    {0x0139, "Lacute"},
    {0x013A, "lacute"},
    {0x013B, "Lcommaaccent"},
    {0x013C, "lcommaaccent"},
    {0x013D, "Lcaron"},
    {0x013E, "lcaron"},
    {0x013F, "Ldot"},
    {0x0140, "ldot"},
    {0x0141, "Lslash"},
    {0x0142, "lslash"},
    {0x0143, "Nacute"},
    {0x0144, "nacute"},
    {0x0145, "Ncommaaccent"},
    {0x0146, "ncommaaccent"},
    {0x0147, "Ncaron"},
    {0x0148, "ncaron"},
    {0x0149, "napostrophe"},
    {0x014A, "Eng"},
    {0x014B, "eng"},
    {0x014C, "Omacron"},
    {0x014D, "omacron"},
    {0x014E, "Obreve"},
    {0x014F, "obreve"},
    {0x0150, "Ohungarumlaut"},
    {0x0151, "ohungarumlaut"},
    {0x0152, "OE"},
    {0x0153, "oe"},
    {0x0154, "Racute"},
    {0x0155, "racute"},
    {0x0156, "Rcommaaccent"},
    {0x0157, "rcommaaccent"},
    {0x0158, "Rcaron"},
    {0x0159, "rcaron"},
    {0x015A, "Sacute"},
    {0x015B, "sacute"},
    {0x015C, "Scircumflex"},
    {0x015D, "scircumflex"},
    {0x015E, "Scedilla"},
    {0x015F, "scedilla"},
    {0x0160, "Scaron"},
    {0x0161, "scaron"},
    {0x0162, "Tcommaaccent"},
    {0x0163, "tcommaaccent"},
    {0x0164, "Tcaron"},
    {0x0165, "tcaron"},
    {0x0166, "Tbar"},
    {0x0167, "tbar"},
    {0x0168, "Utilde"},
    {0x0169, "utilde"},
    {0x016A, "Umacron"},
    {0x016B, "umacron"},
    {0x016C, "Ubreve"},
    {0x016D, "ubreve"},
    {0x016E, "Uring"},
    {0x016F, "uring"},
    {0x0170, "Uhungarumlaut"},
    {0x0171, "uhungarumlaut"},
    {0x0172, "Uogonek"},
    {0x0173, "uogonek"},
    {0x0174, "Wcircumflex"},
    {0x0175, "wcircumflex"},
    {0x0176, "Ycircumflex"},
    {0x0177, "ycircumflex"},
    {0x0178, "Ydieresis"},
    {0x0179, "Zacute"},
    {0x017A, "zacute"},
    {0x017B, "Zdotaccent"},
    {0x017C, "zdotaccent"},
    {0x017D, "Zcaron"},
    {0x017E, "zcaron"},
    {0x017F, "longs"},
    {0x0192, "florin"},
    {0x01A0, "Ohorn"},
    {0x01A1, "ohorn"},
    {0x01AF, "Uhorn"},
    {0x01B0, "uhorn"},
    {0x01E6, "Gcaron"},
    {0x01E7, "gcaron"},
    {0x01FA, "Aringacute"},
    {0x01FB, "aringacute"},
    {0x01FC, "AEacute"},
    {0x01FD, "aeacute"},
    {0x01FE, "Oslashacute"},
    {0x01FF, "oslashacute"},
    {0x0218, "Scommaaccent"},
    {0x0219, "scommaaccent"},
    {0x021A, "Tcommaaccent"},
    {0x021B, "tcommaaccent"},
    {0x02BC, "afii57929"},
    {0x02BD, "afii64937"},
    {0x02C6, "circumflex"},
    {0x02C7, "caron"},
    {0x02C9, "macron"},
    {0x02D8, "breve"},
    {0x02D9, "dotaccent"},
    {0x02DA, "ring"},
    {0x02DB, "ogonek"},
    {0x02DC, "tilde"},
    {0x02DD, "hungarumlaut"},
    {0x0300, "gravecomb"},
    {0x0301, "acutecomb"},
    {0x0303, "tildecomb"},
    {0x0309, "hookabovecomb"},
    {0x0323, "dotbelowcomb"},
    {0x0384, "tonos"},
    {0x0385, "dieresistonos"},
    {0x0386, "Alphatonos"},
    {0x0387, "anoteleia"},
    {0x0388, "Epsilontonos"},
    {0x0389, "Etatonos"},
    {0x038A, "Iotatonos"},
    {0x038C, "Omicrontonos"},
    {0x038E, "Upsilontonos"},
    {0x038F, "Omegatonos"},
    {0x0390, "iotadieresistonos"},
    {0x0391, "Alpha"},
    {0x0392, "Beta"},
    {0x0393, "Gamma"},
    {0x0394, "Delta"},
    {0x0395, "Epsilon"},
    {0x0396, "Zeta"},
    {0x0397, "Eta"},
    {0x0398, "Theta"},
    {0x0399, "Iota"},
    {0x039A, "Kappa"},
    {0x039B, "Lambda"},
    {0x039C, "Mu"},
    {0x039D, "Nu"},
    {0x039E, "Xi"},
    {0x039F, "Omicron"},
    {0x03A0, "Pi"},
    {0x03A1, "Rho"},
    {0x03A3, "Sigma"},
    {0x03A4, "Tau"},
    {0x03A5, "Upsilon"},
    {0x03A6, "Phi"},
    {0x03A7, "Chi"},
    {0x03A8, "Psi"},
    {0x03A9, "Omega"},
    {0x03AA, "Iotadieresis"},
    {0x03AB, "Upsilondieresis"},
    {0x03AC, "alphatonos"},
    {0x03AD, "epsilontonos"},
    {0x03AE, "etatonos"},
    {0x03AF, "iotatonos"},
    {0x03B0, "upsilondieresistonos"},
    {0x03B1, "alpha"},
    {0x03B2, "beta"},
    {0x03B3, "gamma"},
    {0x03B4, "delta"},
    {0x03B5, "epsilon"},
    {0x03B6, "zeta"},
    {0x03B7, "eta"},
    {0x03B8, "theta"},
    {0x03B9, "iota"},
    {0x03BA, "kappa"},
    {0x03BB, "lambda"},
    {0x03BC, "mu"},
    {0x03BD, "nu"},
    {0x03BE, "xi"},
    {0x03BF, "omicron"},
    {0x03C0, "pi"},
    {0x03C1, "rho"},
    {0x03C2, "sigma1"},
    {0x03C3, "sigma"},
    {0x03C4, "tau"},
    {0x03C5, "upsilon"},
    {0x03C6, "phi"},
    {0x03C7, "chi"},
    {0x03C8, "psi"},
    {0x03C9, "omega"},
    {0x03CA, "iotadieresis"},
    {0x03CB, "upsilondieresis"},
    {0x03CC, "omicrontonos"},
    {0x03CD, "upsilontonos"},
    {0x03CE, "omegatonos"},
    {0x03D1, "theta1"},
    {0x03D2, "Upsilon1"},
    {0x03D5, "phi1"},
    {0x03D6, "omega1"},
    {0,0}
};


static wxString string2sng(const wxString &s) {
  wxString result;
  for (size_t i=0;i<s.size();++i) {
    int c=(unsigned char)s[i];
    if (c<128) result+=s[i];
    else {
      int i;
      for (i=0;
	   st_unicode_name[i].code && st_unicode_name[i].code!=c;++i);
      if (st_unicode_name[i].code) {
	char *name=st_unicode_name[i].name;
	if (!strcmp(name+1,"grave")) result+="\\`";
	else if (!strcmp(name+1,"acute")) result+="\\'";
	else if (!strcmp(name+1,"circumflex")) result+="\\^";
	else if (!strcmp(name+1,"tilde")) result+="\\~";
	else if (!strcmp(name+1,"dieresis")) result+="\\\"";
	else if (!strcmp(name+1,"cedilla")) result+="\\,";
	else if (!strcmp(name+1,"caron")) result+="\\<";
	result+=name[0];
      }
      else
	result+="?";
    };
  }
return result;
}

void MyEditor::OnChar(wxKeyEvent& event) {
  int c=event.GetKeyCode();
  std::cerr<<"OnChar() c="<<c<<"\n";
  if (c<128 || c>256) 
    event.Skip();
  else {
    wxString s;
    s+=c;
    WriteText(string2sng(s));
    OnTextVoid();
  }
};

bool MyEditor::InsertHeader() {
  std::cerr<<"MyEditor::InsertHeader\n";
  wxDialog* frame=new wxDialog(this,-1,wxString("Insert Song Headers"));
  wxFlexGridSizer *grid= new wxFlexGridSizer(2,3,5);
  grid->AddGrowableCol(1);
  grid->Add(new wxStaticText(frame,-1,_T("title:")),0,
	    wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
  wxTextCtrl *title=new wxTextCtrl(frame,-1);
  grid->Add(title,0,wxGROW);
  grid->Add(new wxStaticText(frame,-1,_T("author:")),0,
	    wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
  
  wxFlexGridSizer* author=new wxFlexGridSizer(2,2);
  author->AddGrowableCol(0);
  author->AddGrowableCol(1);
  wxTextCtrl *firstName=new wxTextCtrl(frame,-1);
  wxTextCtrl *name=new wxTextCtrl(frame,-1);
  author->Add(firstName,0,wxGROW);
  author->Add(name,0,wxGROW);
  author->Add(new wxStaticText(frame,-1,_T("first name")));
  author->Add(new wxStaticText(frame,-1,_T("name")));
  grid->Add(author);
  grid->Add(new wxBoxSizer(wxHORIZONTAL));
  wxBoxSizer *buttons=new wxBoxSizer(wxHORIZONTAL);
  buttons->Add(new wxButton(frame,wxID_OK,"OK"));
  buttons->Add(new wxButton(frame,wxID_CANCEL,"Cancel"));
  grid->Add(buttons);
  frame->SetSizer(grid);
  if (frame->ShowModal()==wxID_OK) {
    std::cerr<<"OK!\n";
    WriteText("\\title "+string2sng(title->GetValue())+"\n");
    WriteText("\\author "+string2sng(firstName->GetValue())
	      +" "+string2sng((name->GetValue()).Upper())+"\n\n");
    frame->Destroy();
    OnTextVoid();
    return true;
  } 
  frame->Destroy();
  SetFocus();
  return false;
};

bool MyEditor::InsertStanza() {
  std::cerr<<"MyEditor::InsertStanza\n";
  wxDialog* frame=new wxDialog(this,-1,wxString("Insert Song Stanza"));
  wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *type=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *buttons=new wxBoxSizer(wxHORIZONTAL);
  type->Add(new wxStaticText(frame,-1,_T("type: ")));
  wxString types[4]={"strophe","refrain","talking","tablature"};
  wxChoice *choice=new wxChoice(frame,-1,wxDefaultPosition,
				wxDefaultSize,4,types);
  type->Add(choice);
  sizer->Add(type);
  buttons->Add(new wxButton(frame,wxID_OK,"OK"));
  buttons->Add(new wxButton(frame,-1,"Cancel"));
  sizer->Add(buttons);
  frame->SetSizer(sizer);
  if (frame->ShowModal()==wxID_OK) {
    std::cerr<<"OK!\n";
    switch(choice->GetSelection()) {
    case -1:
    case 0:
      WriteText("\\s\n");
      break;
    case 1:
      WriteText("\\r\n");
      break;
    case 2:
      WriteText("\\k\n");
    case 3:
      WriteText("\\b\n");
    };
    frame->Destroy();
    OnTextVoid();
    return true;
  };
  frame->Destroy();
  SetFocus();
  return false;
};
