#include <cstdio>
#include <vector>
#include <iostream>
#include <cassert>

#include "song.h"
#include "iso.h"
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

  virtual void Write(std::ostream &out, const SongArray &song_list,
		     const PlugoutOptions &opt) {
    unsigned int i;
    for (i=0;i<song_list.size();++i) {
      const Song* p=song_list[i];
      assert(p->head()!=0);
      out<<iso(p->head()->title);
      size_t j;
      for (j=0;j<p->head()->author.size();++j){
	if (j==0) out<<" (";
	else out<<", ";
	string s=p->head()->author[j]->firstName;
	if (s.size()) s+=" ";
	s+=p->head()->author[j]->Name;
	out<<iso(s);
      }
      if (j) out<<")\n";
    }
  };

  ListPlugout(): Plugout("list","lst","list titles and authors"){};
};

// registra il plugin
const bool ListPlugout::dummy=ListPlugout::startup();
