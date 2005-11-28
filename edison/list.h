#ifndef LIST_H
#define LIST_H
#include "corda/song.h"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>

class MyFrame;

class FileItem {
  wxString content;
  wxString filename; // se filename="" e` un file nuovo
  wxString plug; //sng o cho
  bool modified;
  mutable bool compiled_valid;
  mutable SongList compiled;
  void reset();
  void reload();
  void compile() const;
 public:
  bool Modified() const {return modified;};
  const wxString &getContent() const;
  void setContent(const wxString &);
  const wxString &FileName() const;
  FileItem();
  void Load(const wxString &filename);
  void Save();
  void SaveAs(const wxString &filename);
  const Song *getSong(unsigned int n=0) const;
  const SongList &getList() const;
};

class Item {
 private:
  int n;
 public:
  FileItem *file;
  Item(FileItem *_file, int _n=0): n(_n), file(_file){};
  const Song *getSong() {return file->getSong(n);};
};

class MyList: public wxListBox /*wxListCtrl*/ {
 private:
  MyFrame *frame;
  std::vector<Item> songs;
  unsigned int n;
 public:
  void Load(const wxString &filename);
  void Save();
  void SaveAs(const wxString &filename);
  MyList(wxWindow *parent, MyFrame *frame);
  const Song *getSong(unsigned int n=0) {return songs[n].getSong();};
  FileItem *CurrentFile();
  void Export(const wxString &filename, const wxString &plug);

  void OnSelect(wxListEvent &event);

 private:
  DECLARE_EVENT_TABLE()
};

#endif
