#include <cassert>

#include "song.h"

template<class T> 
void delete_elements(vector<T> &list) {
  for (size_t i=0;i<list.size();++i) {
    delete list[i];
    list[i]=0;
  }
};

Song::~Song() {delete body;delete head;};
Head::~Head() {delete_elements(author);};
Body::~Body() {

  // IL TEMPLATE NON FUNZIONA!!
  // delete_elements(stanza);
  for (size_t i=0;i<stanza.size();++i)
    { delete stanza[i]; stanza[i]=0;}
};

Stanza::~Stanza() {
    assert(chords==0);
    assert(copy==0);
    delete_elements(verse);
  };

PhraseList::~PhraseList() {delete_elements(list);};

Modifier::~Modifier() {delete child;};


