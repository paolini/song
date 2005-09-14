#ifndef SONG_H
#define SONG_H

#include <string>
#include <vector>


using namespace std;

// ALL STRINGS ENCODED IN UTF8

class Head;
class Body;
class Author;
class Stanza;
class Verse;
class PhraseList;
class PhraseItem;

class Song {
public:
  Head *head;
  Body *body;
  ~Song();
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
};

class Modifier: public PhraseItem {
 public:
  enum {STRUM, NOTES} attribute;
  PhraseList *child;
  ~Modifier();
};

class Chord: public PhraseItem {
 public:
  int base; // LA=0, LA#=1,...
  int bass; // ""    ""
  string modifier; // -7dim ...
};

class Tab: public PhraseItem {
 public:
};

#endif
