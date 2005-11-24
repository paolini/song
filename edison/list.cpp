#include "list.h"
#include "frame.h"
#include "editor.h"
#include <wx/ffile.h>
#include <wx/listctrl.h>
#include "corda/plug.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>


void FileItem::reset() {
  filename=wxString(); // file vuoto
  content=wxString();
  modified=false;
  compiled.clear();
  compiled_valid=true;  
};

FileItem::FileItem() {
  reset();
  assert(compiled.size()==0);
};

void FileItem::Load(const wxString &file) {
  filename=file;
  reload();
};

void FileItem::Save() {
  if (filename==wxString()) throw std::runtime_error("no filename specified");
  wxFFile file(filename.c_str(),"w");
  // come si controlla se il file e` aperto?
  //  if (file) throw runtime_error(("cannot write file "+filename).c_str());
  file.Write(getContent());
  modified=false;
};

void FileItem::SaveAs(const wxString &newname) {
  std::cerr<<"FileItem::SaveAs "<<newname<<"\n";
  modified=true;
  filename=newname;
  Save();
};

void FileItem::reload() {
  std::cerr<<"reload() filename="<<filename<<"\n";
  if (filename==wxString()) {
    content="\\t Titolo\n\\a FirstName NAME\n\n\\s\n";
    modified=true;
  } else {
    wxFFile file(filename.c_str(),"r");
    if (!file.ReadAll(&content)) {
      wxMessageBox("error reading file "+filename,
		   "error", wxOK|wxICON_INFORMATION);
    }
    std::cerr<<"reload content="<<content.size()<<"\n";
    modified=false;
  }
  compiled.clear(); // cancella l'eventuale vecchio contenuto
  compiled_valid=false;
};

const wxString &FileItem::getContent() const {
  std::cerr<<"getContent() ="<<content.size()<<"\n";
  return content;
};

void FileItem::setContent(const wxString &val) {
  std::cerr<<"setContent() = "<<val.size()<<"\n";
  modified=true;
  compiled_valid=false;
  content=val;
};

const wxString &FileItem::FileName() const {
  return filename;
};

void FileItem::compile() const {
  std::cerr<<"compile()\n";
  if (compiled_valid) return;
  compiled.clear();
  try {
    // visualizza
    Plugin* reader=Plugin::Construct("sng");
    if (!reader) {
      throw std::runtime_error("cannot create SNG reader");
    }
    
    std::string buf=getContent().c_str();
    std::stringstream in(buf);
    
    reader->Read(in,compiled);
    
    if (compiled.size()==0) {
      // wxMessageBox("No song in file","warning", wxOK|wxICON_INFORMATION);
      std::cerr<<"no song in file\n";
    }
  } catch(std::runtime_error &e) {
    wxMessageBox(e.what(), "error", wxOK|wxICON_INFORMATION);
    //    cerr<<e.what()<<"\n";
    //    Close(TRUE);
    //    reset();
  }
  compiled_valid=true;
};

const Song *FileItem::getSong(unsigned int n) const {
  compile();
  if (n>=compiled.size()) return 0;
  return compiled[n];
};

BEGIN_EVENT_TABLE(MyList, wxListCtrl)
  EVT_LIST_ITEM_SELECTED(-1,MyList::OnSelect)  
END_EVENT_TABLE()

MyList::MyList(wxWindow *parent, MyFrame* fr):
  wxListCtrl(parent,-1,wxDefaultPosition,wxDefaultSize,
	     0)
{
  n=0;
  frame=fr;
};

void MyList::Load(const wxString &filename) {
  FileItem *it=new FileItem;
  it->Load(filename);
  songs.push_back(it);
  n=songs.size()-1;
  InsertItem(n,songs[n].file->FileName()
	     /*+": "+songs[n]->getSong()->head()->title*/);
  
  //  ProcessEvent(wxListEvent(wxEVT_LIST_ITEM_SELECTED,n));
  frame->editor->Set(songs[n].file->getContent());
  frame->resetTitle();
  frame->Refresh();
};

void MyList::Save() {
  if (songs.size()==0 || songs[n].file->FileName()==wxString()) {
    throw std::runtime_error("no filename specified");
  }
  songs[n].file->Save();
  frame->resetTitle();
};

void MyList::SaveAs(const wxString &name) {
  std::cerr<<"MyList::SaveAs "<<name<<"\n";
  if (songs.size()==0) throw std::runtime_error("no file");
  assert(n<songs.size());
  songs[n].file->SaveAs(name);
  frame->resetTitle();
};

FileItem *MyList::CurrentFile() {
  if (songs.size()==0) return 0;
  return songs[n].file;
};

void MyList::OnSelect(wxListEvent &event) {
  n=event.GetIndex();
  frame->resetTitle();
  frame->editor->Set(CurrentFile()->getContent());
  std::cerr<<"OnSelect "<<event.GetIndex()<<"\n";
};
