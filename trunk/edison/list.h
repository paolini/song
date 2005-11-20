#ifndef LIST_H
#define LIST_H
#include <wx/listctrl.h>

class MyFrame;

class MyList: public wxListCtrl {
 private:
  MyFrame *frame;
 public:
  MyList(wxWindow *parent, MyFrame *frame);
};

#endif
