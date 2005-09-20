#ifndef _media_hh_
#define _media_hh_
#include <string>

using namespace std;

// questa classe astrae il formato di stampa
// dovrebbe adattarsi a txt a PS e altro...

class Media {
protected:

public:
  enum font {NORMAL=0, REFRAIN,CHORD, TITLE, AUTHOR, TAB, NOFONT};
  bool uppercase_title;

  int stanza_sep; // spazio tra le strofe
  int column_sep; // spazio tra le colonne
  int body_sep; // spazio tra titolo e testo
  int song_sep; // spazio tra le canzoni
  
  virtual void goto_xy(int x, int y)=0;
  virtual void move_xy(int x, int y)=0;
  virtual int get_x()=0;
  virtual int get_y()=0;

  virtual int wordWidth(const string &s, font f) const=0; 
  virtual void wordWrite(const string &s, font f)=0; 
  virtual int chordWidth(const string &s) const {
    return wordWidth(s,CHORD);};
  virtual void chordWrite(const string &s) {
    wordWrite(s,CHORD);};
  virtual void chordReset() {}; //nuova riga di accordi
  virtual int spaceWidth(font f) const {return wordWidth(" ",f);};
  virtual void spaceWrite(font f) {wordWrite(" ",f);};

  virtual int lineSkip(font f)=0; 
  virtual int chordSkip() {return lineSkip(NORMAL)+lineSkip(CHORD);};
  
  virtual int page_width() const=0;
  virtual int page_height() const=0;

  virtual void newPage()=0;

  virtual ~Media(){};
  Media(bool the_uppercase_title=false): uppercase_title(the_uppercase_title) {};
  virtual void frame(int dx,int dy) const {};
};

class VectorMedia: public Media {
  // dimensioni in centesimi di punto
public:
  // normal, refrain, chord, title, author, tabs, nofont
  static const int font_size[];
  static const int font_font[]; // roman, italic, bold

  int left, bottom,top;
  int my_width, my_height;
  int chord_sep;

  virtual int page_height() const {return my_height;};
  virtual int page_width() const {return my_width;};
  VectorMedia();
};

#endif
