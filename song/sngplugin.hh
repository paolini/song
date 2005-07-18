#ifndef __SNG_PLUGIN_H__
#define __SNG_PLUGIN_H__
#include <cstdio>
#include <vector>
#include <libxml/tree.h>

#include "plugin.hh"

class SngPlugin:public Plugin {
private:
  static const bool dummy;
  static bool plugin_startup();

  static const int MAXLINE;
  istream *current_in;
  int nriga;
  char *buff;
  char *buffp;
public:
  virtual int Read(std::istream &, std::vector<xmlNodePtr> &list);
  SngPlugin(): 
    Plugin("song","sng","song sng TeX-like format [manu-fatto]"),
    current_in(0)
  {buff=new char[MAXLINE+1];};
  virtual ~SngPlugin() {delete buff;};

  friend class Sng;
};

#endif
