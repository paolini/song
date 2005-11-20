#include "list.h"
#include "frame.h"

MyList::MyList(wxWindow *parent, MyFrame* fr):
  wxListCtrl(parent,-1,wxDefaultPosition,wxDefaultSize,
	     0)
{
  InsertItem(0,"line 0");
};
