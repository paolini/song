#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cassert>
#include <sstream>
#include <vector>
#include <libxml/tree.h>

using namespace std;

#include "plug.h"

// class SngPlugin:public Plugin {
// private:
//   static const bool dummy;
//   static bool plugin_startup();

//   static const int MAXLINE;
//   istream *current_in;
//   int nriga;
//   char *buff;
//   char *buffp;
// public:
//   virtual int Read(std::istream &, std::vector<xmlNodePtr> &list);
//   SngPlugin(): 
//     Plugin("song","sng","song sng TeX-like format [manu-fatto]"),
//     current_in(0)
//   {buff=new char[MAXLINE+1];};
//   virtual ~SngPlugin() {delete buff;};

//   friend class Sng;
// };

// utility functions

char *strlwr(char *s)
{
  for (;*s!='\0';s++) *s=tolower(*s);
  return s;
}
char *strupr(char *s)
{
  for (;*s!='\0';s++) *s=toupper(*s);
  return s;
}

void strdel(char *s)
{
  for (;*s!='\0';s++) *s=*(s+1);
}
void strsubst(char *s,char from,char to)
{
  for (;*s!='\0';s++)
    if (*s==from) *s=to;
}

static char *commands[]={
  "ssong",
  "ttitle","aauthor",
  "xkeys","gpage","ooffset",
  "sstrophe","rrefrain","kspoken","btab","nnotes",
  "llabel","ycopy","frefer",
  "cchords","mstrum",""};
static char *command_msg[]=
  {"ttitolo","aautore","xchiavi","gpagina","ooffset",NULL};
static char *xmlcmd[]=
  {"ttitle","aauthor","xkeyword","gpage","ooffset",NULL};

char *head_name(char type)
{
  int i;
  for (i=0;command_msg[i]!=NULL && command_msg[i][0]!=type;i++);
  if (command_msg[i]!=NULL) return command_msg[i]+1;
  else return "Errore!";
}
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

int add_utf8_char(unsigned char c,xmlChar *to) {
  if ( c & 128 ) { // 2 caratteri
    *(to++)=(c>>6)|(128+64);
   
    *(to++)=(c&(63))|128;
    return 2;
  } else {
    *(to++)=c;
    return 1;
  }
}

xmlNodePtr new_unicode(char *s) {
  xmlChar *ret=(xmlChar *)xmlMalloc(sizeof(xmlChar)*2*strlen(s)+1);
  xmlChar *p;
  p=ret;
  for (int i=0;s[i];++i)
    p+=add_utf8_char((xmlChar) s[i],p);
  *p=0;
  xmlNodePtr x=xmlNewText(ret);
  xmlFree(ret);
  return x;
}

const char *surname(const char *s) {
  const char *p;
  for (p=s;!( (isupper(*p)&& (isupper(*(p+1))||*(p+1)=='\'' )) || *p=='\0');p++);
  if (*p=='\0') return s;
  else return p;
}


int myisalpha(unsigned char c) {
  return isalpha(c) || c>128+64;
}
char* get_author(const char *s,const char *end) {
  char *ret=(char *)malloc(sizeof(char)*(end-s+4));
  const char *cog=surname(s);
  const char *p;
  char *r;
  r=ret;
  for (p=cog;p<end;++p) {
    if (p!=cog && myisalpha(*p) && myisalpha(*(p-1))) {
      unsigned char c=*p;
      if (c>=128+64 && c<128+64+32) {
	c+=32;
      } else c=tolower(*p);
      *(r++)=c;
    }
    else *(r++)=*p;
  }
  while (r>s && isspace(*(r-1))) r--;
  if (cog>s)
    *(r++)=','; *(r++)=' ';
  for (p=s;p<cog;++p)
    *(r++)=*p;
  while (r>ret && isspace(*(r-1))) r--;
  *r=0;
  //  cerr<<" AUTHOR ["<<ret<<"] ("<<int(*(r-1))<<")\n";
  
  return ret;
}

void split_authors(const char *l, xmlNodePtr Head)
{
  const char *p;
  if (l==NULL) return;
  p=strchr(l,',');
  if (p==NULL) {
    for (p=l;*p;++p);
  }
  char *r=get_author(l,p);
  xmlNodePtr Author=xmlNewNode(NULL,BAD_CAST "author");
  xmlAddChild(Author,new_unicode(r));
  free(r);r=0;
  xmlAddChild(Head,Author);
  
  if (p && *p) {
    for (p++;p!='\0' && isspace(*p);p++);
    split_authors(p,Head);
  }
}


static char *xml_phrase_name[6]={"","c","note","strum","part","chordref"};

///////////////

class SngPlugin:public Plugin {
private:
  static const bool dummy;
  static Plugin *plugin_creator() {
    return new SngPlugin();
  }

  static bool plugin_startup() {
    Plugin::Register("sng",plugin_creator);
    return true;
  }
  
public:
  virtual int Read(istream &, vector<xmlNodePtr> &list);
  SngPlugin(): Plugin("song","sng","song sng TeX-like format [manu-fatto]"),
    current_in(0)
  {
    myreset();
  };
  

  /*READING INPUT FILE*/
  istream *current_in;
  

#define MAXLINE 1023
  int nriga;
  char buff[MAXLINE+1];
  char *buffp; /*primo carattere da leggere*/
  int spoken;


  /* all'inizio si deve avere nriga=0, buff[0]='\0', buffp=buff;*/
  void myreset(void) {
    buff[0]=0;
    buffp=buff;
    nriga=0;
  }

  void buff_check(void)
  {
    if (*buffp=='\0')
      {
	//      position=ftell(current_fp);
	current_in->getline(buff,MAXLINE/2,'\n');
	if (!(*current_in))
	  {
	    buff[0]=-1;buff[1]='\0';
	  } else {
	    buffp=strchr(buff,0);
	    if (buffp-buff >MAXLINE/2-5) {
	      errore("linea troppo lunga!");
	    }
	    *(buffp++)='\n';
	    *buffp=0;
	  }
	buffp=buff;
	nriga++;
      }
  }
  
  int mygetc(void)
  {
    int c;
    
    buff_check();
    c=*(buffp++);
    if (c==-1)
      {
	return EOF;
      }
    return (unsigned char) c;
  }
  
  void myungetc(char c)
  {
    if (buffp>buff)
      *(--buffp)=c;
    else
      errore("Can't put back character...");
  }
  
  void errore(string s) {
    int i;
      //ostream &m=cerr;
      stringstream m; 
      if (nriga!=-1)
	m<<"Errore alla riga "<<nriga<<" :"<<s<<"\n";
      else
	m<<"Errore: "<<s<<"\n";
      m<<""<<buff<<"\n";
      for (i=0;buff+i<buffp;i++)
	m<<" ";
      m<<"\\_ Qui.\n";
    throw runtime_error(m.str()); 
  }

  void skip_spaces(void)
  {
    char c;
    while (isspace(c=mygetc()));
    if (c=='%')
      {
	do
	  {c=mygetc();} while (c!='\n' && c!=EOF);
	skip_spaces();
      }
    else
      myungetc(c);
  }
  void skip_but(void)
  {
    char c;
    do
      {
	c=mygetc();
      } while (isspace(c)&&c!='\n');
    myungetc(c);
  }
  char getcommand(void)
  {
    int n,i;
    static char buf[16];
    for (n=0;n+1<16 && isalpha(buf[n]=mygetc());n++);
    if (n+1<16)
      {
	myungetc(buf[n]);
	buf[n]='\0';
	strlwr(buf);
	for (i=0;commands[i][0]!='\0';i++)
	  {
	    if (n==1?buf[0]==commands[i][0]:!strcmp(buf,commands[i]+1))
	      return commands[i][0];
	  }
      }
    return 0;
  }
  
  int readchar(char *c)
  {
    int x,y,z;
    x=mygetc();
    if (spoken && x=='\n') x=' ';
    switch(x)
      {
      case '\\':
	y=mygetc();
	switch(y)
	  {
	  case '\\':
	  case 'p':
	  case 'P':
	    myungetc('\n');
	    return 0;
	  case '{':
	  case '}':
	  case '[':
	  case ']':
	  case '&':
	    *c=y;
	    return 1;
	  case '`':
	  case '\'':
	  case '\"':
	  case '^':
	  case '~':
	  case ',': /*cedilla*/
	  case '<': /* \v del TeX */
	    {
	      z=mygetc();
	      int i,j;
	      for (i=0;isoacc[0][i] && isoacc[0][i]!=z;++i);
	      for (j=1;isoacc[j][0] && isoacc[j][0]!=y;++j);
	      if (isoacc[j][0]==0 || isoacc[0][i]==0 || isoacc[j][i]==' ') {
		//stringstream m;
		cerr<<"Codifica <"<<char(y)<<"> del carattere <"<<char(z)<<"> non supportata\n";
		//errore(m.str()); //non torna!
		*c=z;
		return 1;
	      }
	      *c=isoacc[j][i];
	      return 1;
	    }
	  default:
	    myungetc(y);
	    myungetc('\\');
	    *c='\0';
	    return 0;
	  }
      case '%':
      case '{':
      case '}':
      case '[':
      case ']':
      case EOF:
	myungetc(x);
	return 0;
      case '&':
      case '\n':
	myungetc(x);
	return 0;
      case ' ':
      case '\t':
      case 13:
	y=x;
	while (y==' ' || y=='\t' || y==13) y=mygetc();
	myungetc(y);
	if (y=='&')
	  return readchar(c);
	else if (y=='\n')
	  return 0;
	else
	  *c=' ';
	return 1;
      case 0xe8: //e`
      case 0xf9: //u`
      case 0xec: //i`
      case 0xe0: //a`
	*c=x;
	return 1;
      }
    if (x > 127 || x<0) {
      errore("carattere ascii non sopportato");
    }
    *c=x;
    return 1;
  }

  char *readline(void)
  {
    int n=0;
    int alloc=0;
    char *p=0;
    char c;
    for (n=0;readchar(&c);) {
      while (n+2>=alloc) {
	alloc+=8;
	p=(char *)realloc(p,sizeof(char)*alloc);
      }
      p[n++]=c;
    }
    if (n) {
      p[n]=0;
      return p;
    }
    else return 0;
  }

  // xmlChar *xmlreadline() {
  //   return BAD_CAST readline();
  // }

  int commandlevel(char c)
  {
    switch(c)
      {
      case 't':
	return 5;
      case 'r':
      case 's':
      case 'k':
      case 'b':
	return 4;
      case 'm':
	return 3;
      case 'n':
	return 2;
      case 'c':
	return 1;
      default:
	errore("comando sconosciuto");
	return 10; /*non dovrebbe mai arrivare qui*/
      }
  }
  //  xmlNodePtr readsequenza(int level,xmlNodePtr Parent);

  xmlNodePtr readcommand(int level)
  {
    xmlNodePtr p;
    char c;
    int l;
    skip_but();
    c=mygetc();
    if (c=='\\')
      {
	c=getcommand();
	if ((l=commandlevel(c))>=level)
	  {
	    if (l==10) return NULL;
	    myungetc(c);
	    myungetc('\\');
	    return NULL;
	  }
	p=xmlNewNode(NULL,BAD_CAST xml_phrase_name[l]);
	if (!readsequenza(l,p)) {
	  xmlFreeNode(p);
	  p=0;
	}
	return p;
      }
    if (c=='[')
      {
	p=xmlNewNode(NULL,BAD_CAST xml_phrase_name[1]);
	if (!readsequenza(1,p)) {
	  xmlFreeNode(p);
	  p=0;
	}
	skip_but();
	if (mygetc()!=']')
	  errore("Ci doveva essere una ']'");
	return p;
      }
    if (c=='&') {
      if (level!=4) {
	myungetc(c);
	return NULL;
      }
      p=xmlNewNode(NULL, BAD_CAST "tab");
      return p;
    }
    myungetc(c);
    return NULL;
  }

  xmlNodePtr readphrase(int level) /*1:accordo,...,4:part*/
  {
    char *l;
    xmlNodePtr f;
    if ((l=readline())!=NULL) {
      f=new_unicode(l);
      free(l);
      return  f;
    }
    f=readcommand(level);
    return f;
  }

  xmlNodePtr readsequenza(int level,xmlNodePtr Parent)
  {
    int count=0;
    char c;
    skip_but();
    c=mygetc();
    if (c=='{')
      {
	skip_spaces();
	xmlNodePtr Child;
	while ((Child=readphrase(level))!=NULL) {
	  xmlAddChild(Parent,Child);
	  count++;
	}
	skip_spaces();
	if (mygetc()!='}')
	  errore("Mi aspettavo una '}'");
      }
    else
      {
	myungetc(c);
	xmlNodePtr Child;
	while ((Child=readphrase(level))!=NULL) {
	  xmlAddChild(Parent,Child);
	  count++;
	}
      }
    if (count)
      return Parent;
    else 
      return NULL;
  }

  xmlNodePtr readverse(void)
  {
    int count=0;
    xmlNodePtr Verse=NULL;

    skip_spaces();
    Verse=xmlNewNode(NULL, BAD_CAST "v");
    while ((readsequenza(4,Verse))!=NULL) {count++;};
    if (!count) {
      xmlFreeNode(Verse);
      Verse=0;
    }
    return Verse;
  }

  int getint(void)
  {
    int r=0;
    char c;
    char ok=0;
    skip_spaces();
    while (isdigit(c=mygetc()))
      {
	ok=1;
	r*=10;
	r+=c-'0';
      }
    myungetc(c);
    if (ok)
      return r+1;
    errore("Ci doveva essere un numero intero");
    return 0;
  }
  int readlabel(int *label,int *ref)
  {
    char c;
    char ok=1;
    *label=0;
    *ref=0;
    do
      {
	skip_spaces();
	c=mygetc();
	if (c=='\\')
	  {
	    c=getcommand();
	    switch(c)
	      {
	      case 'l':
		if (*label==0)
		  *label=getint();
		else
		  {
		    errore("Era gia` stata definita la label");
		    return 0;
		  }
		break;
	      case 'f':
		if (*ref==0)
		  *ref=-getint();
		else
		  {
		    errore("Era gia` stato definito il riferimento");
		    return 0;
		  }
		break;
	      case 'y':
		if (*ref==0)
		  *ref=getint();
		else
		  {
		    errore("Era gia` stato definito il testo");
		    return 0;
		  }
		break;
	      case 0:
		ok=0;
		myungetc('\\');
		break;
	      default:
		ok=0;
		myungetc(c);
		myungetc('\\');
		break;
	      }
	  }
	else
	  {
	    myungetc(c);
	    ok=0;
	  }
	if (*label!=0 && *ref!=0)
	  ok=0;
      }while(ok);
    return 1;
  }

  xmlNodePtr readonepart(void)
  {
    xmlNodePtr Stanza=xmlNewNode(NULL,BAD_CAST "stanza");
    char c;
    int type;
    int lab=0,ref=0;
    skip_spaces();
    if ((c=mygetc())!='\\')
      {
	myungetc(c);
	return NULL;
      }
    spoken=0;
    c=getcommand();
    switch(c)
      {
      case 'r':
	xmlNewProp(Stanza, BAD_CAST "type", BAD_CAST "refrain");
	type=0;
	break;
      case 's':
	type=1;
	break;
      case 'k':
	xmlNewProp(Stanza, BAD_CAST "type", BAD_CAST "talking");
	type=2;
	spoken=1;
	break;
      case 'b':
	xmlNewProp(Stanza, BAD_CAST "type", BAD_CAST "tabular");
	type=3;
	break;
      case 0:
	errore("Comando sconosciuto");
	return NULL;
      default:
	myungetc(c);
	myungetc('\\');
	return NULL;
      }
    readlabel(&lab,&ref);
    if (lab) {
      char s[2];
      assert(lab>0);
      s[0]='A'+lab-1;
      assert(s[0]<='Z');
      s[1]=0;
      xmlNewProp(Stanza, BAD_CAST "id", BAD_CAST s);
    }
    if (ref) {
      char s[2];
      char *name="chords";
      if (ref<0) {
	name="copy";
	ref=-ref;
      }
      s[0]='A'+ref-1;
      assert(s[0]<='Z');
      s[1]=0;
      xmlNewProp(Stanza, BAD_CAST name, BAD_CAST s);
    }
    xmlNodePtr Verse;
    int count=0;
    while ((Verse=readverse())!=NULL) {
      xmlAddChild(Stanza,xmlNewText(BAD_CAST "\n"));
      xmlAddChild(Stanza,Verse);
      count++;
    }
  
    if (count==0)
      {
	errore("Non c'e' il testo...");
	return NULL;
      }
    return Stanza;
  }

  xmlNodePtr readpart(void)
  {
    int count=0;
    xmlNodePtr Body,Stanza;
    Body=xmlNewNode(NULL, BAD_CAST "body");
    while ((Stanza=readonepart())!=NULL) {
      xmlAddChild(Body,xmlNewText(BAD_CAST "\n"));
      xmlAddChild(Body,Stanza);
      count++;
    }
    return Body;
  }

  xmlNodePtr readmain(void)
  {
    char c;
    //  struct part *p;
    xmlNodePtr Body;

    skip_spaces();
    Body=readpart();

    skip_spaces();
    c=mygetc();
    if (c==EOF)
      myungetc(c);
    else if (c=='\\')
      {
	c=mygetc();
	if (c!='t' && c!='T')
	  errore("Pensavo che la canzone fosse finita");
	myungetc(c);
	myungetc('\\');
      }
    else
      {
	errore("Pensavo che la canzone fosse finita");
	myungetc(c);
      }
    return Body;
  }
  char *readlinea(void)
  {
    char c;
    int i;
    char *p;
    skip_spaces();
    c=mygetc();
    if (c!='{') myungetc(c);
    skip_spaces();
    p=readline();
    if (p!=NULL && *p!='\0') {
      for (i=0;p[i];++i);
      for (;i>=0 && p[i]>=0 /* && p[i]<128*/ && isspace(p[i]);i--);
      p[i+1]='\0';
    }
    if (c=='{')
      {
	skip_spaces();
	c=mygetc();
	if (c!='}')
	  errore("Mi aspettavo una '}'");
      }
    return p;
  }

  void readend(void)
  {
    skip_spaces();
    if (!(current_in->eof()))
      errore("Pensavo che il file fosse finito");
  }

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

  xmlNodePtr readsong()	{
    char c;
    char esci;
    xmlNodePtr Song=xmlNewNode(NULL,BAD_CAST "song");
  
    skip_spaces();
    xmlNodePtr Head=xmlNewNode(NULL,BAD_CAST "head");
    xmlAddChild(Head,xmlNewText(BAD_CAST"\n"));

    skip_spaces();
    buff_check();
    if (!(buff[0]=='\\' && (buff[1]=='t' || buff[1]=='T'))) {
      current_in=0;
      return NULL;
    }
    if (mygetc()!='\\')
      errore("Mi aspettavo un '\\'");
    c=getcommand();
    if (c!='t')
      errore("Mi aspettavo un titolo");
    char* line=readlinea();

    xmlNodePtr Title=xmlNewNode(NULL,BAD_CAST "title");
    xmlAddChild(Title,new_unicode(line));
    free(line); line=0;
    xmlAddChild(Head,Title);
    xmlAddChild(Head,xmlNewText(BAD_CAST"\n"));

    do
      {
	esci=1;
	skip_spaces();
	c=mygetc();
	if (c=='\\')
	  {
	    esci=0;
	    c=getcommand();
	    switch(c)
	      {
	      case 'a':
		line=readlinea();
		split_authors(line,Head);
		free(line);
		break;
	      case 's':
	      case 'r':
	      case 'k':
	      case 'b':
		myungetc(c);
		myungetc('\\');
		esci=1;
		break;
	      case 't':
		myungetc(c);
		myungetc('\\');
		esci=1;
		break;
	      default:
		{
		  int i;
		  for (i=0;xmlcmd[i] && xmlcmd[i][0]!=c;++i);
		  if (xmlcmd[i]==0) {
		    errore("unknown header command");
		  }
		  line=readlinea();
		  xmlNodePtr Other=xmlNewNode(0,BAD_CAST xmlcmd[i]+1);
		  xmlAddChild(Other,new_unicode(line));
		  free(line);line=0;
		  xmlAddChild(Head,Other);
		}
		break;
	      }
	  }
	else
	  {
	    myungetc(c);
	  }
      } while (!esci);

    xmlNodePtr Body=readmain();
    if (Body==NULL)
      {
	fprintf(stderr,"C'e` stato un errore alla riga %d\n",nriga);
	errore("Non sono riuscito a leggere la canzone");
	//      freesong(p);
	current_in=0;
	return NULL;
      }
    current_in=0;
    xmlAddChild(Song,Head);
    xmlAddChild(Song,xmlNewText(BAD_CAST"\n"));
    xmlAddChild(Song,Body);
    xmlAddChild(Song,xmlNewText(BAD_CAST"\n"));
    return Song;
  }
};

int SngPlugin::Read(istream &in, vector<xmlNodePtr> &list) {
  xmlNodePtr q;
  int n=0;
  connect(in);
  while ((q=readsong())!=NULL) {
    list.push_back(q);
    n++;
  }
  disconnect();
  return n; 
}

const bool SngPlugin::dummy=SngPlugin::plugin_startup();
