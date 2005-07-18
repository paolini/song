#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "songplugin.hh"

using namespace std;

static void error(char *s) {
  fprintf(stderr,"ERROR: %s\n",s);
  abort();
}

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

char *two_voices="&";

/* OPERAZIONI SULLE STRINGHE */
void strdel(char *s)
{
  for (;*s!='\0';s++) *s=*(s+1);
}
void strsubst(char *s,char from,char to)
{
  for (;*s!='\0';s++)
    if (*s==from) *s=to;
}

/*READING INPUT FILE*/
FILE *current_fp=0;

void errore(char *);


#define MAXLINE 256
static int nriga;
static char buff[MAXLINE+1];
static char *buffp; /*primo carattere da leggere*/

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
      if ((buffp=fgets(buff,MAXLINE/2,current_fp))==0)
	{
	  buff[0]=-1;buff[1]='\0';
	  buffp=buff;
	}
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

void errore(char *s)
{
  int i;
  if (nriga!=-1)
    fprintf(stderr,"\nErrore alla riga %d (%s): \n",nriga,s);
  else
    fprintf(stderr,"\nErrore: \n");
  fprintf(stderr,"%s\n",buff);
  for (i=0;buff+i<buffp;i++)
    fprintf(stderr," ");
  fprintf(stderr,"^- Qui.\n");
  error(s);
}
void fatal(char *error)
{
  errore(error);
  //fatal_error(error);
  abort();
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

static char *isoacc[]={
  " " "a"   "e"   "i"   "o"   "u"   "A"   "E"   "I"   "O"   "U"   "nNcC",
  "`" "\xE0""\xE8""\xEC""\xF2""\xF9""\xC0""\xC8""\xCC""\xD2""\xD9""    ",
  "'" "\xE1""\xE9""\xED""\xF3""\xFA""\xC1""\xC9""\xCD""\xD3""\xDA""    ",
  "^" "\xE2""\xEA""\xEE""\xF4""\xFB""\xC2""\xCA""\xCE""\xD4""\xDB""    ",
  "~" "\xE3"" "   " "   "\xF5"" "   "\xC3"" "   " "   "\xD5"" "   "\xF1\xD1  ",
  "\"""\xE4""\xEB""\xEF""\xF6""\xFC""\xC4""\xCB""\xCF""\xD6""\xDC""    ",
  "," " "   " "   " "   " "   " "   " "   " "   " "   " "   " "   "  \xE7\xC7",
  ""};

int add_utf8_char(unsigned char c,char *to) {
  if ( c & 128 ) { // 2 caratteri
    *(to++)=(c>>6)|(128+64);
   
    *(to++)=(c&(63))|128;
    return 2;
  } else {
    *(to++)=c;
    return 1;
  }
}

static int spoken;
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
	    if (isoacc[j][0]==0 || isoacc[0][i]==0) {
	      fprintf(stderr,"Codifica %c del carattere %c non supportata\n",
		      y,z);
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
    error("carattere ascii non sopportato");
  }
  *c=x;
  return 1;
}

xmlChar *readline(void)
{
  int n=0;
  int alloc=0;
  char *p=0;
  char c;
  for (n=0;readchar(&c);) {
    while (n+2>=alloc) {
      alloc+=8;
      // DA FARE: utilizzare xmlAlloc
      p=(char *)realloc(p,sizeof(char)*alloc);
    }
    n+=add_utf8_char(c,p+n);
  }
//   if (n==0) {
//     c=mygetc();
//     if (c=='&') {
// 	abort(); // da implementare!
// /* 	ps=two_voices; */
// /* 	pctrl=two_voices; */
// 	skip_but();
//     } else {
//       myungetc(c);
//       return 0;
//     }
//   }
//   else
  if (n) {
    p[n]=0;
    return BAD_CAST p;
  }
  else return 0;
}

xmlChar *xmlreadline() {
  return BAD_CAST readline();
}

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
      assert(0);
      return 10; /*non dovrebbe mai arrivare qui*/
    }
}
xmlNodePtr readsequenza(int level,xmlNodePtr Parent);

static char *xml_phrase_name[6]={"","c","note","strum","part","chordref"};
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
      errore("Tabulazione non permessa qui");
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
  xmlChar *l;
  xmlNodePtr f;
  if ((l=readline())!=NULL) {
    return xmlNewText(l);
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
  xmlNodePtr Stanza=xmlNewNode(NULL,BAD_CAST "p");
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
    xmlNewProp(Stanza, BAD_CAST "label", BAD_CAST s);
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
xmlChar *readlinea(void)
{
  char c;
  int i;
  xmlChar *p;
  skip_spaces();
  c=mygetc();
  if (c!='{') myungetc(c);
  skip_spaces();
  p=readline();
  if (p!=NULL && *p!='\0') {
    for (i=0;p[i];++i);
    for (;i>=0 && p[i]<128 && isspace(p[i]);i--);
    p[i+1]='\0';
  }
  if (c=='{')
    {
      skip_spaces();
      c=mygetc();
      if (c!='}')
	errore("Mi aspettavo una '}'");
    }
  return BAD_CAST p;
}

void split_authors(const xmlChar *l, xmlNodePtr Head)
{
  const xmlChar *p;
  if (l==NULL) return;
  p=xmlStrchr(l,',');
  if (p==NULL) {
    xmlNewChild(Head,NULL,BAD_CAST "author", l);
    xmlAddChild(Head,xmlNewText(BAD_CAST"\n"));
  }
  else
    {
      xmlChar *x=xmlCharStrndup((const char *)l,p-l);
      for (p++;p!='\0' && isspace(*p);p++);
      xmlNewChild(Head,NULL,BAD_CAST "author", x);
      xmlAddChild(Head,xmlNewText(BAD_CAST"\n"));
      xmlFree(x);
      split_authors(p,Head);
    }
}

xmlNodePtr readsong(FILE *fp)	{
  char c;
  char esci;
  xmlNodePtr Song=xmlNewNode(NULL,BAD_CAST "song");
  if (current_fp) {
    fprintf(stderr,"Concurrent reading not implemented!\n");
    abort();
  }
  current_fp=fp;
  myreset();
  
  skip_spaces();
  xmlNodePtr Head=xmlNewNode(NULL,BAD_CAST "head");
  xmlAddChild(Head,xmlNewText(BAD_CAST"\n"));

  skip_spaces();
  buff_check();
  if (!(buff[0]=='\\' && (buff[1]=='t' || buff[1]=='T'))) {
    current_fp=0;
    return NULL;
  }
  if (mygetc()!='\\')
    errore("Mi aspettavo un '\\'");
  c=getcommand();
  if (c!='t')
    errore("Mi aspettavo un titolo");
  xmlChar* line=readlinea();
  xmlNewChild(Head,NULL, BAD_CAST "title", line);
  xmlFree(line);
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
		xmlFree(line);
	      break;
	    case 'o':
	      fprintf(stderr,"\\o offset command not implemented");
	      xmlFree(readlinea());
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
		  error("unknown header command");
		}
		line=readlinea();
		xmlNewChild(Head,NULL,BAD_CAST xmlcmd[i]+1,line);
		xmlFree(line);
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
      current_fp=0;
      return NULL;
    }
  current_fp=0;
  xmlAddChild(Song,Head);
  xmlAddChild(Song,xmlNewText(BAD_CAST"\n"));
  xmlAddChild(Song,Body);
  xmlAddChild(Song,xmlNewText(BAD_CAST"\n"));
  return Song;
}
void readend(void)
{
  skip_spaces();
  if (!feof(current_fp))
    errore("Pensavo che il file fosse finito");
}

int song_read(FILE *fp, vector<xmlNodePtr> &list) {
  xmlNodePtr q;
  int n=0;
  while ((q=readsong(fp))!=NULL) {
    list.push_back(q);
    n++;
  }
  return n; 
}

int song_read(char *filename, vector<xmlNodePtr> &list) {
  FILE *fp;
  int n=0;
  fp=fopen(filename,"r");
  if (fp) {
    n=song_read(fp,list);
    fclose(fp);
  } else {
    fprintf(stderr,"cannot open file %s\n",filename);
    abort();
  }
  return n;
}

