#include <iostream>
#include <cctype>
#include <cstring>
#include <cassert>
#include <sstream>
#include <vector>
#include <string>
#include <map>

#include "song.h"

using namespace std;

#include "plug.h"

void strlwr(string &s)
{
  for (int i=0;i<s.size();++i) s[i]=tolower(s[i]);
}
void strupr(string &s)
{
  for (int i=0;i<s.size();++i) s[i]=toupper(s[i]);
}

static char *commands[]={
  "ssong",
  "ttitle","aauthor",
  "xkeys","gpage","ooffset",
  "sstrophe","rrefrain","kspoken","btab","nnotes",
  "llabel","ycopy","frefer",
  "cchords","mstrum","ppar",""};

static char *isoacc[]={
  " " "a"   "e"   "i"   "o"   "u"   "A"   "E"   "I"   "O"   "U"   "nNcC",
  "`" "\xE0""\xE8""\xEC""\xF2""\xF9""\xC0""\xC8""\xCC""\xD2""\xD9""    ",
  "'" "\xE1""\xE9""\xED""\xF3""\xFA""\xC1""\xC9""\xCD""\xD3""\xDA""    ",
  "^" "\xE2""\xEA""\xEE""\xF4""\xFB""\xC2""\xCA""\xCE""\xD4""\xDB""    ",
  "~" "\xE3"" "   " "   "\xF5"" "   "\xC3"" "   " "   "\xD5"" "   "\xF1\xD1  ",
  "\"""\xE4""\xEB""\xEF""\xF6""\xFC""\xC4""\xCB""\xCF""\xD6""\xDC""    ",
  "," " "   " "   " "   " "   " "   " "   " "   " "   " "   " "   "  \xE7\xC7",
  "<" " "   " "   " "   " "   " "   " "   " "   " "   " "   " "   "  ""cC",
  ""};

struct {
  char c;
  char *name;
} st_accents[] ={
  {'`',"grave"},
  {'\'',"acute"},
  {'^',"circumflex"},
  {'~',"tilde"},
  {'\"',"dieresis"},
  {',',"cedilla"},
  {'<',"caron"},
  {0,0}};

struct {
  int code;
  char *name;
} st_unicode_name[]={
    {0x00C0, "Agrave"},
    {0x00C1, "Aacute"},
    {0x00C2, "Acircumflex"},
    {0x00C3, "Atilde"},
    {0x00C4, "Adieresis"},
    {0x00C5, "Aring"},
    {0x00C6, "AE"},
    {0x00C7, "Ccedilla"},
    {0x00C8, "Egrave"},
    {0x00C9, "Eacute"},
    {0x00CA, "Ecircumflex"},
    {0x00CB, "Edieresis"},
    {0x00CC, "Igrave"},
    {0x00CD, "Iacute"},
    {0x00CE, "Icircumflex"},
    {0x00CF, "Idieresis"},
    {0x00D0, "Eth"},
    {0x00D1, "Ntilde"},
    {0x00D2, "Ograve"},
    {0x00D3, "Oacute"},
    {0x00D4, "Ocircumflex"},
    {0x00D5, "Otilde"},
    {0x00D6, "Odieresis"},
    {0x00D7, "multiply"},
    {0x00D8, "Oslash"},
    {0x00D9, "Ugrave"},
    {0x00DA, "Uacute"},
    {0x00DB, "Ucircumflex"},
    {0x00DC, "Udieresis"},
    {0x00DD, "Yacute"},
    {0x00DE, "Thorn"},
    {0x00DF, "germandbls"},
    {0x00E0, "agrave"},
    {0x00E1, "aacute"},
    {0x00E2, "acircumflex"},
    {0x00E3, "atilde"},
    {0x00E4, "adieresis"},
    {0x00E5, "aring"},
    {0x00E6, "ae"},
    {0x00E7, "ccedilla"},
    {0x00E8, "egrave"},
    {0x00E9, "eacute"},
    {0x00EA, "ecircumflex"},
    {0x00EB, "edieresis"},
    {0x00EC, "igrave"},
    {0x00ED, "iacute"},
    {0x00EE, "icircumflex"},
    {0x00EF, "idieresis"},
    {0x00F0, "eth"},
    {0x00F1, "ntilde"},
    {0x00F2, "ograve"},
    {0x00F3, "oacute"},
    {0x00F4, "ocircumflex"},
    {0x00F5, "otilde"},
    {0x00F6, "odieresis"},
    {0x00F7, "divide"},
    {0x00F8, "oslash"},
    {0x00F9, "ugrave"},
    {0x00FA, "uacute"},
    {0x00FB, "ucircumflex"},
    {0x00FC, "udieresis"},
    {0x00FD, "yacute"},
    {0x00FE, "thorn"},
    {0x00FF, "ydieresis"},
    {0x0100, "Amacron"},
    {0x0101, "amacron"},
    {0x0102, "Abreve"},
    {0x0103, "abreve"},
    {0x0104, "Aogonek"},
    {0x0105, "aogonek"},
    {0x0106, "Cacute"},
    {0x0107, "cacute"},
    {0x0108, "Ccircumflex"},
    {0x0109, "ccircumflex"},
    {0x010A, "Cdotaccent"},
    {0x010B, "cdotaccent"},
    {0x010C, "Ccaron"},
    {0x010D, "ccaron"},
    {0x010E, "Dcaron"},
    {0x010F, "dcaron"},
    {0x0110, "Dcroat"},
    {0x0111, "dcroat"},
    {0x0112, "Emacron"},
    {0x0113, "emacron"},
    {0x0114, "Ebreve"},
    {0x0115, "ebreve"},
    {0x0116, "Edotaccent"},
    {0x0117, "edotaccent"},
    {0x0118, "Eogonek"},
    {0x0119, "eogonek"},
    {0x011A, "Ecaron"},
    {0x011B, "ecaron"},
    {0x011C, "Gcircumflex"},
    {0x011D, "gcircumflex"},
    {0x011E, "Gbreve"},
    {0x011F, "gbreve"},
    {0x0120, "Gdotaccent"},
    {0x0121, "gdotaccent"},
    {0x0122, "Gcommaaccent"},
    {0x0123, "gcommaaccent"},
    {0x0124, "Hcircumflex"},
    {0x0125, "hcircumflex"},
    {0x0126, "Hbar"},
    {0x0127, "hbar"},
    {0x0128, "Itilde"},
    {0x0129, "itilde"},
    {0x012A, "Imacron"},
    {0x012B, "imacron"},
    {0x012C, "Ibreve"},
    {0x012D, "ibreve"},
    {0x012E, "Iogonek"},
    {0x012F, "iogonek"},
    {0x0130, "Idotaccent"},
    {0x0131, "dotlessi"},
    {0x0132, "IJ"},
    {0x0133, "ij"},
    {0x0134, "Jcircumflex"},
    {0x0135, "jcircumflex"},
    {0x0136, "Kcommaaccent"},
    {0x0137, "kcommaaccent"},
    {0x0138, "kgreenlandic"},
    {0x0139, "Lacute"},
    {0x013A, "lacute"},
    {0x013B, "Lcommaaccent"},
    {0x013C, "lcommaaccent"},
    {0x013D, "Lcaron"},
    {0x013E, "lcaron"},
    {0x013F, "Ldot"},
    {0x0140, "ldot"},
    {0x0141, "Lslash"},
    {0x0142, "lslash"},
    {0x0143, "Nacute"},
    {0x0144, "nacute"},
    {0x0145, "Ncommaaccent"},
    {0x0146, "ncommaaccent"},
    {0x0147, "Ncaron"},
    {0x0148, "ncaron"},
    {0x0149, "napostrophe"},
    {0x014A, "Eng"},
    {0x014B, "eng"},
    {0x014C, "Omacron"},
    {0x014D, "omacron"},
    {0x014E, "Obreve"},
    {0x014F, "obreve"},
    {0x0150, "Ohungarumlaut"},
    {0x0151, "ohungarumlaut"},
    {0x0152, "OE"},
    {0x0153, "oe"},
    {0x0154, "Racute"},
    {0x0155, "racute"},
    {0x0156, "Rcommaaccent"},
    {0x0157, "rcommaaccent"},
    {0x0158, "Rcaron"},
    {0x0159, "rcaron"},
    {0x015A, "Sacute"},
    {0x015B, "sacute"},
    {0x015C, "Scircumflex"},
    {0x015D, "scircumflex"},
    {0x015E, "Scedilla"},
    {0x015F, "scedilla"},
    {0x0160, "Scaron"},
    {0x0161, "scaron"},
    {0x0162, "Tcommaaccent"},
    {0x0163, "tcommaaccent"},
    {0x0164, "Tcaron"},
    {0x0165, "tcaron"},
    {0x0166, "Tbar"},
    {0x0167, "tbar"},
    {0x0168, "Utilde"},
    {0x0169, "utilde"},
    {0x016A, "Umacron"},
    {0x016B, "umacron"},
    {0x016C, "Ubreve"},
    {0x016D, "ubreve"},
    {0x016E, "Uring"},
    {0x016F, "uring"},
    {0x0170, "Uhungarumlaut"},
    {0x0171, "uhungarumlaut"},
    {0x0172, "Uogonek"},
    {0x0173, "uogonek"},
    {0x0174, "Wcircumflex"},
    {0x0175, "wcircumflex"},
    {0x0176, "Ycircumflex"},
    {0x0177, "ycircumflex"},
    {0x0178, "Ydieresis"},
    {0x0179, "Zacute"},
    {0x017A, "zacute"},
    {0x017B, "Zdotaccent"},
    {0x017C, "zdotaccent"},
    {0x017D, "Zcaron"},
    {0x017E, "zcaron"},
    {0x017F, "longs"},
    {0x0192, "florin"},
    {0x01A0, "Ohorn"},
    {0x01A1, "ohorn"},
    {0x01AF, "Uhorn"},
    {0x01B0, "uhorn"},
    {0x01E6, "Gcaron"},
    {0x01E7, "gcaron"},
    {0x01FA, "Aringacute"},
    {0x01FB, "aringacute"},
    {0x01FC, "AEacute"},
    {0x01FD, "aeacute"},
    {0x01FE, "Oslashacute"},
    {0x01FF, "oslashacute"},
    {0x0218, "Scommaaccent"},
    {0x0219, "scommaaccent"},
    {0x021A, "Tcommaaccent"},
    {0x021B, "tcommaaccent"},
    {0x02BC, "afii57929"},
    {0x02BD, "afii64937"},
    {0x02C6, "circumflex"},
    {0x02C7, "caron"},
    {0x02C9, "macron"},
    {0x02D8, "breve"},
    {0x02D9, "dotaccent"},
    {0x02DA, "ring"},
    {0x02DB, "ogonek"},
    {0x02DC, "tilde"},
    {0x02DD, "hungarumlaut"},
    {0x0300, "gravecomb"},
    {0x0301, "acutecomb"},
    {0x0303, "tildecomb"},
    {0x0309, "hookabovecomb"},
    {0x0323, "dotbelowcomb"},
    {0x0384, "tonos"},
    {0x0385, "dieresistonos"},
    {0x0386, "Alphatonos"},
    {0x0387, "anoteleia"},
    {0x0388, "Epsilontonos"},
    {0x0389, "Etatonos"},
    {0x038A, "Iotatonos"},
    {0x038C, "Omicrontonos"},
    {0x038E, "Upsilontonos"},
    {0x038F, "Omegatonos"},
    {0x0390, "iotadieresistonos"},
    {0x0391, "Alpha"},
    {0x0392, "Beta"},
    {0x0393, "Gamma"},
    {0x0394, "Delta"},
    {0x0395, "Epsilon"},
    {0x0396, "Zeta"},
    {0x0397, "Eta"},
    {0x0398, "Theta"},
    {0x0399, "Iota"},
    {0x039A, "Kappa"},
    {0x039B, "Lambda"},
    {0x039C, "Mu"},
    {0x039D, "Nu"},
    {0x039E, "Xi"},
    {0x039F, "Omicron"},
    {0x03A0, "Pi"},
    {0x03A1, "Rho"},
    {0x03A3, "Sigma"},
    {0x03A4, "Tau"},
    {0x03A5, "Upsilon"},
    {0x03A6, "Phi"},
    {0x03A7, "Chi"},
    {0x03A8, "Psi"},
    {0x03A9, "Omega"},
    {0x03AA, "Iotadieresis"},
    {0x03AB, "Upsilondieresis"},
    {0x03AC, "alphatonos"},
    {0x03AD, "epsilontonos"},
    {0x03AE, "etatonos"},
    {0x03AF, "iotatonos"},
    {0x03B0, "upsilondieresistonos"},
    {0x03B1, "alpha"},
    {0x03B2, "beta"},
    {0x03B3, "gamma"},
    {0x03B4, "delta"},
    {0x03B5, "epsilon"},
    {0x03B6, "zeta"},
    {0x03B7, "eta"},
    {0x03B8, "theta"},
    {0x03B9, "iota"},
    {0x03BA, "kappa"},
    {0x03BB, "lambda"},
    {0x03BC, "mu"},
    {0x03BD, "nu"},
    {0x03BE, "xi"},
    {0x03BF, "omicron"},
    {0x03C0, "pi"},
    {0x03C1, "rho"},
    {0x03C2, "sigma1"},
    {0x03C3, "sigma"},
    {0x03C4, "tau"},
    {0x03C5, "upsilon"},
    {0x03C6, "phi"},
    {0x03C7, "chi"},
    {0x03C8, "psi"},
    {0x03C9, "omega"},
    {0x03CA, "iotadieresis"},
    {0x03CB, "upsilondieresis"},
    {0x03CC, "omicrontonos"},
    {0x03CD, "upsilontonos"},
    {0x03CE, "omegatonos"},
    {0x03D1, "theta1"},
    {0x03D2, "Upsilon1"},
    {0x03D5, "phi1"},
    {0x03D6, "omega1"},
    {0,0}
};


int add_utf8_char(unsigned char c, string &to) {
  if ( c & 128 ) { // 2 caratteri
    to+=char((c>>6)|(128+64));
    to+=char((c&(63))|128);
    return 2;
  } else {
    to+=char(c);
    return 1;
  }
}

string new_unicode(string &s) {
  string ret;
  for (int i=0;i<s.size();++i)
    add_utf8_char(s[i],ret);  
  return ret;
}

int myisalpha(unsigned char c) {
  return isalpha(c) || c>128+64;
}

Author *get_author(const string  &s) {
  Author *ret=new Author;

  int i;
  for (i=0;i+1<s.size() && 
	 !(isupper(s[i]) && (isupper(s[i+1])|| s[i+1]=='\'' ));i++);
  if (i+1==s.size()) {
    ret->Name=s;
    ret->firstName="";
  }
  else {
    ret->Name=s.substr(i);
    while (i>0 && isspace(s[i-1])) --i;
    ret->firstName=s.substr(0,i);
  }
  
  string &cog=ret->Name;

  // mette in minuscolo

  bool convert=false;
  for (i=0;i<cog.size();i++) {
    unsigned char c=cog[i];
    if ( (c & 128)==0) { // single utf8-char
      if (convert)
	cog[i]=tolower(cog[i]);
      convert=true;
      if (isspace(c) || c=='\'') convert=false;
    } else {
      assert((c>>2)==48); // solo 8 bit ammessi
      if (c==128+64+2+1 && convert) // i primi 2 bit sono 11
	cog[i+1] |=32; // setta il terzo bit
      ++i;
      convert=true;
    }
  }

  return ret;
}

void split_authors(string l, Head *head) {
  if (l.size()==0) return;
  string::size_type f;
  
  while (true) {
    f=l.find(',');
    
    if (f==string::npos) {
      head->author.push_back(get_author(l));
      return;
    }
    
    head->author.push_back(get_author(string(l,0,f)));
    while(f+1<l.size() && isspace(l[f+1])) f++;
    l=string(l,f+1);
  }
}


class SngPlugin:public Plugin {
private:
  static const bool dummy;
  static Plugin *plugin_creator() {
    Plugin *p=new SngPlugin();
    //    cerr<<"create SngPlugin: "<<p<<" ["<<p->name<<"]\n";     
  
    return new SngPlugin();
  }

  static bool plugin_startup() {
    Plugin::Register("sng",plugin_creator);
    return true;
  }

  map<string,int> unicode_name;
  
  void init_unicode() {
    int i;
    for (i=0;st_unicode_name[i].code;++i)
      unicode_name[st_unicode_name[i].name]=st_unicode_name[i].code;
    //cerr<<i<<" unicodes loaded\n";
  };

public:
  virtual int Read(istream &, vector<Song *> &list);
  SngPlugin(): Plugin("song","sng","song sng TeX-like format [manu-fatto]"),
	       current_in(0)
  {
    init_unicode();
    myreset();
  };
  
  void myreset() {
    nline=1;
  };


  /*READING INPUT FILE*/
  istream *current_in;
  int nline;
  int ncol;
  string token;

  char get() {
    int c;
    if (current_in->eof()) return 4;
    c=current_in->get();
    if (c==-1) return 4;
    ncol++;
    if (c==10) {nline++;ncol=0;};

    //    cerr<<"get: "<<c<<" --\n";
    return c;
  };

  void unget(char c) {
    current_in->putback(c);
  };

  void next_token() {
    token="";
    // legge il prossimo token
    char c=get();
    if (c=='\\') {
      token+=c;
      c=get();
      if (strchr("\\&%{}'`^~\",<",c)) { //comandi da 1 carattere
	token+=c;
      } else if (isalpha(c)) {
	do {
	  token+=c;
	} while (isalpha(c=get()));
	unget(c);
	
	strlwr(token);
	int i;
	for (i=0;commands[i] && ! 
	       (string(token,1)==commands[i]+1
		|| (token.size()==2 && token[1]==commands[i][0]));++i);
	if (commands[i]) token=string("\\")+commands[i][0];
	else error(string("unknown command '")+token+"'");
      } else {
	error(string("unrecognised command sequence '\\")+c+"'");
      }
    } else if (c=='%') {
      token+=c;
      while ((c=get())!='\n') token+=c;
      token+=c;
    } else if (c==13) { //CR "\r\n" -> "\n", "\r" -> "\n"
      token+='\n';
      int c=get();
      if (c!=10) unget(c);
    } else {
      token+=c;
    }
    //    cerr<<"token: >>"<<token<<"<< ("<<int(token[0])<<")\n";
  };
 
  void warning(string s) {
    cerr<<"WARNING: "<<s<<"\n";
  }

  void error(string s) {
    int i;
    //ostream &m=cerr;
    stringstream m; 
    m<<nline<<":"<<ncol<<" ERROR: "<<s<<"\n";
    throw runtime_error(m.str()); 
  };

  void skip_spaces(void) {
    assert(token.size());
    while (isspace(token[0]) || token[0]=='%') 
      next_token();
  };

  void skip_but(void) {
    while(token.size()==0 || (isspace(token[0]) && token[0]!=10))
      next_token();
  };

  void connect(istream &in) {
    if (current_in) {
      throw runtime_error(string("Concurrent reading not implemented\n"));
    }
    current_in=&in;
    myreset();
  }
  
  void disconnect() {
    current_in=0;
    myreset();
  }
  
  // legge una stringa. Si ferma se c'e` un comando sconosciuto.
  string readString(bool newline=true) {
    string ret;
    
    if (token[0]=='{') {
      next_token();
      ret=readString(false);
      if (token[0]!='}') error("'}' expected");
      next_token();
      return ret;
    }

    //    cerr<<"read string...["<<newline<<"]\n";

    while(true) {
      if (token[0]=='\\') { // accento?
	int i;
	if (strchr("\\{}&%",token[1])) {
	  ret+=token[1];
	  next_token();
	} else {
	  for (i=0;st_accents[i].c && st_accents[i].c!=token[1];++i);
	  if (st_accents[i].c==0) break; // forse e' un comando
	  string name;
	  name+="x";
	  name+=st_accents[i].name;
	  next_token(); // accento valido
	  if (!isalpha(token[0])) error("alphabetic character expected");
	  name[0]=token[0];
	  int c=unicode_name[name];
	  if (c==0) error("accent "+name+" not supported");
	  add_utf8_char(c,ret);
	  next_token();
	}
      } else if (isspace(token[0]) || token[0]=='%') {
	do {
	  char c=token[0];
	  if (newline && (c=='\n' || c=='%')) {
	    goto fine;
	  }
	  next_token();
	} while (isspace(token[0]));
	ret+=' ';
      } else if (strchr("&{}[]",token[0])) {
	break;
      } else if (token[0]==4) {
	break;
      } else {
	ret+=token;
	next_token();
      }
    }

  fine:

    //    cerr<<"...done ["<<ret<<"]\n";
    return ret;
  };
  
  Song *readSong() {
    Song *song=0;
    skip_spaces();
    
    if (token[0]==4) return 0; // fine del file;

    if (token!="\\t") error("'\\title' command expected, '"+token+"' found");
    next_token();
    skip_spaces();
    song=new Song;
    song->head=new Head;
    song->head->title=readString();
    skip_spaces();

    if (token=="\\a") {
      next_token();
      skip_spaces();
      string line=readString();
      split_authors(line,song->head);
    }
    skip_spaces();

    song->body=readBody();
    
    return song;
  };
  
  Body *readBody() {
    Body *body=0;
    Stanza *stanza;
    while((stanza=readStanza())!=0) {
      if (!body) body=new Body;
      body->stanza.push_back(stanza);
    }
    return body;
  }

  map<int,Stanza *> label;

  Stanza* readStanza() {
    bool newline=true;
    Stanza *stanza=0;
    if (token[0]=='\\' && strchr("srkb",token[1])) {
      stanza=new Stanza;
      switch(token[1]) {
      case 's': stanza->type=Stanza::STROPHE;break;
      case 'r': stanza->type=Stanza::REFRAIN;break;
      case 'k': stanza->type=Stanza::SPOKEN;newline=false;break;
      case 'b': stanza->type=Stanza::TAB;break;
      default: assert(false);
      }
      next_token();
      skip_spaces();

      // leggere label / copy / ref
      while (token=="\\l" || token=="\\y" || token=="\\f") {
	char c=token[1];
	int n=0;
	next_token();
	skip_spaces();
	{ 
	  int count=0;
	  while(isdigit(token[0])) {
	    n=n*10+token[0]-'0';
	    next_token();
	    count++;
	  }
	  if (count==0) error("number expected");
	}
	if (c=='l') label[n]=stanza;
	else {
	  Stanza *ref=label[n];
	  if (ref==0) error("label undefined");
	  if (c=='y') stanza->copy=ref;
	  else stanza->chords=ref;
	}
      }

      PhraseList *verse;
      while (verse=readVerse(newline)) {
	stanza->verse.push_back(verse);
      }
    } 
    return stanza;
  };

  PhraseList *readVerse(bool newline) {
    PhraseList *list;
    skip_spaces();
    list=readPhraseList(newline);
    if (token=="\\p") next_token();
    skip_spaces();
    return list;
  };
  
  PhraseList *readPhraseList(bool newline=true) {
    PhraseList *phrase=0;
    PhraseItem *item;
    while((item=readPhraseItem(newline))!=0) {
      if (!phrase) phrase=new PhraseList;
      phrase->list.push_back(item);
    }
    skip_spaces();
    return phrase;
  };

  PhraseItem *readPhraseItem(bool newline=true) {
//     if (token[0]=='{') {
//       next_token();
//       PhraseItem *ret=readPhraseItem(false);
//       if (token[0]!='}') error("'}' expected");
//       next_token();
//       return ret;
//     }
    
    if (token=="\\m" || token=="\\n") {
      char c=token[1];
      next_token();
      skip_spaces();
      Modifier *ret=new Modifier;
      if (c=='m') ret->attribute=Modifier::STRUM;
      else ret->attribute=Modifier::NOTES;
      c=token[0];
      if (c=='{') {
	next_token();
	ret->child=readPhraseList(false);
	if (token[0]!='}') error("'}` expected");
	next_token();
      } else {
	ret->child=new PhraseList;
	ret->child->list.push_back(readPhraseItem(false));
      }
      return ret;
    } 

    if (token[0]=='[') {
      next_token();
      Chord *ret=new Chord;
      ret->modifier=readString(false);
      if (token[0]!=']') error("']' expected");
      next_token();
      return ret;
    }
    
    if (token[0]=='&') {
      next_token();
      return new Tab;
    }

    string s=readString(newline);
    if (s.size()) {
      Word *ret=new Word;
      ret->word=s;
      return ret;
    }
    
    return 0;
  };

};

int SngPlugin::Read(istream &in, vector<Song *> &list) {
  Song *q;
  int n=0;
  connect(in);
  if (token.size()==0) next_token();
  while ((q=readSong())!=NULL) {
    list.push_back(q);
    n++;
  }
  disconnect();
  return n; 
}

const bool SngPlugin::dummy=SngPlugin::plugin_startup();
