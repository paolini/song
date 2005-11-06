#include <iostream>
#include <cassert>

#include "iso.h"
#include "psmedia.h"

#define CM * int(6000 / 2.54)
#define OUT (*out)

int PsMedia::lineSkip(font f) {return font_size[f]*100;};

/*larghezza in centesimi di punto dei caratteri da 32 a 127 ASCII font 10*/
/* roman, italic, bold */
static const int char_size[3][96]={
    {250,333,408,500,500,833,778,333,333,333,500,564,250,333,250,278,
     500,500,500,500,500,500,500,500,500,500,278,278,564,564,564,444,
     921,722,667,667,722,611,556,722,722,333,389,722,611,889,722,722,
     556,722,667,556,611,722,722,944,722,722,611,333,278,333,469,500,
     333,444,500,444,500,444,333,500,500,278,278,500,278,778,500,500,
     500,500,333,389,278,500,500,722,500,500,444,480,200,480,541,250},
    {250,333,420,500,500,833,778,333,333,333,500,675,250,333,250,278,
     500,500,500,500,500,500,500,500,500,500,333,333,675,675,675,500,
     920,611,611,667,722,611,611,722,722,333,444,667,556,833,667,722,
     611,722,611,500,556,722,611,833,611,556,556,389,278,389,422,500,
     333,500,500,444,500,444,278,500,500,278,278,444,278,722,500,500,
     500,500,389,389,278,500,444,667,444,444,389,400,275,400,541,250},
    {250,333,555,500,500,1000,833,333,333,333,500,570,250,333,250,278,
     500,500,500,500,500,500,500,500,500,500,333,333,570,570,570,500,
     930,722,667,722,722,667,611,778,778,689,500,778,667,944,722,778,
     611,778,722,556,667,722,722,1000,722,722,667,333,278,333,581,500,
     333,500,556,444,556,444,333,500,556,278,333,556,278,833,556,500,
     556,556,444,378,333,556,500,722,500,500,444,394,220,394,520,250}};

/*
  static char tab1[]={'`','\'','\"','^','~',',','\0'};
  static char tab2[]={"aeiouAEIOUnNcC"};
*/

static char *octal(int n) {
  static char s[4];
  
  if (n<0) n=256+n;

  assert(n<256 && n>=0);

  s[2]=n%8+'0';
  n/=8;
  s[1]=n%8+'0';
  n/=8;
  s[0]=n%8+'0';
  s[3]='\0';
  return s;
}

int PsMedia::char_width(char x) const {
  if (x<32 || x > 126) {
    x='X'; 
    if (x<-64-32) x='x';
  }
  return char_size[font_font[font_type]][x-32]*font_size[font_type]/10;
}

void PsMedia::char_out(char x) const {
  if (x=='(' || x==')' || x=='\\' || x<32 || x>126)  
    OUT<<"\\"<<octal(x);
  else
    OUT<<x;
}

static char *psnames[]={
  "grave",
  "acute",
  "circumflex",
  "tilde",
  "dieresis",
  "cedilla",
  ""};
static char *unicode_table[]={
  " " "a"   "e"   "i"   "o"   "u"   "A"   "E"   "I"   "O"   "U"   "nNcC",
  "`" "\xE0""\xE8""\xEC""\xF2""\xF9""\xC0""\xC8""\xCC""\xD2""\xD9""    ",
  "'" "\xE1""\xE9""\xED""\xF3""\xFA""\xC1""\xC9""\xCD""\xD3""\xDA""    ",
  "^" "\xE2""\xEA""\xEE""\xF4""\xFB""\xC2""\xCA""\xCE""\xD4""\xDB""    ",
  "~" "\xE3"" "   " "   "\xF5"" "   "\xC3"" "   " "   "\xD5"" "   "\xF1\xD1  ",
  "\"""\xE4""\xEB""\xEF""\xF6""\xFC""\xC4""\xCB""\xCF""\xD6""\xDC""    ",
  "," " "   " "   " "   " "   " "   " "   " "   " "   " "   " "   "  \xE7\xC7",
  0};

PsMedia::PsMedia(ostream &the_out, int start_page): 
  VectorMedia(),
  x(0), y(0), chord_x(0),
  out(&the_out), 
  font_type(NOFONT),
  page(start_page-1) {



  OUT<<
    "%!PS-Adobe-2.0\n"
    "%%Creator: corda, manu-fatto\n"
    "%%BoundingBox: 0 0 "<<my_width/100<<" "<<my_height/100<<"\n"
    "%%DocumentPaperSizes: A4\n"
    "%%EndComments\n"
    "%%EndProlog\n"
    "\n"
    "%%  PREAMBLE\n\n"
    "/cm {72 2.54 div mul} def\n"
    // goto_xy
    "/left "<<left<<" def\n" //margine sinistro
    "/bottom "<<bottom<<" def\n"  //margine inferiore
    "/cx 0 def\n" //
    "/g {100 div bottom add exch 100 div left add exch moveto} def\n"
    "/m {100 div exch 100 div exch rmoveto} def\n"
    "/s {show} def\n" // word write
    "/c {/mys exch def currentpoint /myy exch def /myx exch def\n"
    "    currentpoint pop cx "<<char_width(' ')<<" 100 div add max \n"
    "    currentpoint exch pop "<<lineSkip(NORMAL)<<" 100 div add moveto mys show\n"
    "    currentpoint pop /cx exch def"
    "    myx myy moveto} def\n" // chord write //da finire!!!
    "/creset {/cx currentpoint pop def} def\n"
    ;

  OUT<<
    "/reencsmalldict 14 dict def\n"
    "/ReEncodeSmall\n"
    "{reencsmalldict begin\n"
    "/newcodesandnames exch def\n"
    "/newfontname exch def\n"
    "/basefontname exch def\n"
    "/basefontdict basefontname findfont def\n"
    "/newfont basefontdict maxlength dict def\n"
    "basefontdict\n"
    "{exch dup /FID ne {dup /Encoding eq\n"
    "{exch dup length array copy\n"
    "newfont 3 1 roll put }\n"
    "{exch newfont 3 1 roll put}\n"
    "ifelse\n"
    "}{pop pop }\n"
    "ifelse }forall\n"
    "newfont /FontName newfontname put\n"
    "newcodesandnames aload pop\n"
    "newcodesandnames length 2 idiv\n"
    "{newfont /Encoding get 3 1 roll put}\n"
    "repeat\n"
    "newfontname newfont definefont pop\n"
    "end\n"
    "}def\n"
    "/scandvec[\n";
  for (int i=1;unicode_table[i];i++)
    for (int j=1;unicode_table[i][j];j++) {
      if (unicode_table[i][j]!=' ')
	OUT<<"8#"<<octal(unicode_table[i][j])
	   <<" /"<<unicode_table[0][j]<<psnames[i-1]<<"\n";
  } 
 OUT<<
   // normal, refrain, chord, title, author, nofont
   "] def\n";

 const char *name[]={"Roman","Italic","Bold"};
 for (int f=0;f!=NOFONT;++f) {
   OUT<<
     "/Times-"<<name[font_font[f]]<<" /T"<<f<<" scandvec ReEncodeSmall\n"
     "/F"<<f<<" {/T"<<f<<" findfont "<<font_size[f]<<
     " scalefont setfont} def\n";
 }
 // set_font(NORMAL);
 pageStart();
}

void PsMedia::set_font(font f) {
  if (f!=font_type) {
      font_type=f;
      OUT<<"F"<<f<<"\n";
    }
  }

PsMedia::~PsMedia() {
  pageEnd();
  OUT<<"\n%%Trailer\n";
  OUT<<"\n%%EOF\n";
}

void PsMedia::pageStart() {
  page++;
  OUT<<"%% START_OF_PAGE "<<page<<"\n";
  OUT<<"\n%%Page: "<<page<<" "<<page<<"\n";
  x=0;y=0;chord_x=0;
  OUT<<"0 0 g\n";
  font_type=NOFONT;
  set_font(NORMAL);
}

void PsMedia::pageEnd() {
  const Media::font f=AUTHOR;
  set_font(f);
  OUT<<my_width/2<<" "<<-100*(font_size[NORMAL]+font_size[AUTHOR])<<" g ("<<page<<") show\n";
  OUT<<"showpage\n"
     <<"%% END_OF_PAGE\n";
}

void PsMedia::newPage() {
  pageEnd(); pageStart();
}

void PsMedia::goto_xy(int x, int y) {
  OUT<<x<<" "<<y<<" g\n";
  this->x=x;
  this->y=y;
}

void PsMedia::move_xy(int dx, int dy) {
  OUT<<dx<<" "<<dy<<" m\n";
  x+=dx;
  y+=dy;
}

int PsMedia::wordWidth(const string &s, font f) const {
  int len=0;
  size_t i;
  for (i=0;i<s.size();) len+=char_width(iso(s,i));
  return len*font_size[f]/10;
}

void PsMedia::wordWrite(const string &s, font f) {
  set_font(f);
  OUT<<"(";
  for (unsigned int i=0;i<s.size();) char_out(iso(s,i));
  OUT<<") s\n";
  x+=wordWidth(s,f);
}

void PsMedia::chordWrite(const string &s) {
  set_font(CHORD);
  OUT<<"(";
  for (unsigned int i=0;i<s.size();) char_out(iso(s,i));
  OUT<<") c\n";
}

void PsMedia::chordReset() {
  OUT<<"creset\n";
};

void PsMedia::frame(int x,int y) const {
  OUT<<
    "%% FRAME "<<x<<" "<<y<<"\n"
    "1 setlinewidth\n"
    "currentpoint /savey exch def /savex exch def\n"
    "currentpoint\n"
    "newpath moveto\n"
     <<x/100<<" 0 rlineto\n"
    "0 "<<y/100<<" rlineto\n"
     <<-x/100<<" 0 rlineto\n"
    "closepath stroke\n"
    "savex savey moveto\n\n";
};

