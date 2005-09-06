#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include <iostream>

#include <libxml/tree.h>
#include "media.h"

//extern bool stanza_debug, verse_debug, song_debug, body_debug;


class Dim {
public:
  unsigned int x,y;
  Dim(unsigned int xx, unsigned int yy): x(xx), y(yy) {};
  Dim(): x(0), y(0) {};
  unsigned int get(bool hor) const {return hor?x:y;};
  unsigned int &get(bool hor) {return hor?x:y;};
  bool operator==(const Dim &d) const {
    return d.x==x && d.y==y;};
  bool operator!=(const Dim &d) const {
    return !operator==(d);};
  unsigned int area() const {return x*y;};
  friend ostream &operator<<(ostream &, const Dim&);
};

class Badness {
private:
  int bad;
protected:
public:
  Badness(int b=0): bad(b) {};
  Badness &operator+=(const Badness &B) {
    bad+=B.bad;
    return *this;
  };

  Badness operator*(int n) const {return Badness(bad*n);};

  Badness operator+(const Badness &b) const {
    return Badness(bad+b.bad);
  };

  bool operator<=(const Badness &b) const {
    return bad<=b.bad;
  };

  bool operator<(const Badness &b) const {
    return bad<b.bad;
  };

  static Badness BrokenLine() {return Badness(10);};
  static Badness Spills() {return Badness(1000);};

  friend ostream& operator<<(ostream& o, const Badness &b);
};

class DimNBad: public Dim {
public:
  Badness bad;
  DimNBad(): Dim(0,0), bad() {};
  DimNBad(int x, int y, Badness b=Badness()): Dim(x,y), bad(b) {};
  DimNBad(Dim d, Badness b=Badness()): Dim(d), bad(b) {};

  friend ostream& operator<<(ostream &,const DimNBad &);
};

// Base class (empty box)

class Box {
protected:
  static Media *m;
public:
  static void setMedia(Media &media) {m=&media;};

  // che dimensioni assume in un ambiente di dimensioni <space>
  virtual DimNBad dim(Dim space) {return DimNBad(0,0);};

  virtual unsigned int chord_width() {return 0;};
  
  Box() {}
  virtual ~Box() {};
  
  // disegna il contenuto in un ambiente <space>
  virtual DimNBad write(Dim space) {return dim(space);};

  virtual bool please() {return true;};
};

// SequenceBox, StringBox, ChordBox, TabBox, CompressBox, PleaseBox

class ModifierBox: public Box {
protected:
  Box *my_box;
public:
  ModifierBox(Box *box): my_box(box) {};  
  virtual ~ModifierBox() {delete my_box;};
  virtual DimNBad dim(Dim space) {return my_box->dim(space);};
  virtual DimNBad write(Dim space) {return my_box->write(space);};
};

// usato per i Talking, non hanno una dimensione preferita

class PleaseBox: public ModifierBox {
public:
  PleaseBox(Box *box): ModifierBox(box) {};
  virtual ~PleaseBox(){};
  virtual bool please() {return false;};
};

//Helper class: remembers last width computation:
class CacheBox: public Box {
protected:
  Dim last_space; // ultima richiesta
  DimNBad my_dim; // ultima risposta

  // viene chiamata quando la cache non e` sufficiente
  // la dimensione richiesta e` in last_space
  virtual void recalculate()=0;
  // viene richiesto l'effettivo write
  virtual void current_write()=0;

  virtual void add_spilling();
public:
  int valign, halign; //allineamento (-1,0,1)

  CacheBox(): last_space(0,0), my_dim(0,0,Badness(99999)) {};
  virtual ~CacheBox(){};

  virtual DimNBad dim(Dim space);
  virtual DimNBad write(Dim space);
};


class SequenceBox: public CacheBox {
public:
  bool test;
  unsigned int space; //spazio di separazione tra gli oggetti
  int sup_space; // spazio tra le righe/colonne
  int word_align; // allineamento degli oggetti  -1,0,1

protected:
  vector<Box *> list;
  bool hor; //orizzontale / verticale
  // tutti i nomi sono come nel caso orizzontale
  //  int align; //0: center, -1: inf, 1: sup
  //  int sup_align; // allineamento tra le righe/colonne

  bool breakable; // posso spezzarlo su piu' righe/colonne
  bool adjustable; // tenta di modificare l'altezza delle righe

  vector<unsigned int> lines_height; //contiene le altezze/larghezze
  //delle singole righe/colonne
  vector<unsigned int> end_item; //primo oggetto della riga seguente

  vector<Dim> given_space; //spazio dato a ogni elemento

protected:
  virtual void current_write();
  virtual void recalculate();

public:
  virtual ~SequenceBox();
  SequenceBox(bool horizontal=true, 
	      bool Break=false, 
	      bool adjust=false);
  
  void push_back(Box *ptr) {list.push_back(ptr);};

  Box* get(int n) {return list[n];};

  void free(int n) {delete list[n];list.erase(list.begin()+n);};
  
  int size() const {return list.size();};
};

class SequenceBox2: public CacheBox {
public:
  bool test;
  unsigned int space; //spazio di separazione tra gli oggetti
  int sup_space; // spazio tra le righe/colonne
  int word_align; // allineamento degli oggetti  -1,0,1

protected:
  vector<Box *> list;
  bool hor; //orizzontale / verticale
  // tutti i nomi sono come nel caso orizzontale
  //  int align; //0: center, -1: inf, 1: sup
  //  int sup_align; // allineamento tra le righe/colonne

  bool breakable; // posso spezzarlo su piu' righe/colonne
  bool adjustable; // tenta di modificare l'altezza delle righe

  vector<DimNBad> line_dim; 
  //contiene le altezze/larghezze/badness
  //delle singole righe/colonne
  vector<unsigned int> next_item; //primo oggetto della riga seguente

  vector<DimNBad> given_space; //spazio dato a ogni elemento

protected:
  virtual void current_write();
  virtual void recalculate();

public:
  virtual ~SequenceBox2();
  SequenceBox2(bool horizontal=true, 
	      bool Break=false, 
	      bool adjust=false);
  
  void push_back(Box *ptr) {list.push_back(ptr);};

  Box* get(int n) {return list[n];};

  void free(int n) {delete list[n];list.erase(list.begin()+n);};
  
  int size() const {return list.size();};
};

// Helper class: le dimensioni non dipendono dall'ambiente:
class FixedBox: public Box {
protected:
  Dim my_dim;
public:
  virtual DimNBad dim(Dim space);
  virtual ~FixedBox(){};
};

class StringBox: public FixedBox {
protected:
  string my_s;
  Media::font f;
  void calculate();
public:
  StringBox(unsigned char *utf8_string,unsigned char *end,Media::font the_f);
  StringBox(const string &s,Media::font the_f);
  virtual ~StringBox(){};
  virtual DimNBad write(Dim space);
};

class ChordBox: public StringBox {
protected:
  unsigned int my_chord_width;
public:
  ChordBox(string s);
  virtual ~ChordBox(){};
  virtual DimNBad write(Dim space);
  virtual unsigned int chord_width() {return 0*my_chord_width;};
};

class TabBox: public StringBox {
public:
  TabBox(): StringBox(" / ",Media::NORMAL) {};
  virtual ~TabBox(){};
};

class CompressBox: public CacheBox {
private:
  Box *my_box;
  bool my_hor;
public:
  CompressBox(Box *box,bool hor): my_box(box), my_hor(hor) {};
  virtual ~CompressBox() {delete my_box;};
  virtual void current_write() {my_box->write(last_space);};
protected:
  void recalculate();
};

#endif
