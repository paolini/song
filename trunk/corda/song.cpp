#include <cassert>
#include <stdexcept>

#include "song.h"

using namespace std;

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

bool stribeg(const string &s, const string &with, size_t &off) {
  size_t i;
  for (i=0;off+i<s.size() && i<with.size() && 
	 tolower(s[off+i])==tolower(with[i]);++i);
  if (i==with.size()) {
    off+=i;
    return true;
  }
  else return false;
}

const char *Note::it_names[]={"La","Si","Do","Re","Mi","Fa","Sol"};

Note::Note(const string &s, size_t &off) {
  int i;
  for (i=0;i<7 && !stribeg(s,it_names[i],off);++i);
  if (i<7) {
    note=i;
  } else if (s.size()>0 && s[0]>='A' && s[0]<='G') {
    note=s[0]-'A';
  } else {
    note=-1;aug=0;
    return;
  }
  for(aug=0;off<s.size() && s[off]=='#';aug++,off++);
  if (aug==0) {
    for(;off<s.size() && s[off]=='b';aug--,off++);  
  }
}

Note::operator string() const {
  string r;
  if (note>=0) {
    r+='A'+note;
      //it_names[note];
    if (aug>0) {
      for(int i=0;i<aug;++i) r+='#';
    } else if (aug<0) {
      for(int i=0;i<-aug;++i) r+='b';
    }
  }
  return r;
};

Chord::Chord(const string &s) {
  size_t off=0;
  base=Note(s,off);
  size_t off1=off;
  while (off<s.size() && s[off]!=']' && s[off]!='/' && s[off]!='(') off++;
  modifier=string(s,off1,off-off1);
  if (off<s.size() && (s[off]=='/' || s[off]=='(')) { //bass
    char c=s[off++];
    bass=Note(s,off);
    if (c=='(' && off<s.size() && s[off]==')') off++; 
  } else {
    bass=Note();
  }
  if (modifier.find(' ')!=string::npos) 
    throw runtime_error("no spaces allowed in chord");
};

Chord::operator string() const {
  string r=string(base);
  r+=modifier;
  if (bass.note>=0) {
    r+='/';
    r+=string(bass);
  }
  return r;
}
