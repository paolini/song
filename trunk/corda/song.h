// EMACS MODE: -*- mode: c++; -*-
#ifndef SONG_H
#define SONG_H

#include <string>
#include <vector>


//using namespace std;
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
  SongBase(): parent(0){};
  //  SongBase(SongBase *Parent): parent(Parent) }{};
};

template<class T> 
class SongVector: public SongBase{
 protected:
  std::vector<T *> list;
  virtual void removeChild(SongBase *child) {
    size_t i;
    for (i=0;i<list.size() && list[i]!=child;++i);
    if (i<list.size()) list.erase(list.begin()+i);
  };
 public:
  SongVector() {}; //non puo` essere costruito dall'utente
  void clear() {
    for (size_t i=0;i<list.size();++i) {
      delete list[i];list[i]=0;}
  }; //cancella tutto il contenuto
  virtual ~SongVector() {clear();};
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
  Song(): my_head(0), my_body(0){};
};

class Head: public SongBase {
 public:
  std::string title;
  SongVector<Author> author;
};

class Author: public SongBase {
 public:
  std::string firstName;
  std::string Name;
  Author(){};
  Author(const std::string &firstname,const std::string &name): firstName(firstname),
						      Name(name){};
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
  Stanza(Type t=STROPHE): type(t), chords(0), copy(0) {};
  void setType(Type t) {type=t;};
protected:
  friend class Body;
};

class PhraseList: public SongVector<PhraseItem> {
 public:
};

class Word;
class Modifier;
class Note;
class Chord;
class Tab;

class PhraseItem: public SongBase {
 public:
  virtual ~PhraseItem(){};
  virtual const Word* wordp() const {return 0;};
  virtual const Modifier* modifierp() const {return 0;};
  virtual const Note* notep() const {return 0;};
  virtual const Chord* chordp() const {return 0;};
  virtual const Tab* tabp() const {return 0;};
};

class Word: public PhraseItem {
 public:
  std::string word;
  Word(const std::string &s): word(s) {};
  virtual const Word* wordp() const {return this;};
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
  virtual const Modifier *modifierp() const {return this;};
};

class Note {
private:
  static const char *it_names[];
public:
  int note; // 0...6 or -1 if empty
  int aug;  // +# -b
  Note(): note(-1), aug(0){};
  Note(const std::string &s,size_t &off);
  operator std::string() const;
  virtual const Note* notep() const {return this;};
};

class Chord: public PhraseItem {
public:
  Note base;
  Note bass;
  std::string modifier; // -7dim ...
 public:
  Chord(const std::string &s);
  operator std::string() const;
  virtual const Chord* chordp() const {return this;};
};

class Tab: public PhraseItem {
 public:
 virtual const Tab* tabp() const {return this;};
};

#endif
