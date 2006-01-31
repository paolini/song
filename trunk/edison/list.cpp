#include "list.h"
#include "frame.h"
#include "editor.h"
#include <wx/ffile.h>
#include <wx/listctrl.h>
#include <wx/filename.h>
#include <wx/listbox.h>
#include "corda/plug.h"
#include "corda/iso.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>


void FileItem::reset() {
  filename=wxString(); // file nuovo, senza nome
  content=wxString(); // file vuoto
  plug="sng"; // default
  modified=false;
  compiled.clear();
  status=COMPILED_EMPTY;  
};

FileItem::FileItem(MyFrame *the_frame): frame(the_frame)
{
  wxMutexLocker lock(mutex);
  reset();
  assert(compiled.size()==0);
};

void FileItem::reload() {
  std::cerr<<"reload() filename='"<<filename<<"'\n";
  if (filename==wxString()) {
    reset();
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
    status=NOT_COMPILED;
  }
};

void FileItem::Load(const wxString &file) {
  wxMutexLocker lock(mutex);  
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


const wxString &FileItem::getContent() const {
  std::cerr<<"getContent() ="<<content.size()<<"\n";
  return content;
};

void FileItem::setContent(const wxString &val) {
  std::cerr<<"setContent() = "<<val.size()<<"\n";
  modified=true;
  status=NOT_COMPILED;
  content=val;
};

const wxString &FileItem::FileName() const {
  return filename;
};

class Lock {
  int &freeze;
public:
  Lock(int &x): freeze(x) {freeze++;};
  ~Lock() {freeze--;};
};

void FileItem::compile() {
  assert(frame);
  Lock lock(frame->editor->freeze);
  if (status!=NOT_COMPILED) return;
  wxMutexLocker mutex_lock(mutex);  
  static int count=0;
  Lock dummy(count);
  std::cerr<<"FileItem::compile() count="<<count<<"\n";
  


  std::cerr<<"compile() filename='"<<filename<<"' content size='"<<content.size()<<"'\n";
  compiled.clear();
  assert(compiled.size()==0);
  compilation_error=PlugError("Not yet compiled");
  status=COMPILED_ERROR;

  Plugin* reader=Plugin::Construct(plug.c_str());
  if (!reader) {
    std::string msg="cannot find '"+std::string(plug)+"' reader";
    compilation_error=PlugError(msg);
    throw std::runtime_error(msg);  
  }
  
  std::string buf=getContent().c_str();
  std::stringstream in(buf);
  
  try {
    assert(compiled.size()==0);
    reader->Read(in,compiled);
    std::cerr<<"compile(): read: "<<compiled.size()<<" songs\n";
    status=COMPILED_OK;
    if (compiled.size()) {
      wxString msg="Compilation: OK!";
      msg<<" ["<<compiled.size()<<" songs]";
      frame->SetStatusText(msg);
    }
    else
      frame->SetStatusText("Compilation: OK but empty");
  } catch (PlugError &e) {
    compilation_error=e;

    frame->SetStatusText(wxString(e.what()));
    // lo fa Update()
    // frame->editor->SetStyle(frame->editor->XYToPosition(0,e.line-1),
    // frame->editor->XYToPosition(0,e.line),
    // wxTextAttr(*wxWHITE,*wxRED));
  }
  if (compiled.size()==0) {
    // wxMessageBox("No song in file","warning", wxOK|wxICON_INFORMATION);
    std::cerr<<"no song in file\n";
  }
  assert(status!=NOT_COMPILED); // altrimenti va in loop!
  //  if (frame) frame->list->Update();
};

const Song *FileItem::getSong(unsigned int n) {
  compile();
  if (n>=compiled.size()) {
    std::cerr<<"request for song "<<n<<" in file "<<FileName()<<
      " which has only "<<compiled.size()<<" songs\n";
    return 0;
  }
  return compiled[n];
};

int FileItem::nSongs() {
  compile();
  return compiled.size();
};

const SongList &FileItem::getList() {
  compile();
  return compiled;
};

wxString FileItem::Title(unsigned int n) {
  const Song* song=getSong(n);
  if (song && song->head())
    return wxString(iso(song->head()->title).c_str());
  else return wxString("-- no title --");
};

/*
wxString Item::Title() const {
  const Song* song=getSong();
  if (song && song->head()) 
    return wxString(iso(song->head()->title).c_str());
  return wxString("--no title--");
};
*/

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
  nfile=0; nsong=0;
  frame=fr;
};

void MyList::OnDraw() {
  Update();
};

void MyList::Update() {
  std::cerr<<"MyList::Update()\n";
  Clear();
  assert(GetCount()==0);
  for (size_t i=0;i<files.size();++i) {
    wxString name=files[i]->FileName();
    if (name==wxString()) name+="-- new file--";
    if (files[i]->Modified()) name+=" *";
    Append(name);
    int n=files[i]->nSongs();
    std::cerr<<"Update: found "<<n<<" songs\n";
    for (size_t j=0;j<n;++j) {
      name="";
      name<<"  ["<<j<<"] "<<files[i]->Title(j);
      Append(name);
    }
  }
  frame->resetTitle();
  if (files.size()) {
    frame->editor->Set(files[nfile]->getContent());
    if (files[nfile]->status==FileItem::COMPILED_ERROR) {
      PlugError &e=files[nfile]->compilation_error;
      frame->editor->SetStyle(frame->editor->XYToPosition(0,e.line-1),
			      frame->editor->XYToPosition(0,e.line),
			      wxTextAttr(*wxWHITE,*wxRED));
    }
    frame->editor->Enable(true);
  } else {
    frame->editor->Set("--- no file loaded ---");
    frame->editor->Enable(false);
  }
  //  frame->Refresh();
};

unsigned int MyList::get_n() const {
  unsigned int count=0;
  unsigned int i,x;
  for (i=0;i<nfile && i+1<files.size();++i) {
    x=files[i]->nSongs();
    count+=x+1;
  }
  x=files[i]->nSongs();
  if (nsong<x) count+=nsong+1;
  return count;
};

void MyList::set_n(unsigned int count) {
  nsong=0;
  for (nfile=0;nfile<files.size();++nfile) {
    unsigned int x=files[nfile]->nSongs();
    x++;
    if (count<x) {
      if (count>0)
	nsong=count-1;
      else
	nsong=0;
      return;
    }
    count-=x;
  }
  if (files.size()) {
    nfile=files.size()-1;
    nsong=files[nfile]->nSongs();
    if (nsong>0) nsong--;
  }
};

void MyList::Load(const wxString &filename) {
  FileItem *it=new FileItem(frame);
  it->Load(filename);
  files.push_back(it);
  SetSelection(get_n(),0);
  nfile=files.size()-1;
  nsong=0;
  SetSelection(get_n(),1);
  Update();
};

void MyList::Save() {
  if (files.size()==0) {
    throw std::runtime_error("no file to be saved");
  }
  if (files[nfile]->FileName()==wxString()) {
    throw std::runtime_error("no filename specified");
  }
  files[nfile]->Save();
  Update();
};

void MyList::SaveAs(const wxString &name) {
  std::cerr<<"MyList::SaveAs "<<name<<"\n";
  //  if (songs.size()==0) throw std::runtime_error("no file");
  assert(nfile<files.size());
  files[nfile]->SaveAs(name);
  Update();
};

FileItem *MyList::CurrentFile() {
  if (files.size()==0) return 0;
  assert(nfile<files.size());
  return files[nfile];
};

const Song *MyList::CurrentSong() const {
  if (files.size()==0) return 0;
  if (nfile>=files.size()) return 0;
  if (nsong>=files[nfile]->nSongs()) return 0;
  return files[nfile]->getSong(nsong);
};

/*
Item *MyList::CurrentItem() {
  if (songs.size()==0) return 0;
  return &songs[n];
};
*/

void MyList::OnSelect(wxCommandEvent &event) {
  set_n(event.GetInt());
  frame->resetTitle();
  FileItem *file=CurrentFile();
  if (file)
    frame->editor->Set(file->getContent());
  else
    frame->editor->Set(wxString("-- no file selected --"));
  //  std::cerr<<"OnSelect "<<n<<"\n";
};

void MyList::Export(const wxString &filename, const wxString &plug) {
  Plugout *writer=Plugout::Construct(plug.c_str());
  PlugoutOptions opt;
  if (!writer) throw std::runtime_error(("Cannot initialize "+plug+" plug").c_str());
  SongCollection list;
  for (size_t i=0;i<files.size();++i) {
    for (size_t j=0;j<files[i]->nSongs();++j) {
      list.push_back(files[i]->getSong(j));
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
