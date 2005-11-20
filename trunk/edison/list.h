#ifndef LIST_H
#define LIST_H
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "song.h"

class MyFrame;

class FileItem {
  wxString content;
  wxString filename;
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
  const Song *getSong(int n=0) const;
};

class MyList: public wxListCtrl {
 private:
  MyFrame *frame;
 public:
  MyList(wxWindow *parent, MyFrame *frame);
};

#endif
