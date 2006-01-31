#ifndef LIST_H
#define LIST_H
#include "corda/song.h"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "corda/plug.h"

class MyFrame;

class FileItem {
  MyFrame *frame;
  wxString content;
  wxString filename; // se filename="" e` un file nuovo
  wxString plug; //sng o cho
  bool modified;
  wxMutex mutex;
 public:
  //bool freezed;
  enum {NOT_COMPILED=0,
		COMPILED_OK, // le song sono in 'compiled'
		COMPILED_EMPTY, // nessuna song, ma il file e' valido
		COMPILED_ERROR // ci sono errori
  } status;
 private:
  SongList compiled;
 public:
  PlugError compilation_error;
 private:
  void reset();
  void reload();
  void compile();
 public:
  bool Modified() const {return modified;};
  const wxString &getContent() const;
  void setContent(const wxString &);
  const wxString &FileName() const;
  FileItem(MyFrame *frame);
  void Load(const wxString &filename);
  void Save();
  void SaveAs(const wxString &filename);
  const Song *getSong(unsigned int n);
  wxString Title(unsigned int n);
  int nSongs();
  const SongList &getList();
};

/*
class Item {
 public:
  int n; //n-esima canzone del file
 public:
  FileItem *file;
  Item(FileItem *_file, int _n=0): n(_n), file(_file){};
  const Song *getSong() const {return file->getSong(n);};
  wxString Title() const;
};
*/

class MyList: public wxListBox /* wxListCtrl*/ {
 private:
  MyFrame *frame;
  std::vector<FileItem *> files;
  //  unsigned int n; // canzone visualizzata
  unsigned int nfile; // file corrente
  unsigned int nsong; // canzone corrente (all'interno del file)
  unsigned int get_n() const;
  void set_n(unsigned int n);
 public:
  void OnDraw();
  void Update();
  void Load(const wxString &filename);
  void Save();
  void SaveAs(const wxString &filename);
  MyList(wxWindow *parent, MyFrame *frame);
  //  const Song *getSong(unsigned int n=0) {return songs[n].getSong();};
  FileItem *CurrentFile();
  const Song *CurrentSong() const;
  //  Item *CurrentItem();
  void Export(const wxString &filename, const wxString &plug);

  void OnSelect(wxCommandEvent &event);

 private:
  DECLARE_EVENT_TABLE()
};

#endif
