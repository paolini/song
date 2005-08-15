#include <cstdio>
#include <vector>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <iostream>

#include "util.h"

#include "plug.h"

using namespace std;

class ListPlugout:public Plugout {
private:
  static const bool dummy;
  static bool startup() {
    Plugout::Register("lst",&(ListPlugout::Create));
    return true;
  };
  static Plugout* Create() {return new ListPlugout();};
public:

  virtual void Write(std::ostream &out, std::vector<xmlNodePtr> &song_list,
		     const PlugoutOptions &opt) {
    unsigned int i;
    for (i=0;i<song_list.size();++i) {
      xmlNodePtr p=song_list[i];
      string title="no title",author="no author";
      for (p=p->children;p && strcmp((const char *)p->name,"head");p=p->next);
      if (p) {
	for (p=p->children;p;p=p->next) {
	  if (!strcmp((const char *)p->name,"title") && p->children) {
		xmlChar *s=xmlNodeListGetString(NULL,p->children,1);
		title=utf8(s);
		xmlFree(s);
	      }
	      if (!strcmp((const char *)p->name,"author") && p->children) {
		xmlChar *s=xmlNodeListGetString(NULL,p->children,1);
		author=utf8(s);
		xmlFree(s);
	      }
	    }
      }
	  out<<title<<" ("<<author<<")\n";
    }
  };

  ListPlugout(): Plugout("list","lst","list titles and authors"){};
};

// registra il plugin
const bool ListPlugout::dummy=ListPlugout::startup();
