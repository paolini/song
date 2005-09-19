#include <cassert>

#include "song.h"

void SongBase::setParent(SongBase *Parent) {assert(parent==0);parent=Parent;};

Song::~Song() {delete my_body;delete my_head;};

Modifier::~Modifier() {delete child;};
Modifier::Modifier(Type t, PhraseList *c): attribute(t), child(c) {
  assert(c);};


void Song::removeChild(SongBase *c) {
    assert(c==my_head || c==my_body);
    if (c==my_head) my_head=0;
    else if (c==my_body) my_body=0;
  };

void Song::setHead(Head *h) {assert(my_head==0);my_head=h;h->setParent(this);};
void Song::setBody(Body *b) {assert(my_body==0);my_body=b;b->setParent(this);};
void Modifier::removeChild(SongBase *c) {
  assert(false);
  //non posso rimuovere l'unico child
};
