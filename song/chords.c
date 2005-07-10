#define UNIX


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef BORLAND_C
#include <alloc.h>
#endif


#include "song.h"
#include "chords.h"

struct accordo
	{
	int nota;
	char *var;
	int basso;
	};

static int mode;
static char *buf;
static char *note[]={"do","do#","re","mib","mi","fa","fa#","sol","sol#","la","sib","si"};
static char *english_note[]={"c","c#","d","eb","e","f","f#","g","g#","a","bb","b"};

extern void error(char *s);

int mycmp(char *s)
	{
	int i;
	for (i=0;s[i] && s[i]==tolower(buf[i]);i++);
	if (s[i]==0) return i;
	else return 0;
	}
void free_accordo(struct accordo *p)
	{
	free(p->var);
	free(p);
	}
int read_nota(void)
	{
	int i,a;
	for (i=0;i<12 && !(a=mycmp(note[i]));i++);
	if (i>=12) 
	  {
	    for (i=0;i<12 && !(a=mycmp(english_note[i]));i++);
	    if (i>=12)
	      return -1;
	  }
	buf+=a;
	while (*buf=='#')
		{buf++;i++;}
	while (tolower(*buf)=='b')
		{buf++;i--;}
	while (i<0) i+=12;
	return i;
	}
struct accordo *read_accordo(void)
	{
	int n=-1,b=-1;
	char *v=NULL;
	char c;
	struct accordo *p=NULL;
	n=read_nota();
	if (n>=0)
		{
		v=buf;
		for (;!((*buf==0)||(*buf=='|')||isspace(*buf)||(*buf=='(')||(buf[0]=='/' && isalpha(buf[1])));buf++);
		if (buf!=v)
			{
			c=*buf;*buf=0;
			v=strdup(v);
			if (v==NULL) error("memoria insufficiente");
			*buf=c;
			}
		else
			v=NULL;
		}
	if (*buf=='(' || *buf=='/')
		{
		c=*(buf++);
		b=read_nota();
		if (b<0)
			error("mi aspettavo una nota di basso");
		if (c=='(')
			{
			if (*buf==')')
				buf++;
			else
				error("mi aspettavo una ')'");
			}
		}
	if (n>=0 || b>=0)
		{
		p=(struct accordo *)malloc(sizeof(struct accordo));
		if (p==NULL) error("memoria insufficiente");
		else
			{
			p->nota=n;
			p->basso=b;
			p->var=v;
			}
		}
	return p;
	}

char *write_accordo(struct accordo *p)
{
  char *s;
  int l=0;
  char *a;
  if (p->nota>=0)
    l+=(mode&ENGLISH!=0)?strlen(english_note[p->nota]):strlen(note[p->nota]);
  if (p->var)
    l+=strlen(p->var);
  if (p->basso>=0)
    {
      l+=(mode&ENGLISH)!=0?strlen(english_note[p->nota]):strlen(note[p->basso]);
      if (mode&BASSO_TRA_PARENTESI)
	l+=2;
      else
	l+=1;
    }
  s=(char *)malloc(sizeof(char)*(l+1));
  *s=0;
  if (p->nota>=0)
    {
      a=strchr(s,'\0');
      
      if (mode&ENGLISH)
	strcat(s,english_note[p->nota]);
      else
	strcat(s,note[p->nota]);
      if (mode&MAIUSCOLO)
	strupr(a);
      if (!(mode&TUTTO_MAIUSCOLO))
	strlwr(a+1);
      if ((mode&ENGLISH)==0)
	if (a[2]=='B') a[2]='b';
      if (p->var)
	{
	  if (p->var[0]=='-' || p->var[0]=='m')
	    p->var[0]=(mode&MINORE_MENO)?'-':'m';
	    strcat(s,p->var);
	}
    }
  if (p->basso>=0)
    {
      if (mode&BASSO_TRA_PARENTESI)
	strcat(s,"(");
      else
	strcat(s,"/");
      a=strchr(s,'\0');
      if (mode&ENGLISH)
	strcat(s,english_note[p->basso]);
      else
	strcat(s,note[p->basso]);
      if (mode&BASSO_MAIUSCOLO)
	strupr(a);
      if (!(mode&BASSO_TUTTO_MAIUSCOLO))
	strlwr(a+1);
      if ((mode&ENGLISH)==0)
	if (a[2]=='B') a[2]='b';
      if (mode&BASSO_TRA_PARENTESI)
	strcat(s,")");
    }
  return s;
}

int alza (int nota,int n)
	{
	if (nota>=0)
		{
		nota+=n;
		while (nota<0) nota+=12;
		while (nota>11) nota-=12;
		return nota;
		}
	else
		return -1;
	}
char *converti(char *string,int alzo,int m)
	{
	struct accordo *p;
	char *s,*t;
	buf=string;
	mode=m;
	s=NULL;
	while (*buf)
		{
		p=read_accordo();
		if (p!=NULL)
			{
			p->nota=alza(p->nota,alzo);
			p->basso=alza(p->basso,alzo);
			t=write_accordo(p);
			free_accordo(p);
			}
		else if (*buf)
			{
			t=(char*)malloc(sizeof(char)*2);
			t[0]=*(buf++);
			t[1]=0;
			}
		else t=NULL;
		if (t)
			{
			if (s)
				{
				s=(char *) realloc(s,sizeof(char)*(strlen(s)+strlen(t)+1));
				strcat(s,t);
				free(t);
				}
			else
				s=t;
			if (s==NULL) error("memoria insufficiente");
			}
		}
	return s;
	}

