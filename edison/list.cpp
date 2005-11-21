#include "list.h"
#include "frame.h"
#include <wx/ffile.h>
#include "plug.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>

void FileItem::reset() {
  filename=wxString();
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
  file.Write(getContent());
  modified=false;
};

void FileItem::reload() {
  std::cerr<<"reload() filename="<<filename<<"\n";
  if (filename==wxString()) {
    content=wxString();
  } else {
    wxFFile file(filename.c_str(),"r");
    if (!file.ReadAll(&content)) {
      wxMessageBox("error reading file "+filename,
		   "error", wxOK|wxICON_INFORMATION);
    }
    std::cerr<<"reload content="<<content.size()<<"\n";
  }
  compiled.clear(); // cancella l'eventuale vecchio contenuto
  compiled_valid=false;
  modified=false;
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

MyList::MyList(wxWindow *parent, MyFrame* fr):
  wxListCtrl(parent,-1,wxDefaultPosition,wxDefaultSize,
	     0)
{
  InsertItem(0,"line 0");
};
