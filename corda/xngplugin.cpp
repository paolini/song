#include <cstdio>
#include <vector>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <iostream>
#include <cassert>

#include "plug.h"

//#define XNG_VERSION "0.1"
//#define XNG_DTD "http://www.math.unifi.it/paolini/src/song/xng-" XNG_VERSION ".dtd"

using namespace std;

class XngPlug:public Plugin {
private:
  static const bool dummy;
  static bool startup() {
    Plugin::Register("xng",&(XngPlug::Createin));
    return true;
  };
  static Plugin* Createin() {return new XngPlug();};
public:

  warning(const string &msg) {
    cerr<<"WARNING: "<<msg<<"\n";
  };
  
  ignore(unsigned char *item) {
    warning("ignore unknow <"+item+"> item");
  };

  Head* ReadHead(XmlNodePtr p) {
    // ...........................
    // ........................
  };

  Song* ReadSong(XmlNodePtr p) {
    Song *song=new Song;
    assert(!strcmp((char*)p->name,"song"));
    for (p=p->children;p;p=p->next) {
      if (!strcmp((char*)p->name,"head"))
	song->head=ReadHead(p);
      else if (!strcmp((char*)p->name,"body"))
	song->body=ReadBody(p);
      else ignore(p->name);
    };
  };

  virtual int Read(string filename, std::vector<Song *> &list) {
    int count=0;
    xmlDocPtr doc=xmlParseFile(filename.c_str());
    if (!doc)
      throw runtime_error("non riesco a leggere il file "+filename);
    for (xmlNodePtr p=doc->children;p;p=p->next) {
      if (strcmp((char*)(p->name),"songs")!=0) p=p->children;
      if (strcmp((char*)(p->name),"song")!=0) {
	ignore(p->name);
	continue;
      } 
      if (p->children)
	list.push_back(ReadSong(p));
      count++;
    }
    return count;
  };

  void Write(FILE *out, std::vector<xmlNodePtr> &list,
	     const PlugoutOptions &opt) {
    unsigned int i;
    for (i=0;i<list.size();++i) {
      xmlDtdPtr dtd;
      xmlDocPtr doc;
      doc=xmlNewDoc(BAD_CAST "1.0");
      dtd=xmlNewDtd(doc,BAD_CAST "song",BAD_CAST XNG_DTD,0);
      doc->extSubset=dtd;
      doc->children=(xmlNodePtr) dtd;
      xmlDocSetRootElement(doc,list[i]);
      xmlDocDump(out,doc);
    }
  };
  
  virtual void Write(std::ostream &out, std::vector<xmlNodePtr> &list,
		     const PlugoutOptions &opt) {
    assert(&out==&cout);
    Write(stdout,list,opt);
  };

  virtual void Write(string filename, std::vector<xmlNodePtr> &list,
		     const PlugoutOptions &opt) {
    FILE *out=fopen(filename.c_str(),"wt"); 
    if (!out) throw runtime_error("cannot write file "+filename);
    Write(out,list,opt);
    fclose(out);
  };

  XngPlug(): 
             Plugin("xsong","xng","xml song format [manu-fatto]"),
             Plugout("xsong","xng","xml song format [manu-fatto]")
	     {};
};

// registra il plugin
const bool XngPlug::dummy=XngPlug::startup();
