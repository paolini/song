#include "list.h"
#include "frame.h"
#include "editor.h"
#include <wx/ffile.h>
#include <wx/listctrl.h>
#include <wx/filename.h>
#include <wx/listbox.h>
#include "corda/plug.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>


void FileItem::reset() {
  filename=wxString(); // file vuoto
  content=wxString();
  plug="unknown"; // default
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
  std::cerr<<"reload() filename='"<<filename<<"'\n";
  if (filename==wxString()) {
    plug="sng"; // default
    compiled.clear();
    compiled_valid=true;
  } else {
    plug=wxFileName(filename).GetExt();
    wxFFile file(filename.c_str(),"r");
    if (!file.ReadAll(&content)) {
      wxMessageBox("error reading file "+filename,
		   "error", wxOK|wxICON_INFORMATION);
    }
    std::cerr<<"reload content="<<content.size()<<"\n";
    modified=false;
    compiled.clear(); // cancella l'eventuale vecchio contenuto
    compiled_valid=false;
  }
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
  if (compiled_valid) return;
  std::cerr<<"compile() filename='"<<filename<<"' content='"<<content<<"'\n";
  compiled.clear();
  //  try {
  Plugin* reader=Plugin::Construct(plug.c_str());
    if (!reader) {
      throw std::runtime_error("cannot find '"+std::string(plug)+"' reader");
    }
    
    std::string buf=getContent().c_str();
    std::stringstream in(buf);
    
    compiled_valid=true;
    reader->Read(in,compiled);
    
    if (compiled.size()==0) {
      // wxMessageBox("No song in file","warning", wxOK|wxICON_INFORMATION);
      std::cerr<<"no song in file\n";
    }
};

const Song *FileItem::getSong(unsigned int n) const {
  compile();
  if (n>=compiled.size()) return 0;
  return compiled[n];
};

int FileItem::nSongs() const {
  compile();
  return compiled.size();
};

const SongList &FileItem::getList() const {
  compile();
  return compiled;
};

wxString Item::Title() const {
  const Song* song=getSong();
  if (song && song->head()) return wxString(iso(song->head()->title).c_str());
  return wxString("--no title--");
};

enum {MYLIST=1010};

BEGIN_EVENT_TABLE(MyList, wxListBox /*wxListCtrl*/ )
  EVT_LISTBOX(MYLIST,MyList::OnSelect)  

END_EVENT_TABLE()

MyList::MyList(wxWindow *parent, MyFrame* fr):
  //  wxListCtrl(
  wxListBox(
            parent,MYLIST,wxDefaultPosition,wxDefaultSize,
	    0)
{
  n=0;
  frame=fr;
};

void MyList::Update() {
  Clear();
  for (size_t i=0;i<songs.size();++i) {
    wxString name;
    const Song *song=songs[i].getSong();
    if (song==0) { // la canzone non c'e` piu`!
      songs.erase(songs.begin()+i);
      --i;
      continue;
    }
    name=songs[i].file->FileName();
    if (name==wxString()) name="-- new file --";
    name+=": ";
    name+=songs[i].Title();
    Append(name);
  }
  frame->resetTitle();
  frame->editor->Set(songs[n].file->getContent());
  frame->editor->Enable(true);
  frame->Refresh();
};

void MyList::Load(const wxString &filename) {
  FileItem *it=new FileItem;
  it->Load(filename);
  for (int i=0;i<it->nSongs();++i)
    songs.push_back(Item(it,i));
  //SetItemState(n,0,wxLIST_STATE_SELECTED);
  SetSelection(n,0);
  n=songs.size()-1;
  //  ProcessEvent(wxListEvent(wxEVT_LIST_ITEM_SELECTED,n));
  //  SetItemState(n,wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
  SetSelection(n,1);
  Update();
};

void MyList::Save() {
  if (songs.size()==0 || songs[n].file->FileName()==wxString()) {
    throw std::runtime_error("no filename specified");
  }
  songs[n].file->Save();
  Update();
};

void MyList::SaveAs(const wxString &name) {
  std::cerr<<"MyList::SaveAs "<<name<<"\n";
  if (songs.size()==0) throw std::runtime_error("no file");
  assert(n<songs.size());
  songs[n].file->SaveAs(name);
  Update();
};

FileItem *MyList::CurrentFile() {
  if (songs.size()==0) return 0;
  return songs[n].file;
};

Item *MyList::CurrentItem() {
  if (songs.size()==0) return 0;
  return &songs[n];
};

void MyList::OnSelect(wxCommandEvent &event) {
  n=event.GetInt();
  assert(n>=0);
  frame->resetTitle();
  frame->editor->Set(CurrentFile()->getContent());
  std::cerr<<"OnSelect "<<n<<"\n";
};

void MyList::Export(const wxString &filename, const wxString &plug) {
  Plugout *writer=Plugout::Construct(plug.c_str());
  PlugoutOptions opt;
  if (!writer) throw std::runtime_error(("Cannot initialize "+plug+" plug").c_str());
  SongCollection list;
  for (size_t i=0;i<songs.size();++i) {
    const SongList &file=songs[i].file->getList();
    for (size_t j=0;j<file.size();++j) {
      list.push_back(file[j]);
      //      std::cerr<<"push_back("<<file[j]<<")\n";
    }
  }
  for (size_t i=0;i<list.size();++i) {
    //    std::cerr<<"Title: "<<list[i]->head()->title<<"\n";
  }
  std::cerr<<"MyList::Export "<<list.size()<<" songs for "
	   <<plug<<" on file "<<filename<<"\n";
  assert(writer);
  writer->Write(filename.c_str(),list,opt);
  //  std::cerr<<"MyList: wrote!\n";
};
