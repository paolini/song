// EMACS MODE: -*- mode: c++; -*-
#ifndef SONG_H
#define SONG_H

#include <string>
#include <vector>


using namespace std;

// ALL STRINGS ENCODED IN UTF8

class Song;
class Head;
class Body;
class Author;
class Stanza;
//class Verse;
class PhraseList;
class PhraseItem;

class SongBase {
private:
  SongBase *parent;
protected:
  virtual void removeChild(SongBase *child) {};
public:
  void setParent(SongBase *Parent);
  virtual ~SongBase() {if (parent) parent->removeChild(this);};
  //  SongBase(SongBase *Parent): parent(Parent) }{};
};

template<class T> 
class SongVector: public SongBase{
 protected:
  vector<T *> list;
  virtual void removeChild(SongBase *child) {
    size_t i;
    for (i=0;i<list.size() && list[i]!=child;++i);
    if (i<list.size()) list.erase(list.begin()+i);
  };
 public:
  SongVector() {}; //non puo` essere costruito dall'utente
  virtual ~SongVector() {for (size_t i=0;i<list.size();++i) {
  delete list[i];list[i]=0;}};
  const T *operator[](size_t i) const {return list[i];}; 
  T *operator[](size_t i) {return list[i];}; 
  size_t size() const {return list.size();}; 
  void push_back(T *p) {list.push_back(p);p->setParent(this);}; 
};

class SongList: public SongVector<Song> {
protected:
public:
};

class Song: public SongBase {
  Head *my_head;
  Body *my_body;
protected:
  virtual void removeChild(SongBase *c);
public:
  const Head *head() const {return my_head;};
  const Body *body() const {return my_body;};
  Head *head() {return my_head;};
  Body *body() {return my_body;};
  
  void setHead(Head *h);
  void setBody(Body *b);

  virtual ~Song();
};

class Head: public SongBase {
 public:
  string title;
  SongVector<Author> author;
};

class Author: public SongBase {
 public:
  string firstName;
  string Name;
};

class Body: public SongVector<Stanza> {
 public:
};

typedef PhraseList Verse;

class Stanza: public SongVector<Verse> {
 public:
  typedef enum {STROPHE, REFRAIN, SPOKEN, TAB} Type;
  Type type;
  Stanza *chords;
  Stanza *copy;
  //  vector<PhraseList *> verse;
  Stanza(Type t): type(t), chords(0), copy(0) {};
protected:
  friend class Body;
};

class PhraseList: public SongVector<PhraseItem> {
 public:
};

class PhraseItem: public SongBase {
 public:
  virtual ~PhraseItem(){};
};

class Word: public PhraseItem {
 public:
  string word;
  Word(const string &s): word(s) {};
};

class Modifier: public PhraseItem {
protected:
  void removeChild(SongBase *c);
 public:
  typedef enum {STRUM, NOTES} Type;
  Type attribute;
  PhraseList *child;
  Modifier(Type t, PhraseList *c);
  ~Modifier();
};

class Chord: public PhraseItem {
 public:
  int base; // LA=0, LA#=1,...
  int bass; // ""    ""
  string modifier; // -7dim ...
  Chord(const string &s): modifier(s) {};
};

class Tab: public PhraseItem {
 public:
};

#endif
