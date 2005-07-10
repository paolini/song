#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "song.h"
#include "songout.h"


static FILE *output;
static char *HREF;

static int language;/*1 html, 5 txt, 10: xsg*/

#define MAXWORD 	30

#define MAXLINE 81
#define MAXOUT MAXLINE+30
#define INDENT " > "


static char line[MAXOUT]; /*null terminated*/
static char chords[MAXLINE]; /*piena di spazi*/
static int pos,chordx,linex;
	/*line[pos]='\0', ultimo carattere di line
	linex dice la lunghezza in output di line,
	chordx e' la posizione=lunghezza di chords*/
static int is_chord,is_strum;
static int nlines;

static void (*more)(void); /* funzione da chiamare per il cambio pagina */

static char *html_acc[]={"'acute","`grave","\"uml","^circ","~tilde",",cedil",NULL};
#ifdef DOS
static char *acc[]={
	" aeiouAEIOUnNcC",
	"`\x85\x8A\x8D\x95\x97         ",
	"'\xA0\x82\xA1\xA2\xA3 \x90       ",
	"\" \x89\x8B\x94\x81\x8e  \x99\x9A     ",
	"^\x83\x88\x8C\x93\x96         ",
	"~          \xA4\xA5  ",
	",            \x87\x80",
	""};
#endif
#ifdef UNIX
static char *acc[]={
  " " "a"   "e"   "i"   "o"   "u"   "A"   "E"   "I"   "O"   "U"   "nNcC",
  "`" "\xE0""\xE8""\xEC""\xF2""\xF9""\xC0""\xC8""\xCC""\xD2""\xD9""    ",
  "'" "\xE1""\xE9""\xED""\xF3""\xFA""\xC1""\xC9""\xCD""\xD3""\xDA""    ",
  "^" "\xE2""\xEA""\xEE""\xF4""\xFB""\xC2""\xCA""\xCE""\xD4""\xDB""    ",
  "~" "\xE3"" "   " "   "\xF5"" "   "\xC3"" "   " "   "\xD5"" "   "\xF1\xD1  ",
  "\"""\xE4""\xEB""\xEF""\xF6""\xFC""\xC4""\xCB""\xCF""\xD6""\xDC""    ",
  "," " "   " "   " "   " "   " "   " "   " "   " "   " "   " "   "  \xE7\xC7",
  ""};
#endif

char* word_to_string(char *s,char *ctrl,int *n,int *l)
{
  static char buf[MAXWORD];
  int i,j,x,y;
  for(i=0,j=0,*l=0;s[j]!='\0' && s[j]!=' ' && i+2<MAXWORD;)
    {
      switch(ctrl[j])
	{
	case ' ':
	  buf[i++]=s[j++];
	  (*l)++;
	  break;
	case '&':
	  error("interno #2351");
	  break;
	default: /*un accento*/
	  if (language==1 || language==10)
	    {
	      for (x=0;html_acc[x]!=NULL && html_acc[x][0]!=ctrl[j];x++);
	      if (html_acc[x]==NULL)
		error("interno #9158");
	      sprintf(buf+i,"&%c%s;",s[j],html_acc[x]+1);
	      i+=2+strlen(html_acc[x]);
	      j++;
	      (*l)++;
	    }
	  else
	    {
	      for (x=1;acc[x]!=NULL && acc[x][0]!=ctrl[j];x++);
	      for (y=1;acc[0][y]!='\0' && acc[0][y]!=s[j];y++);
	      if (acc[x]!=NULL && acc[0][y]==s[j] && acc[x][y]!=' ')
		{
		  buf[i++]=acc[x][y];
		  j++;
		  (*l)++;
		}
	      else
		{
		  buf[i++]=s[j];
		  buf[i++]=ctrl[j++];
		  (*l)+=2;
		}
	    }
	  break;
	}
    }
  buf[i]='\0';
  *n=j;
  return buf;
}

void clr_line(void)
	{
	int i;
	for (i=0;i<MAXLINE-1;i++)
		chords[i]=' ';
	chords[i]='\0';
	line[0]='\0';
	pos=0;
	chordx=linex=0;
	}

void out_out(void)
{
  int i;
  if (language==10) return;
  for (i=MAXLINE-1;i>0 && chords[i-1]==' ';i--);
  if (i>0)
    {
      chords[i]='\0';
      fprintf(output,"%s\n",chords);
      nlines++;
    }
  fprintf(output,"%s\n",line);
  nlines++;
  if (more!=NULL && nlines>21)
    {
      (*more)();
      nlines=0;
    }
  clr_line();
}

void out_string(char *s,int n) /*n e' la lunghezza in output*/
	{
	int i;
	if (is_chord && !is_strum)
		{
		if (chordx<linex) chordx=linex;
		if (chordx+n>=MAXLINE-1)
			{
			out_out();
			strcpy(line,INDENT);
			pos=linex=chordx=strlen(line);
			}
		for (i=0;s[i]!='\0';i++)
			{
			if (chordx<MAXLINE-1)
				chords[chordx++]=s[i];
			}
		}
	else
		{
		if (linex+n>=MAXLINE-1)
			{
			out_out();
			strcpy(line,INDENT);
			pos=linex=chordx=strlen(line);
			}
		for (i=0;s[i]!='\0';i++)
			{
			if (pos<MAXOUT-1)
				line[pos++]=s[i];
			}
		line[pos]='\0';
		linex+=n;
		}
	}

void out_voice(void)
{
  out_string(" ",1);
  while (linex<MAXLINE/2)
    out_string(" ",1);
}

void out_xml_author(struct line *p) {
  int i,j,pass;
  for (j=0;
       p->s[j] && ! (j>1 && isspace(p->s[j-1]) 
		     && isupper(p->s[j]) && p->s[j+1] && isupper(p->s[j+1]));
       ++j);
  if (p->s[j]) pass=0; else pass=1;
  for (;pass<2;pass++) {
    int start=0;
    if (pass==0) start=j;
    for (i=start;p->s[i] && (pass==0 || i<j-1);++i) {
      char c=p->s[i];
      if (pass==0 && isalpha(p->s[i-1]) && isalpha(c)) c=tolower(c);
      switch(p->ctrl[i]) {
      case '&':
	fprintf(output,"<tab />");
	break;
      case ' ':
	if (c==13) break;
	fprintf(output,"%c",c);
	break;
      default:
	{
	  int j,k;
	  for (j=0;acc[0][j] && acc[0][j]!=c;++j);
	  for (k=0;acc[k] && acc[k][0]!=p->ctrl[i];++k);
	  if (acc[0][j] && acc[k] && acc[k][j]!=' ')
	    fprintf(output,"%c",acc[k][j]);
	  else fprintf(output,"%c",c);
	}
      }
    }
    if (pass==0) fprintf(output,", ");
  }
}

void out_line(struct line *p)
{
  if (language==10) {
    int i;
    for (i=0;p->s[i];++i) {
      switch(p->ctrl[i]) {
      case '&':
	fprintf(output,"<tab />");
	break;
      case ' ':
	if (p->s[i]==13) break;
	fprintf(output,"%c",p->s[i]);
	break;
      default:
	{
	  int j,k;
	  for (j=0;acc[0][j] && acc[0][j]!=p->s[i];++j);
	  for (k=0;acc[k] && acc[k][0]!=p->ctrl[i];++k);
	  if (acc[0][j] && acc[k] && acc[k][j]!=' ')
	    fprintf(output,"%c",acc[k][j]);
	  else fprintf(output,"%c",p->s[i]);
	}
      }
    }
  } else {
    int i;
    int n,l;
    char *s;
    if (p->ctrl[0]=='&')
      out_voice();
    else
      {
	for (i=0;p->s[i]!='\0';)
	  {
	    s=word_to_string(p->s+i,p->ctrl+i,&n,&l);
	    i+=n;
	    out_string(s,l);
	    if (p->s[i]==' ')
	      {
		out_string(" ",1);
		i++;
	      }
	  }
      }
  }
}

void out_line_cont(struct line *p)
{
  int i;
  int n,l;
  char *s;
  for (i=0;p->s[i]!='\0';)
    {
      s=word_to_string(p->s+i,p->ctrl+i,&n,&l);
      i+=n;
      out_string(s,l);
      if (p->s[i]==' ')
	{
	  out_string(" ",1);
	  i++;
	}
    }
  fprintf(output,"%s",line);
  clr_line();
}

void out_phrase(struct phrase *p, struct phrase *parent)
{
  for (;p!=NULL;p=p->next)
    {
      switch(p->level)
	{
	case 1:
	  is_chord=1;
	  if (language==10) { 
	    /*	    if (parent && parent->next && parent->next->l 
		&& isspace(parent->next->l->s[0]))
	      fprintf(output,"<c prev>");
	      else */ 
	    fprintf(output,"<c>");
	  }
	  break;
	case 2:
	  is_strum++;
	  if (language==10) {
	    if (is_strum==1) fprintf(output,"<note>");
	  }
	  else
	    out_string("[",1);
	  break;
	case 3:
	  is_strum++;
	  if (language==10 && is_strum==1) fprintf(output,"<strum>");
	  break;
	}
      if (p->p)
	out_phrase(p->p,p);
      else
	out_line(p->l);
      switch(p->level)
	{
	case 1:
	  is_chord=0;
	  if (language==10) fprintf(output,"</c>");
	  break;
	case 2:
	  is_strum--;
	  if (language==10) {
	    if (is_strum==0) fprintf(output,"</note>");
	  } else out_string("]",1);
	  break;
	case 3:
	  is_strum--;
	  if (language==10 && is_strum==0)
	    fprintf(output,"</strum>");
	  break;
	}
    }
}

int line_is_empty(struct line *l) {
  int i;
  for (i=0;l->s[i];++i)
    if (!isspace(l->s[i])) return 0;
  return 1;
}

int phrase_is_empty(struct phrase *p) {
  if (p->p) if (!phrase_is_empty(p->p)) return 0;
  if (p->l) if (!line_is_empty(p->l)) return 0;
  if (p->next) if (!phrase_is_empty(p->next)) return 0;
  return 1;
}

void out_verse(struct verse *p)
{
  for (;p!=NULL;p=p->next)
    {
      if (phrase_is_empty(p->list)) continue;
      if (language==10)
	fprintf(output,"<v>");
      out_phrase(p->list,0);
      if (language==10) fprintf(output,"</v>\n");
      out_out();
    }
}

void out_part(struct part *p)
{
  int n=0;
  int m;
  struct part *r;
  for (;p!=NULL;p=p->next,++n)
    {
      if (language==1 && p->type==0)
	fprintf(output,"<em>");
      if (language==10) {
	char *s[]={"refrain","strophe","talking","tabular"};
	fprintf(output,"<p");
	if (p->type>=0 && p->type<=3 && p->type!=1) 
	  fprintf(output," type='%s'",s[p->type]);
	for (r=p->next;r && r->ref!=p;r=r->next);
	if (r && r->ref==p)
	  fprintf(output," label='%c'",(char)('a'+n));
	if (p->refmode>0 && p->refmode<4) {
	  for (r=p->ref, m=0;r && r!=p;r=r->next, m++);
	  if (r==p) {
	    char *s[]={"","copy","chords","copy",0};
	    fprintf(output," %s='%c'",s[p->refmode],(char)('a'+n-m));
	  }
	}
	fprintf(output,">\n");
      }
      out_out();
      out_verse(p->list);
      if (language==1 && p->type==0)
	fprintf(output,"</em>");
      if (language==10) fprintf(output,"</p>\n");
    }
}
void out_titles(struct song *p)
{
  struct author *a;
  if (p->head.l) {
    if (language==1)
      fprintf(output,"<H2>");
    else if (language==10) 
      fprintf(output,"<title>");
    out_line(p->head.l);
    out_out();
    if (language==1) fprintf(output,"</H2>");
    else if (language==10) fprintf(output,"</title>\n");
  }
  if (p->author)
    {
      if (language==1)
	fprintf(output,"<H3>");
      else if (language==10) fprintf(output,"<author>");
      else
	out_string("(",1);
      if (language==10) 
	out_xml_author(p->author->name);
      else
	out_line(p->author->name);
      if (language==10) fprintf(output,"</author>\n");
      for (a=p->author->next;a!=NULL;a=a->next)
	{
	  if (language==10) fprintf(output,"<author>");
	  else out_string(", ",2);
	  if (language==10) out_xml_author(a->name);
	  else out_line(a->name);
	  if (language==10) fprintf(output,"</author>\n");
	}
      if (language!=1 && language!=10)
	out_string(")",1);
      out_out();
      if (language==1)
	fprintf(output,"</H3>");
    }
  if (p->file)
    {
      if (language==1)
	{
	  if (HREF)
	    fprintf(output,
		    "<H4> file: <A HREF=\"%sVOID=%s;search=;\">%s</A></H4>",
		    HREF,p->file->name,p->file->name);
	  else
	    fprintf(output,"<H4> source: %s</H4>\n",p->file->name);
	}
      else if (language==10) {
	// nop
      } else {
	fprintf(output,"source: %s\n",p->file->name);
      }
    }
}

void out_song(struct song *p)
	{
	nlines=0;
	rereadsong(p);
	clr_line();
	if (language==10)
	  fprintf(output,"<song>\n<head>\n");
	out_titles(p);
	if (language==1)
	  fprintf(output,"<PRE>");
	else if (language==10) {
	  fprintf(output,"</head>\n");
	  fprintf(output,"<log>\n<item>converted from sng file");
	  if (p->file) fprintf(output," %s",p->file->name);
	  fprintf(output,"</item>\n");
	  fprintf(output,"</log>\n<body>\n");
	}
	out_part(p->main);
	if (language==1)
	  fprintf(output,"</PRE>");
	else if (language==10)
	  fprintf(output,"</body>\n</song>\n");
	freemain(p);
	}

void out_songs(struct song_list *p,FILE *fp)
  {
  language=5;
  output=fp;
  more=NULL;
  for (;p!=NULL;p=p->next)
    {
      out_song(p->first);
      fprintf(output,"\n\n");
    }
  }

void out_songs_html(struct song_list *p,FILE *fp)
{
  language=1;
  output=fp;
  more=NULL;
  for(;p!=NULL;p=p->next)
    out_song(p->first);
}

void out_songs_xml(struct song_list *p,FILE *fp)
{
  language=10;
  output=fp;
  more=NULL;
  fprintf(output,"<?xml version='1.0' encoding='ISO-8859-1' ?>\n");
  for(;p!=NULL;p=p->next)
    out_song(p->first);
}

void out_songs_src(struct song_list *p, FILE *fp)
{
  for (;p!=NULL;p=p->next)
    {
    writesource(p->first,fp);
    }
}
void out_song_con(struct song *p,void (*f)(void))
{
  language=5;
  output=stdout;
  more=f;
  out_song(p);
  (*more)();
}
STRING htmlconv(STRING s)
  {
  char *p;
  if (s.s!=NULL)
    {
      for (p=s.s;*p!='\0';p++)
	if (*p==' ' || *p=='&' || *p==';') *p='+';
    }
  return s;
  }

int is_format(char *s,char *format)
{
  char *p;
  for (;*format!='\0';format++)
    {
      p=strchr(s,'%');
      if (p==NULL || *(p+1)!=*format) return 0;
      s=p+2;
    }
  return strchr(s,'%')==NULL;
}

void out_index_titles(struct song *p,int n)
{
  struct author *a;
  struct info *h;
  STRING s;
  fprintf(output,"%d) ",n);
  if (language==1 && HREF!=NULL)
    {
      fprintf(output,"<A HREF=\"");
      if (is_format(HREF,"ss"))
	  {
	    s=song_key(p);
	    s=htmlconv(s);
	    fprintf(output,HREF,p->file->name,s.s);
	    delete_string(s);
	  }
      else
	fprintf(output,"%s",HREF);
      fprintf(output,"\">");
      }
  out_line_cont(p->head.l);
  if (p->author)
    {
      fprintf(output," (");
      out_line_cont(p->author->name);
      for (a=p->author->next;a!=NULL;a=a->next)
	{
	  fprintf(output,", ");
	  out_line_cont(a->name);
	}
      fprintf(output,")");
    }
  for (h=p->head.next;h!=NULL;h=h->next)
    {
    fprintf(output,", %s: ",head_name(h->type));
    out_line_cont(h->l);
    }
  if (language==1)
     {
     if (HREF!=NULL)
        fprintf(output,"</A>");
     fprintf(output,"<BR>\n");
     }
  else
    fprintf(output,"\n");
  nlines++;
}

void out_one_author(int i,struct author *p)
{
  char c;
  STRING S;
  char *s;
  
  if (language==1 && HREF!=NULL)
    {
      fprintf(output,"<A HREF=\"");
      if (is_format(HREF,"s"))
	{
	  S=author_key(p);
	  S=htmlconv(S);
	  fprintf(output,HREF,S.s);
	  delete_string(S);
	}
      else
	fprintf(output,"%s",HREF);
      fprintf(output,"\">");
    }
  s=surname(p->name->s);
  fprintf(output,"%d) %s",i,s);
  if (s!=p->name->s)
    {
      c=*(s-1);
      *(s-1)='\0';
      fprintf(output," (%s)",p->name->s);
      *(s-1)=c;
    }
  if (language==1)
    {
      if (HREF!=NULL)
	fprintf(output,"</A>");
      fprintf(output,"<BR>");
    }
  fprintf(output,"\n");
  
}
void my_out_authors(void)
{
  int i;
  struct author *p;
  for (i=1,p=biblio;p!=NULL;i++,p=p->next)
    {
      out_one_author(i,p);
    }
}
void out_authors(FILE *fp)
{
  output=fp;
  language=5;
  more=NULL;
  my_out_authors();
}
void out_authors_html(FILE *fp)
{
  output=fp;
  language=1;
  more=NULL;
  my_out_authors();
}
void out_authors_con(void (*f)(void))
{
  output=stdout;
  language=1;
  more=f;
  my_out_authors();
}
void out_index(FILE *fp,struct song_list *p)
  {
  int i;
  output=fp;
  more=NULL;
  language=5;
  for (i=0;p!=NULL;p=p->next,i++)
    {
      out_index_titles(p->first,i+1);
    }
  }
void out_index_html(FILE *fp,struct song_list *p)
  {
  int i;
  output=fp;
  language=1;
  more=NULL;
  for (i=0;p!=NULL;p=p->next,i++)
      {
      out_index_titles(p->first,i+1);
      }
  }

void set_lang(int new)
{
language=new;
}
void set_HREF(char *new)
  {
    HREF=new;
    language=1;
    output=stdout;
  }
