#include <cstdio>
#include <vector>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <iostream>
#include <cassert>
#include <map>
#include <string.h>

#include "plug.h"

//#define XNG_VERSION "0.1"
//#define XNG_DTD "http://www.math.unifi.it/paolini/src/song/xng-" XNG_VERSION ".dtd"

using namespace std;

class XngPlugin:public Plugin {
private:
  static const bool dummy;
  static bool startup() {
    Plugin::Register("xng",&(XngPlugin::Createin));
    return true;
  };
  static Plugin* Createin() {return new XngPlugin();};
public:

  void warning(const string &msg) {
    cerr<<"WARNING: "<<msg<<"\n";
  };
  
  void ignore(xmlNodePtr p) {
    if (p->type==XML_TEXT_NODE) return;
    const char *item;
    if (p && p->name) item=(char*)p->name;
    else item="";
    warning(string("ignore unknow <")+item+"> item");
  };

  string ReadString(xmlNodePtr p) {
    if (p==0 || p->children==0) return "";
    xmlChar *s=xmlNodeListGetString(0,p->children,1);
    string r=(char *)s;
    xmlFree(s);
    return r;
  };

  PhraseItem* ReadItem(xmlNodePtr p) {
    if (p->type == XML_TEXT_NODE) 
      return new Word((char*)p->content);
    else if (!strcmp((char*)p->name,"strum")) 
      return new Modifier(Modifier::STRUM,ReadList(p));
    else if (!strcmp((char*)p->name,"note")) 
      return new Modifier(Modifier::NOTES,ReadList(p));
    else if (!strcmp((char*)p->name,"c"))
      return new Chord(ReadString(p));
    else if (!strcmp((char*)p->name,"tab"))
      return new Tab;
    else ignore(p);
    return 0;
  };

  PhraseList* ReadList(xmlNodePtr p) {
    PhraseList *list=new PhraseList;
    for (p=p->children;p;p=p->next) {
      PhraseItem *item=ReadItem(p);
      if (item) list->push_back(item);
    }
    return list;
  };

  map<string,Stanza*> ids;

  Stanza *ReadStanza(xmlNodePtr p) {
    char *prop;
    prop=(char*)xmlGetProp(p,(xmlChar *)"type");
    Stanza::Type t=Stanza::STROPHE;
    if (prop) {
      if (!strcmp(prop,"strophe")) ;
      else if (!strcmp(prop,"refrain")) t=Stanza::REFRAIN;
      else if (!strcmp(prop,"talking")) t=Stanza::SPOKEN;
      else if (!strcmp(prop,"tab")) t=Stanza::TAB;
      else warning(string("unknown stanza type ")+prop);
    }
    
    Stanza* stanza=new Stanza(t);

    prop=(char*)xmlGetProp(p,(xmlChar *)"id");
    if (prop) ids[prop]=stanza;
    
    prop=(char*)xmlGetProp(p,(xmlChar *)"chords");
    if (prop) {
      stanza->chords=ids[prop];
      if (!stanza->chords) 
	throw runtime_error(string("chords reference ")
			    +prop+" not found");
    }

    prop=(char*)xmlGetProp(p,(xmlChar *)"copy");
    if (prop) {
      stanza->copy=ids[prop];
      if (!stanza->copy) 
	throw runtime_error(string("copy reference ")
			    +prop+" not found");
    }

    for (p=p->children;p;p=p->next) {
      if (!strcmp((char *) p->name,"v")) 
	stanza->push_back(ReadList(p));
      else ignore(p);
    };
    return stanza;
  };

  Body* ReadBody(xmlNodePtr p) {
    Body *body=new Body;
    for (p=p->children;p;p=p->next) {
      if (!strcmp((char *)p->name,"stanza"))
	body->push_back(ReadStanza(p));
      else ignore(p);
    }
    return body;
  };

  Author* ReadAuthor(xmlNodePtr p) {
    Author *author=new Author;
    for (p=p->children;p;p=p->next) {
      if (!strcmp((char *)p->name,"name")) 
	author->Name=ReadString(p);
      else if (!strcmp((char *)p->name,"firstname"))
	author->firstName=ReadString(p);
      else ignore(p);
    };
    return author;
  };

  Head* ReadHead(xmlNodePtr p) {
    Head *head=new Head;
    for (p=p->children;p;p=p->next) {
      if (!strcmp((char *)p->name,"title"))
	head->title=ReadString(p);
      else if (!strcmp((char *)p->name,"author")) 
      head->author.push_back(ReadAuthor(p));
      else ignore(p);
    }
    return head;
  };

  Song* ReadSong(xmlNodePtr p) {
    Song *song=new Song;
    assert(!strcmp((char*)p->name,"song"));
    for (p=p->children;p;p=p->next) {
      if (!strcmp((char*)p->name,"head"))
	song->setHead(ReadHead(p));
      else if (!strcmp((char*)p->name,"body"))
	song->setBody(ReadBody(p));
      else ignore(p);
    };
    if (song->head()==0) throw runtime_error("no head found");
    if (song->body()==0) throw runtime_error("no body found");
    return song;
  };

  virtual int Read(string filename, SongList &list) {
    int count=0;
    xmlDocPtr doc=xmlParseFile(filename.c_str());
    if (!doc)
      throw runtime_error("non riesco a leggere il file "+filename);
    for (xmlNodePtr p=doc->children;p;p=p->next) {
      if (p->type!=XML_ELEMENT_NODE) continue;
      if (!strcmp((char*)(p->name),"songs")) p=p->children;
      if (!strcmp((char*)(p->name),"song")) {
	list.push_back(ReadSong(p));
	continue;
      } else ignore(p);
    }
    return count;
  };

  XngPlugin(): 
             Plugin("xsong","xng","xml song format [manu-fatto]")
	     {};
};

// registra il plugin
const bool XngPlugin::dummy=XngPlugin::startup();
