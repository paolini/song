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
class Verse;
class PhraseList;
class PhraseItem;

class SongList {
 public:
  ~SongList();
  vector<Song *> list;
  const Song *operator[](size_t i) const {return list[i];};
  Song *operator[](size_t i) {return list[i];};
  size_t size() const {return list.size();};
  void push_back(Song *p) {list.push_back(p);};
};

class Song {
public:
  Head *head;
  Body *body;
  ~Song();
  Song(Head *h=0,Body* b=0):head(h), body(b) {};
};

class Head {
 public:
  string title;
  vector<Author *> author;
  ~Head();
};

class Author {
 public:
  string firstName;
  string Name;
};

class Body {
 public:
  vector<Stanza *> stanza;
  ~Body();
};

class Stanza {
 public:
  typedef enum {STROPHE, REFRAIN, SPOKEN, TAB} Type;
  Type type;
  Stanza *chords;
  Stanza *copy;
  vector<PhraseList *> verse;
  Stanza(Type t): type(t), chords(0), copy(0) {};
protected:
  ~Stanza();
  friend class Body;
};

class PhraseList {
 public:
  vector<PhraseItem *> list;
  ~PhraseList();
};

class PhraseItem {
 public:
  virtual ~PhraseItem(){};
};

class Word: public PhraseItem {
 public:
  string word;
  Word(const string &s): word(s) {};
};

class Modifier: public PhraseItem {
 public:
  typedef enum {STRUM, NOTES} Type;
  Type attribute;
  PhraseList *child;
  Modifier(Type t, PhraseList *c): attribute(t), child(c) {};
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
