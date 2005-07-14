#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "song.h"
#include "chords.h"
#include "string.h"


char *lang_list[]={"con","html","sng","ps","src","txt","list","xng",NULL};
char *lang_desc[]={"console","html","song","PostScript.ps","sorgente.sng","FileTesto.txt",NULL};
char *chord_list[]={"off","on","ref","copyoff","copy","copyref",NULL};
int chord_value[]={0,1,3,4,5,7};
char *chord_desc[]={
  "Senza accordi","Solo accordi segnati","Con riferimenti",
  "Copie senza accordi","Mette copie","Mette copie e riferimenti"};
char *mode_list[]={
  "Lascia","do-(do)","Do-(do)","DO-(do)","Do-(Do)","DO-(DO)","DO-/DO",
  "Dom(Do)","Dom/do","C-(C)","C-/C",NULL};
int mode_value[]={
  LASCIA,
  BASSO_TRA_PARENTESI|MINORE_MENO,
  MAIUSCOLO|BASSO_TRA_PARENTESI|MINORE_MENO,
  MAIUSCOLO|TUTTO_MAIUSCOLO|MINORE_MENO|BASSO_TRA_PARENTESI,
  MAIUSCOLO|MINORE_MENO|BASSO_TRA_PARENTESI|BASSO_MAIUSCOLO,
  MAIUSCOLO|TUTTO_MAIUSCOLO|MINORE_MENO|BASSO_TRA_PARENTESI|BASSO_MAIUSCOLO|BASSO_TUTTO_MAIUSCOLO,
  MAIUSCOLO|TUTTO_MAIUSCOLO|MINORE_MENO|BASSO_MAIUSCOLO|BASSO_TUTTO_MAIUSCOLO,
  MAIUSCOLO|BASSO_TRA_PARENTESI|BASSO_MAIUSCOLO,
  MAIUSCOLO,
  ENGLISH|MAIUSCOLO|TUTTO_MAIUSCOLO|BASSO_TRA_PARENTESI|MINORE_MENO|BASSO_MAIUSCOLO,
  ENGLISH|MAIUSCOLO|TUTTO_MAIUSCOLO|MINORE_MENO|BASSO_MAIUSCOLO
};

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

int alzo=0,mode_alzo=LASCIA,mode_chords=1;

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

void freeline(struct line *p)	{
  if (p){
    if (p->s!=two_voices || p->ctrl!=two_voices)
      {
	free(p->s);
	free(p->ctrl);
      }
    free(p);
  }
}
void freephrase(struct phrase *p)
{
  struct phrase *q;
  while (p!=NULL)
    {
      if (p->p!=NULL && p->level!=5) freephrase(p->p);
      if (p->l!=NULL) freeline(p->l);
      q=p;
      p=p->next;
      free (q);
    }
}
void freeverse(struct verse *p)
{
  struct verse *q;
  while(p!=NULL)
    {
      freephrase(p->list);
      q=p;
      p=p->next;
      free(q);
    }
}
void freepart(struct part *p)
{
  struct part *q;
  while(p!=NULL)
    {
      if (p->refmode!=3) /*senno' l'ho gia' liberato*/
	freeverse(p->list);
      q=p;
      p=p->next;
      free(q);
    }
}
void freeauthor(struct author *p)
{
  struct author *q;
  while (p!=NULL)
    {
      q=p;
      p=p->next;
      free(q);
    }
}
void freeinfo(struct info *p)
{
  if (p)
    {
      freeinfo(p->next);
      freeline(p->l);
      free(p);
    }
}
void freesong(struct song *p)
{
  freeline(p->head.l);
  freeinfo(p->head.next);
/*  if (p->author!=NULL) freeauthor(p->author);*/
  if (p->main) freepart(p->main);
  free (p);
}
void freelist(struct song_list *p)
{
  struct song_list *q;
  for (;p!=NULL;p=q)
    {
      q=p->next;
      free(p);
    }
}

void freemain(struct song *p)
{
  if (p->main!=NULL) freepart(p->main);
  p->main=NULL;
}
void freeindexx(struct song_list *p)
{
  struct song_list *q;
  for (q=p;q!=NULL;q=q->next)
    freesong(q->first);
  freelist(p);
}

/*READING INPUT FILE*/
FILE *current_fp=0;

void errore(char *);


#define MAXLINE 256
static int nriga;
static signed char buff[MAXLINE+1];
static signed char *buffp; /*primo carattere da leggere*/
static unsigned long position;
/* all'inizio si deve avere nriga=0, buff[0]='\0', buffp=buff;*/

void buff_check(void)
{
if (*buffp=='\0')
  {
    position=ftell(current_fp);
    if ((buffp=(signed char *)fgets(buff,MAXLINE/2,current_fp))==NULL)
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

void myreset(void) {
  buff[0]=0;
  buffp=buff;
  nriga=0;
}

#define MAXLABELS 40
static int label[MAXLABELS];
static struct part* partlabel[MAXLABELS];
static int nlabels=0;

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
  fatal_error(error);
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

char *head_name(char type)
  {
  int i;
  for (i=0;command_msg[i]!=NULL && command_msg[i][0]!=type;i++);
  if (command_msg[i]!=NULL) return command_msg[i]+1;
  else return "Errore!";
  }
struct line *heading_line(struct song *s,char type)
  {
  struct info *p;
  if (type=='t') return s->head.l;
  for (p=s->head.next;p!=NULL && p->type!=type;p=p->next);
  if (p!=NULL) return p->l;
  else return NULL;
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
static int spoken;
int readchar(char *c,char *ctrl)
{
  int x,y;
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
	  *ctrl=' ';
		return 1;
	case '`':
	case '\'':
	case '\"':
	case '^':
	case '~':
	case ',': /*cedilla*/
	case '<': /* \v del TeX */
	  *c=mygetc();
	  *ctrl=y;
	  return 1;
	default:
	  myungetc(y);
	  myungetc('\\');
	  *c='\0';
	  *ctrl='\0';
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
      y=x;
      while (y==' ' || y=='\t') y=mygetc();
      myungetc(y);
      if (y=='&')
	return readchar(c,ctrl);
      else if (y=='\n')
	return 0;
      else
	*c=*ctrl=' ';
      return 1;
    case 0xe8: *c='e';*ctrl='`';return 1;
    case 0xf9: *c='u';*ctrl='`';return 1;
    case 0xec: *c='i';*ctrl='`';return 1;
    case 0xe0: *c='a';*ctrl='`';return 1;
      
    }
  if (x > 127 || x<0) {
    error("carattere ascii non sopportato");
  }
  *c=x;
  *ctrl=' ';
  return 1;
}
struct line *readline(void)
	{
	int n=0;
	int alloc=0;
	struct line *p;
	char *ps,*pctrl;
	char c,ctrl;
	ps=pctrl=NULL;
	for (n=0;readchar(&c,&ctrl);n++)
	  {
	    while (n+1>=alloc)
	      {
		alloc+=8;
		/*			printf("Alloc=%d, ",alloc);*/
		if (ps)
		  ps=(char *)realloc(ps,sizeof(char)*alloc);
		else
		  ps=(char *)malloc(sizeof(char)*alloc);
		if (pctrl)
		  pctrl=(char *)realloc(pctrl,sizeof(char)*alloc);
		else
		  pctrl=(char *)malloc(sizeof(char)*alloc);
		if (ps==NULL || pctrl==NULL)
		  {
		    free(ps);
		    free(pctrl);
		    error("Memoria insufficiente");
		    return NULL;
		  }
	      }
	    ps[n]=c;
	    pctrl[n]=ctrl;
	  }
	if (n==0)
	  {
	    c=mygetc();
	    if (c=='&')
	      {
		ps=two_voices;
				pctrl=two_voices;
				skip_but();
				}
			else
				{
				myungetc(c);
				return NULL;
				}
		}
	else
		ps[n]=pctrl[n]='\0';
	p=(struct line *)malloc(sizeof(struct line));
	if (p==NULL)
		{
		error("Memoria insufficiente");
		if (ps!=two_voices && pctrl!=two_voices)
			{
			free(ps);
			free(pctrl);
			}
		return NULL;
		}
	p->s=ps;
	p->ctrl=pctrl;
	return p;
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
			return 10; /*non dovrebbe mai arrivare qui*/
		}
	}
struct phrase* readsequenza(int level);
struct phrase* readcommand(int level)
	{
	struct phrase *p;
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
		return readsequenza(l);
		}
	if (c=='[')
		{
		p=readsequenza(1);
		skip_but();
		if (mygetc()!=']')
			errore("Ci doveva essere una ']'");
		return p;
		}
	else
		{
		myungetc(c);
		return NULL;
		}
	}

struct phrase* readphrase(int level) /*1:accordo,...,4:part*/
	{
	struct line *l;
	struct phrase *f;
	struct phrase* p;
	if ((l=readline())!=NULL)
		f=NULL;
	else if ((f=readcommand(level))==NULL)
		return NULL;
	p=(struct phrase *)malloc(sizeof (struct phrase));
	if (p==NULL)
		{error("Memoria insufficiente");
		freeline(l);
		freephrase(f);
		return NULL;
		}
	p->level=level;
	p->next=NULL;
	p->l=l;
	p->p=f;
	return p;
	}
struct phrase* readsequenza(int level)
	{
	char c;
	struct phrase *p,*q;
	skip_but();
	c=mygetc();
	if (c=='{')
		{
		skip_spaces();
		for (q=p=readphrase(level);p!=NULL;p=p->next,skip_spaces())
			p->next=readphrase(level);
		skip_spaces();
		if (mygetc()!='}')
			errore("Mi aspettavo una '}'");
		}
	else
		{
		myungetc(c);
		for (q=p=readphrase(level);p!=NULL;p=p->next)
			p->next=readphrase(level);
		}
	return q;
	}
struct verse* readoneverse(void)
	{
	struct phrase *list;
	struct verse *p;
	skip_spaces();
	list=readsequenza(4);
	if (list!=NULL)
		{
		p=(struct verse *) malloc(sizeof(struct verse));
		if (p==NULL)
			{
			error("Memoria insufficiente");
			freephrase(list);
			return NULL;
			}
		p->list=list;
		p->next=NULL;
		skip_spaces();
		}
	else
		p=NULL;
	return p;
	}
struct verse* readverse(void)
	{
	struct verse *p;
	struct verse *q;
	for (p=q=readoneverse();p!=NULL;p=p->next)
		p->next=readoneverse();
	return q;
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
void newlabel(struct part *p,int lab)
{
  int i;
  for (i=0;i<nlabels && label[i]!=lab;i++);
  if (i<nlabels)
    errore("La LABEL e` gia` stata usata");
  else if (nlabels==MAXLABELS)
    errore("Fine dello stack per le LABELS");
  else
    {
      label[nlabels]=lab;
      partlabel[nlabels++]=p;
    }
}
struct part* getlabel(int n)
{
  int i;
  for (i=0;i<nlabels && label[i]!=n;i++);
  if (label[i]==n) return partlabel[i];
/*????*/  printf("nlabels %d, label[0] %d, n %d \n",nlabels,label[0],n);
  errore("LABEL inesistente");
  return NULL;
}
struct part *readonepart(void)
	{
	char c;
	int type;
	int lab=0,ref=0;
	struct part *p;
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
	    type=0;
	    readlabel(&lab,&ref);
	    break;
	  case 's':
	    type=1;
	    readlabel(&lab,&ref);
	    break;
	  case 'k':
	    readlabel(&lab,&ref);
	    type=2;
	    spoken=1;
	    break;
	  case 'b':
	    readlabel(&lab,&ref);
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
	p=(struct part *) malloc(sizeof(struct part));
	if (p==NULL)
	  {
	    error("Memoria insufficiente");
	    return NULL;
	  }
	p->type=type;
	if (lab!=0)
	  newlabel(p,lab);
	if (ref>0)
	  {
	    p->refmode=1;
	    p->ref=getlabel(ref);
	    if (p->ref==NULL)
	      {
		freepart(p);
		return NULL;
	      }
	  }
	else if (ref<0)
	  {
	    p->refmode=2;
	    p->ref=getlabel(-ref);
	    if (p->ref==NULL)
	      {
		freepart(p);
		return NULL;
	      }
	  }
	else
	  {
	    p->refmode=0;
	    p->ref=NULL;
	  }
	p->list=readverse();
	if (p->list==NULL)
	  {
	    errore("Non c'e' il testo...");
	    free(p);
	    return NULL;
	  }
	p->next=NULL;
	return p;
	}
struct part *readpart(void)
	{
	struct part *p,*q;
	for (q=p=readonepart();p!=NULL;p=p->next)
		p->next=readonepart();
	return q;
	}
struct part *readmain(void)
{
  char c;
  struct part *p;
  skip_spaces();
  p=readpart();

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
  if (p)
    {
      song_chords(p);
      p=fill_in_chords(p);
    }
  return p;
}
struct line *readlinea(void)
	{
	char c;
	int i;
	struct line *p;
	skip_spaces();
	c=mygetc();
	if (c!='{') myungetc(c);
	skip_spaces();
	p=readline();
	if (p!=NULL && *(p->s)!='\0')
		{
		for (i=strlen(p->s)-1;i>=0 && isspace(p->s[i]);i--);
		p->s[i+1]='\0';
		p->ctrl[i+1]='\0';
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
void skip_main(void)
	{
	static char tit[]="\\title";
	int i;
	for(;;)
	  {
	  while(buffp[0]!=-1 && !(buffp[0]=='\\' &&
				(buffp[1]=='t' || buffp[1]=='T')))
	    {
	      *buffp='\0';
	      buff_check();
	    }
	  if (buffp[0]==-1) return;
	  if (!isalnum(buffp[2])) return;
	  else
	    {
	    for (i=2;tolower(buffp[i])==tit[i] && tit[i]!='\0';i++);
	    if (tit[i]=='\0') return;
	    }
	  *buffp='\0';
	  buff_check();
	  }
	}
void newhead(struct song *p, char type, struct line *l)
{
  struct info *h;
  for (h=&(p->head);h->next;h=h->next);
  h->next=(struct info *)malloc(sizeof(struct info));
  if (h->next==NULL)
    {
      error("Memoria insufficiente");
      freeline(l);
    }
  else
    {
      h->next->type=type;
      h->next->l=l;
      h->next->next=NULL;
    }
}

struct song *readsong(FILE *fp)	{
  char c;
  char esci;
  long l;
  struct song *p;
  struct line *ln;
  if (current_fp) {
    fprintf(stderr,"Concurrent reading not implemented!\n");
    abort();
  }
  current_fp=fp;
  myreset();
  
  l=position;
  skip_spaces();
  nlabels=0;
  p=(struct song*)malloc(sizeof(struct song));
  if (p==NULL) {
    error("Memoria insufficiente");
    current_fp=0;
    return NULL;
  }
  p->head.l=NULL;
  p->head.type='t';
  p->head.next=NULL;
  p->author=NULL;
  p->flag=SONG_NOOFFSET|SONG_INDEX;
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
  p->head.l=readlinea();
  do
    {
      esci=1;
      skip_spaces();
      l=position+(buffp-buff);
      c=mygetc();
      if (c=='\\')
	{
	  esci=0;
	  c=getcommand();
	  switch(c)
	    {
	    case 'a':
	      if (p->author==NULL)
		p->author=split_authors(readlinea());
	      break;
	    case 'o':
	      ln=readlinea();
	      sscanf(ln->s,"%lu",&(p->offset));
	      p->flag&=~SONG_NOOFFSET;
	      break;
	    case 's':
	    case 'r':
	    case 'k':
	    case 'b':
	      p->flag&=~(SONG_INDEX|SONG_NOOFFSET);
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
	      newhead(p,c==0?'x':c,readlinea());
	      break;
	    }
	}
      else
	{
	  myungetc(c);
	}
    } while (!esci);
  
  p->main=readmain();
  if (p->main==NULL)
    {
      fprintf(stderr,"C'e` stato un errore alla riga %d\n",nriga);
      errore("Non sono riuscito a leggere la canzone");
      freesong(p);
      current_fp=0;
      return NULL;
    }
  current_fp=0;
  return p;
}
void readend(void)
	{
	skip_spaces();
	if (!feof(current_fp))
		errore("Pensavo che il file fosse finito");
	}
char *no_NULL(char *s)
{
  return s!=NULL?s:"";
}
int song_cmp(struct song *p,struct song *q)
{
	return strcmp(no_NULL(p->head.l->s),no_NULL(q->head.l->s));
}
/* gestione indice e selezione */

struct song_list *sort_indexx(struct song_list *indexx)
{
  struct song_list **p;
  struct song_list *to;
  struct song_list *a,*b;
  int ok=0;
  for (to=NULL;!ok;to=*p)
    {
      ok=1;
      /*ordino la lista indexx sapendo che da to in poi sono giusti*/
      for (p=&indexx;(*p)!=NULL && (*p)->next!=to;p=&((*p)->next))
	{
	  if (song_cmp((*p)->first,(*p)->next->first)>0)
	    {
	      ok=0;
	      a=(*p);
	      b=(*p)->next;
	      *p=b;
	      a->next=b->next;
	      b->next=a;
	    }
	}
    }
  return indexx;
}

struct song_list *new_list(struct song *p)
{
  struct song_list *l;
  l=(struct song_list *)malloc(sizeof(struct song_list));
  if (l==NULL) error("Memoria insufficiente");
  l->first=p;
  l->next=NULL;
  return l;
}
struct song_list **end_of_list(struct song_list **x)
{
  for (;*x!=NULL;x=&((*x)->next));
  return x;
}

int indexxlen(struct song_list *p)
{
  int n;
  for (n=0;p!=NULL;p=p->next,n++);
  return n;
}
struct song* indexxsong(int n,struct song_list *p)
{
  for (;n>0 && p!=NULL;n--,p=p->next);
  return p!=NULL?p->first:NULL;
}
struct song_list *indexxsonglist(int n,struct song_list *p)
{
  for (;n>0 && p!=NULL;n--,p=p->next);
  return p;  
}

struct song_list* selected(struct song *q,struct song_list *selection)
{
  struct song_list **p;
  for (p=&selection;(*p)!=NULL;p=&((*p)->next))
    if ((*p)->first==q) return *p;
  return NULL;
}
struct song_list *add_to_selection(struct song *p,struct song_list *selection)
{
  if (p)
    *end_of_list(&selection)=new_list(p);
  return selection;
}
struct song_list *copy_list(struct song_list *indexx)
{
struct song_list *l;
struct song_list *p;
l=NULL;
for (p=indexx;p!=NULL;p=p->next)
  l=add_to_selection(p->first,l);
return l;
}
struct song_list *toggle_selection(struct song *p,struct song_list *selection)
{
  struct song_list **l;
  struct song_list *list;
  for (l=&selection;(*l)!=NULL && (*l)->first!=p;l=&((*l)->next));
  if ((*l)!=NULL)
    {
      list=*l;
      *l=list->next;
      list->next=NULL;
      freelist(list);
      return selection;
    }
  else
    return add_to_selection(p,selection);
}


int mystrcmp(char *s,char *key)
	{
	int i,j;
	for (i=0;s[i]!='\0';i++)
		{
		for (j=0;tolower(s[i+j])==key[j] && key[j]!='\0';j++);
		if (key[j]=='\0') return 1;
		}
	return 0;
	}
int songcmp(struct song *p, char *s)
	{
	  struct author *a;
	  if (mystrcmp(p->head.l->s,s)) return 1;
	  for (a=p->author;a!=NULL;a=a->next)
	    if (mystrcmp(a->name->s,s)) return 1;
	  return 0;
	}
struct song_list* findsong(char *key,struct song_list *p)
{
  struct song_list *l,**x;
  char *s;
  int i,j;
  for (i=1,s=strchr(key,' ');s!=NULL;i++)
    {
      *s='\0';
      s=strchr(s+1,' ');
    }
  l=NULL;
  for (x=&l;p!=NULL;p=p->next)
    {
      for (j=0,s=key;j<i && songcmp(p->first,s);s=strchr(s,'\0')+1,j++);
      if (i==j) 
	{
	  *x=new_list(p->first);
	  x=&((*x)->next);
	}
    }
  for (j=1;j<i;j++)
    *strchr(key,'\0')=' ';
  return l;
}

/*la chiave e' della forma: titolo|autore1|autore2...*/
struct song_list* getsong(char *key,struct song_list *p)
{
  char *s;
  struct song_list *selection;
  struct author *a;
  int n=0,i;
  for (s=key;*s!='\0';s++)
    if (*s=='|') {*s='\0';n++;}
  selection=NULL;
  for (;p!=NULL;p=p->next)
    {
      s=key;
    if (*s=='\0' || !strcmp(p->first->head.l->s,s))
       {
	 for (i=0,s=strchr(s,'\0')+1;i<n;i++,s=strchr(s,'\0')+1)
	   {
	     for (a=p->first->author;a!=NULL && !!strcmp(a->name->s,s);a=a->next);
	     if (a==NULL) break;
	   }
	 if (i==n) selection=add_to_selection(p->first,selection);
       }
    }
  for (s=key;n>0;s++)
    if (*s=='\0') {*s='|';n--;}
  return selection;
}

STRING song_key(struct song *p)
{
  struct author *a;
  STRING s=new_string(NULL);
  s=string_cat(s,p->head.l->s);
  for (a=p->author;a!=NULL;a=a->next)
    {
      s=string_cat(s,"|");
      s=string_cat(s,a->name->s);
    }
  
  return s;
}
STRING author_key(struct author *a)
{
  STRING s=new_string(NULL);
  s=string_cat(s,"|");
  s=string_cat(s,a->name->s);
  return s;
}

int phrase_has_chord(struct phrase *p)
{
  for (;p!=NULL;p=p->next)
    {
			if (p->level==1) return 1;
			else if (p->level==3) return 0;
      if (p->l==NULL && p->p!=NULL)
	{
	  if (phrase_has_chord(p->p)) return 1;
	}
    }
  return 0;
}
int has_chord(struct part *p)
{
  struct verse *v;
  for (v=p->list;v!=NULL;v=v->next)
    {
      if (phrase_has_chord(v->list))
	return 1;
    }
  return 0;
}

int phrase_is_note(struct phrase *p)
	{
	for (;p;p=p->next)
		{
		if (p->level>2)
			{
			if (p->p)
				{
				if (!phrase_is_note(p->p)) return 0;
				}
			else return 0;
			}
		else
			if (p->level!=2) return 0;
		}
	return 1;
	}
int phrase_is_strum(struct phrase *p)
	{
	for (;p;p=p->next)
		{
		if (p->level>3)
			{
			if (p->p)
				{
				if (!phrase_is_strum(p->p)) return 0;
				}
			else return 0;
			}
		else
			if (p->level!=3) return 0;
		}
	return 1;
	}


void phrase_chords(struct phrase *p,int alzo, int mode)
	{
	char *s;
	for (;p;p=p->next)
		{
		if (p->p) phrase_chords(p->p,alzo,mode);
		else if (p->level==1 && p->l->s)
			{
			s=converti(p->l->s,alzo,mode);
			free(p->l->s);
			free(p->l->ctrl);
			if ((p->l->ctrl=strdup(s))==NULL)
				error("Memoria insufficiente");
			p->l->s=s;
			for (s=p->l->ctrl;*s;*(s++)=' ');
			}
		}
	}
void song_chords(struct part* p)
{
  struct part* a;
  struct verse *b;
  if (alzo==0 && mode_alzo&LASCIA) return;
  for (a=p;a;a=a->next)
    for (b=a->list;b;b=b->next)
      phrase_chords(b->list,alzo,mode_alzo);
}


struct phrase *kill_chords_phrase(struct phrase *f)
	{
	struct phrase *k;
	struct phrase **p;
	for (p=&f;*p!=NULL;)
		{
		if ((*p)->level==1 || (*p)->level==3)
			{
			k=*p;
			*p=k->next;
			k->next=NULL;
			freephrase(k);
			}
		else
			{
			if ((*p)->p!=NULL)
				{
				k=*p;
				k->p=kill_chords_phrase(k->p);
				if ((k->p)==NULL)
					{
					*p=k->next;
					k->next=NULL;
					freephrase(k);
					}
				else
					p=&((*p)->next);
				}
			else
				p=&((*p)->next);
			}
		}
	return f;
	}
struct verse* kill_chords_verse(struct verse *v)
	{
	struct verse **p;
	struct verse *k;
	for (p=&v;*p!=NULL;)
		{
		(*p)->list=kill_chords_phrase((*p)->list);
		if ((*p)->list==NULL)
			{
			k=*p;
			*p=k->next;
			k->next=NULL;
			freeverse(k);
			}
		else
			p=&((*p)->next);
		}
	return v;
	}
struct part* kill_chords_part(struct part *v)
	{
	struct part **p;
	struct part *k;
	for (p=&v;*p!=NULL;)
		{
		(*p)->list=kill_chords_verse((*p)->list);
		if ((*p)->list==NULL)
			{
			k=*p;
			*p=k->next;
			k->next=NULL;
			freepart(k);
			}
		else
			p=&((*p)->next);
		}
	return v;
	}
struct part *kill_chords_song(struct part *p)
	{
	return kill_chords_part(p);
	}

/*divide to->l inserendone un pezzo in to->next*/
void split(struct phrase *to1,char *t)
	{
	struct phrase *new_p;
	struct line *new_l;
	char *s,*ctrl;

	if (to1->l->s==two_voices)
		errore("internal error #6193");

	new_p=(struct phrase *)malloc(sizeof(struct phrase));
	new_l=(struct line *)malloc(sizeof(struct line));
	s=strdup(t);
	ctrl=strdup(to1->l->ctrl+(t-to1->l->s));
	*t='\0';
	*(to1->l->ctrl+(t-to1->l->s))='\0';
	to1->l->s=(char*)realloc(to1->l->s,sizeof(char)*(strlen(to1->l->s)+1));
	to1->l->ctrl=(char*)realloc(to1->l->ctrl,sizeof(char)*(strlen(to1->l->s)+1));
	if (new_p==NULL || new_l==NULL || s==NULL || to1->l->s==NULL)
		error("Memoria insufficiente");
	else
		{
		new_l->s=s;
    new_l->ctrl=ctrl;
		new_p->level=to1->level;
		new_p->l=new_l;
		new_p->p=NULL;
		new_p->next=to1->next;
		to1->next=new_p;
		}
	}
static struct phrase **to;
static char *t;
/*
	se *to==NULL, sono finite le parole nel verso
	se (*to)->l != NULL
		se t==NULL siamo all'inizio di (*to)->l->s,
		altrimenti t indica la posizione in (*to)->l->s
*/
void skip_n(int n)
	{
	int l;
	if (n==0) return;
	if (*to!=NULL)
		{
		if ((*to)->l!=NULL)
			{
			if (t==NULL) t=(*to)->l->s;
			if ((l=strlen(t))>n)
				t+=n;
			else
				{
				to=&((*to)->next);
				t=NULL;
				skip_n(n-l);
				}
			}
		else
			{
			to=&((*to)->next);
			t=NULL;
			skip_n(n);
			}
		}
	}

void iter_fill_in (struct phrase *from)
	{
	struct phrase *new;
	for(;from!=NULL;from=from->next)
		{
		if (from->level==1) /*devo copiare l'accordo*/
			{
			new=(struct phrase *)malloc(sizeof(struct phrase));
			if (new==NULL)
				{
				error("Memoria insufficiente");
				return;
				}
			new->level=5;
			new->p=from;
			new->l=NULL;
			new->next=NULL;
			if (*to!=NULL)
				{
				if ((*to)->l!=NULL && t!=NULL && t!=(*to)->l->s)
					{
					split(*to,t);
					to=&((*to)->next);
					t=NULL;
					}
				new->next=*to;
				}
			*to=new;
			to=&(new->next);
			}
		else if (from->p!=NULL)
			iter_fill_in(from->p);
		else
			skip_n(strlen(from->l->s));
		}
	}
struct phrase *phrase_fill_in_chords(struct phrase *from,struct phrase *to1)
	{
	to=&to1;
	t=NULL;
	iter_fill_in(from);
	return to1;
	}

struct part* fill_in_chords(struct part *s)
{
  struct part *p,*q;
  struct verse *f,**t,*b;
  if ((mode_chords&1)==0)
    s=kill_chords_song(s);
  else if (mode_chords&2)
    {
      for (p=s;p!=NULL;p=p->next)
	{
	  if (p->refmode==2)
	    {
	      q=p->ref;
	      for (t=&(p->list),f=q->list;f!=NULL;)
		{
		  if (phrase_is_note(f->list))
		    {
		      f=f->next;
		    }
		  else if (phrase_is_strum(f->list))
		    {
		      b=*t;
		      *t=(struct verse*)malloc(sizeof(struct verse));
		      (*t)->list=(struct phrase*)malloc(sizeof(struct phrase));
		      if (*t==NULL || (*t)->list==NULL)
			error("Memoria insufficiente");
		      (*t)->next=b;
		      (*t)->list->level=5;
		      (*t)->list->p=f->list;
		      (*t)->list->l=NULL;
		      (*t)->list->next=NULL;
		      t=&((*t)->next);
		      f=f->next;
		    }
		  else if (*t==NULL)
		    {
		      break; /*per ora... bisognerebbe aggiungere tutti gli accordi rimasti in f*/
		    }
		  else if (phrase_has_chord((*t)->list))
		    {
		      f=f->next;
		      t=&((*t)->next);
		    }
		  else if (phrase_is_note((*t)->list))
		    {
		      t=&((*t)->next);
		    }
		  else
		    {
		      (*t)->list=phrase_fill_in_chords(f->list,(*t)->list);
		      t=&((*t)->next);
		      f=f->next;
		    }
		}
	      p->refmode=0;
	      p->ref=NULL;
	    }
	}
    }
  if (mode_chords&4)
    {
      for (p=s;p!=NULL;p=p->next)
	{
	  if (p->refmode==1)
	    {
	      freeverse(p->list);
	      p->list=p->ref->list;
	      p->refmode=3;
	      p->ref=NULL;
	    }
	}
    }
  return s;
}

/* gestione lista autori */
struct author *biblio;
void freebiblio(void)
	{
	struct author *p;
	while(biblio!=NULL)
		{
		p=biblio->next;
		if (biblio->name);
			freeline(biblio->name);
		biblio=p;
		}
	}
int bibliolen(void)
	{
	int i;
	struct author *p;
	for (i=0,p=biblio;p!=NULL;p=p->next,i++);
	return i;
	}
int has_written(struct author *a,struct song *s)
	{
	struct author *p;
	for (p=s->author;p!=NULL && p->name!=a->name;p=p->next);
	return p!=NULL;
	}
struct author* biblioauthor(int n)
	{
	int i;
	struct author *p;
	for (i=0,p=biblio;i<n && p!=NULL;i++,p=p->next);
	return p;
	}
struct song_list* select_author(struct author* a,struct song_list *p)
{
  struct song_list *selection,**x;
  struct song *s;
  selection=NULL;
  if (a!=NULL)
    {
      x=&selection;
      for (;p!=NULL;p=p->next)
	{
	  if (has_written(a,p->first))
	    {
	      *x=new_list(p->first);
	      x=&((*x)->next);
	    }
	}
    }
  return selection;
}
char *surname(char *s)
	{
	char *p;
	for (p=s;!( (isupper(*p)&& (isupper(*(p+1))||*(p+1)=='\'' )) || *p=='\0');p++);
	if (*p=='\0') return s;
	else return p;
	}
int author_cmp(struct line *a, struct line *b)
	{
	int r;
	r=strcmp(surname(a->s),surname(b->s));
	if (r==0)
		r=strcmp(a->s,b->s);
	return r;
	}
struct author* new_author(struct line *l)
	{
	struct author **p;
	struct author *a;
	struct author *b;
	int r;
	for (p=&biblio;*p!=NULL && ((r=author_cmp(l,(*p)->name))>0);p=&((*p)->next));
	if (*p!=NULL && r==0)
			freeline(l);
	else
		{
		a=(struct author *)malloc(sizeof(struct author));
		if (a==NULL)
			{
			error("Memoria insufficiente");
			return NULL;
			}
		else
			{
			a->next=*p;
			*p=a;
			a->name=l;
			}
		}
	b=(struct author*)malloc(sizeof(struct author));
	if (b==NULL)
		{
		error("Memoria insufficiente");
		return NULL;
		}
	b->name=(*p)->name;
	b->next=NULL;
	return b;
	}
struct author* split_authors(struct line *l)
	{
	struct author *a;
	char *p;
	struct line *n;
	if (l==NULL) return NULL;
	p=strchr(l->s,',');
	if (p==NULL)
		return new_author(l);
	else
		{
		*p='\0';
		*(l->ctrl+(p-l->s))='\0';
		n=(struct line*)malloc(sizeof(struct line));
		if (n==NULL)
			error("Memoria insufficiente");
		else
			{
			for (p++;p!='\0' && isspace(*p);p++);
			n->s=strdup(p);
			n->ctrl=strdup(l->ctrl+(p-l->s));
			l->s=(char*)realloc(l->s,sizeof(char)*(strlen(l->s)+1));
			l->ctrl=(char*)realloc(l->ctrl,sizeof(char)*(strlen(l->ctrl)+1));
			if (n->s==NULL || n->ctrl==NULL || l->s==NULL || l->ctrl==NULL)
				error("Memoria insufficiente");
			else
				{
				a=new_author(l);
				a->next=split_authors(n);
				return a;
				}
			}
		}
	return NULL;
	}

/* OUTPUT: Debug */
void writechar(char c,char ctrl,FILE *fp)
	{
	if (ctrl==' ')
		putc(c,fp);
	else
		fprintf(fp,"\\%c%c",ctrl,c);
	}
void writeline(struct line *p,FILE *fp)
{
  int status;
  int i;
  status=(p->ctrl==NULL);
  for (i=0;p->s[i]!='\0';i++)
    {
      if (status==0 && p->ctrl[i]=='\0') status=1;
      writechar(p->s[i],status?' ':p->ctrl[i],fp);
    }
}
void writephrase(struct phrase *p,FILE * fp)
	{
	char *s;
	switch(p->level)
		{
		case 1:
			putc('[',fp);
			s="]";
			break;
		case 2:
			fprintf(fp,"\\notes{");
			s="}";
			break;
		case 3:
			fprintf(fp,"\\strum{");
			s="}";
			break;
		case 5:
		case 4:
			s="";
			break;
		}
	for (;p!=NULL;p=p->next)
		{
		if (p->l) writeline(p->l,fp);
		else writephrase (p->p,fp);
		}
	fprintf(fp,"%s",s);
	}
void writeverse(struct verse *p,FILE *fp)
	{
	for (;p!=NULL;p=p->next)
		{
		fprintf(fp,"    ");
		writephrase(p->list,fp);
		fprintf(fp,"\n");
		}
	}
void writepart(struct part *p,FILE *fp)
	{
	struct part*q;
	int lab=0;
	for (q=p;p!=NULL;p=p->next)
		{
		fprintf(fp,"\n  \\");
		switch(p->type)
			{
			case 0: fprintf(fp,"r");break;
			case 1: fprintf(fp,"s");break;
			case 2: fprintf(fp,"k");break;
			case 3: fprintf(fp,"b");break;
			}
		p->refmode=p->refmode|((lab++)<<2);
		fprintf(fp," \\l %d",(int)(p->refmode)>>2);
		switch(p->refmode & 3)
			{
			case 1:
				fprintf(fp,"\\y %d",(int)(p->ref->refmode)>>2);
				break;
			case 2:
				fprintf(fp,"\\f %d",(int)(p->ref->refmode)>>2);
				break;
			case 0:
			case 3:
				break;
			}
		fprintf(fp,"\n");
		writeverse(p->list,fp);
		}
	for (;q!=NULL;q=q->next)
    q->refmode&=3;
	}
void writeauthor(struct author *p,FILE *fp)
	{
	int i;
	for (i=0;p!=NULL;i++,p=p->next)
		{
		if (i) fprintf(fp,", ");
		writeline(p->name,fp);
		}
	}
void writetitles(struct song *p,FILE *fp)
{
  struct info *h;
  fprintf(fp,"\\t ");
  writeline(p->head.l,fp);
  fprintf(fp,"\n");
  if (p->author)
    {
      fprintf(fp,"\\a ");
      writeauthor(p->author,fp);
      fprintf(fp,"\n");
    }
  for (h=p->head.next;h!=NULL;h=h->next)
    {
    fprintf(fp,"\\%c ",h->type);
    writeline(h->l,fp);
    fprintf(fp,"\n");
    }
}
void writeindexxentry(struct song *p,FILE *fp)
  {
  struct info *h;
  fprintf(fp,"\\t{");
  writeline(p->head.l,fp);
  fprintf(fp,"}");
  if (p->author)
    {
      fprintf(fp,"\\a{");
      writeauthor(p->author,fp);
      fprintf(fp,"}");
    }
  for (h=p->head.next;h!=NULL;h=h->next)
    {
    fprintf(fp,"\\%c{",h->type);
    writeline(h->l,fp);
    fprintf(fp,"}");
    }
  if ((p->flag&SONG_NOOFFSET)==0)
    fprintf(fp,"\\o{%lu}",p->offset);
  fprintf(fp,"\n");
  }
void writeindexx(struct song_list *list,FILE *fp)
  {
  for (;list!=NULL;list=list->next)
    writeindexxentry(list->first,fp);
  }
void writesong(struct song *p,FILE *fp)
{
  int rere;
  writetitles(p,fp);
    fprintf(fp,"\n");
  writepart(p->main,fp);
  if (rere)
    freemain(p);
  fprintf(fp,"\n");
}

