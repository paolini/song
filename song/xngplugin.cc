#include <cstdio>
#include <vector>
#include <libxml/tree.h>
#include <iostream>

#include "plugin.hh"

using namespace std;

class XngPlugin:public Plugin {
private:
  static const bool dummy;
  static bool startup() {
    Plugin::Register("xng",&(XngPlugin::Create));
    return true;
  }
  static Plugin* Create() {return new XngPlugin();};
public:

  virtual int Read(std::istream &, std::vector<xmlNodePtr> &list) {
    throw runtime_error("not yet implemented... sorry");
  };
  virtual int Read(string filename, std::vector<xmlNodePtr> &list) {
    int count=0;
    xmlDocPtr doc=xmlParseFile(filename.c_str());
    if (!doc)
      throw runtime_error("non riesco a leggere il file "+filename);
    for (xmlNodePtr p=doc->children;p;p=p->next) {
      if (strcmp((char*)(p->name),"song")!=0) {
	cerr<<"Warning: ignore unknown <"<<(p->name)<<" item\n";
	continue;
      } 
      list.push_back(p);
      count++;
    }
    return count;
  };
  
  XngPlugin(): Plugin("xsong","xng","xml song format [manu-fatto]"){};
};

// registra il plugin
const bool XngPlugin::dummy=XngPlugin::startup();
