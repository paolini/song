/* 
 * Questo file contiene funzioni utili per l'interpretazione delle query
 * nei programmi cgi-bin.
 *
 * Ecco cosa rende disponibile:
 * 
 * struct couple
 *   una lista di coppie variabile=valore 
 *
 * int couple_out_of_memory
 *   questa variabile viente posta =1 in caso non ci sia memoria sufficiente.
 *
 * struct couple *read_couple(void)
 *   legge la query con il metodo GET o POST indifferentemente, e torna una
 *   lista di coppie variabile=valore 
 *
 * char *find(struct couple *list,char *var)
 *   torna il valore della variabile var nella lista list.
 * 
 * int list_length(struct couple *list)
 *   torna il numero di coppie contenute nella lista list.
 *
 * void free_couple(struct couple *list)
 *   libera la memoria occupata dalla lista list.
 * 
 * void delete_couple(struct couple *list, struct couple *p)
 *   libera la couple p dalla lista list.
 *
 * void out_html(void), void out_xbm(void), void out_gif(void)
 *   scrivono l'intestazione dei vari formati
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "cgi-util.h"


static int post;
static char *env;
static int length;
static int n;

int couple_out_of_memory=0;
int out_of_memory(void *s)
{
  return couple_out_of_memory=(s==NULL);
}

static char x2c(char *what) 
{
  register char digit;  
  digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
  digit *= 16;
  digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
  return(digit);
}

static char digit(int n)
{
  return n>=10? 'A'+(n-10):'0'+n;
}

static void unescape_url(char *url) 
{
  register int x,y;  
  for(x=0,y=0;url[y];++x,++y) 
    {
      if ((url[x] = url[y]) == '%') 
	{
	  url[x] = x2c(&url[y+1]);
	  y+=2;
	}
    }
  url[x] = '\0';
}
static int escapanda(char c)
{
  return ((unsigned char)c>127 || c=='+' || c=='&' || c==';' || c=='%');
}
static STRING escape_url(STRING s)
{
  char *p;
  char c;
  if (s.s!=NULL)
    {
      for (p=s.s;*p!='\0';p++)
	{
	  if (escapanda(c=*p))
	    {
	      *(p++)='%';
	      s=string_insert(s,p++,digit(c/16));
	      s=string_insert(s,p,digit(c%16));
	    }
	  else if (*p==' ')
	    *p='+';
	}
    }
  return s;
}

static void plustospace(char *str) 
{
  register int x;
  for(x=0;str[x];x++) if(str[x] == '+') str[x] = ' ';
}

static int mygetc(void)
{
  if (n>length-1) return '\0';
  if (post)
    {
      n++;
      return fgetc(stdin);
    }
  else
    return env[n++];
} 

static int finito()
{
  return n>length-1;
}

static char *read_string(char *stop)
{
  STRING s=new_string(NULL);
  int c;
  do
    {
      s=string_add_char(s,c=mygetc());
      if (post && n>length) c='\0';
    }
  while(strchr(stop,c)==NULL && c!=EOF && c!='\0');
  if (c!='\0' && c!=EOF)
    {
      *(strchr(s.s,'\0')-1)='\0';
    }
  plustospace(s.s);
  unescape_url(s.s);
  s=string_realloc(s);
  return s.s;
}

void free_couple(struct couple *p)
{
  if (p->rest) free_couple(p->rest);
  free(p->name);
  free(p->value);
  free(p);
}

struct couple *delete_couple(struct couple *list, struct couple *p)
{
  struct couple **q;
  for (q=&list;(*q) && (*q)!=p;q=&((*q)->rest));
  if (*q)
    {
      *q=p->rest;
      p->rest=NULL;
      free_couple(p);
    }
  return list;
}

struct couple *read_rest(void)
{
  struct couple *p;
  if (finito()) return NULL;
  p=(struct couple*)malloc(sizeof(struct couple));
  if (out_of_memory(p)) return NULL;
  p->name=read_string("=");
  p->value=read_string("&;");
  p->rest=read_rest();
  if (couple_out_of_memory)
    {
      free_couple(p);
      return NULL;
    }
  return p;
}

struct couple *read_stdin(int len)
{
  post=1;
  length=len;
  return read_rest();
}
static char *nil(char *s)
{
  return s?s:"(nil)";
}
struct couple *read_couple()
{
  post=1;
  if (!strcmp(nil(getenv("REQUEST_METHOD")),"GET"))
    {
      env=nil(getenv("QUERY_STRING"));
      length=strlen(env);
      post=0;
    }
  else if (!strcmp(nil(getenv("REQUEST_METHOD")),"POST"))
    {
      length=atoi(nil(getenv("CONTENT_LENGTH")));
      post=1;
    }
  else
    return NULL;
  return read_rest();
}

struct couple *find_couple(struct couple *p,char *name)
{
  for (;p!=NULL && !!strcmp(name,p->name);p=p->rest);
  return p;
}
char *find(struct couple *p,char *name)
{
  struct couple *l;
  l=find_couple(p,name);
  return l!=NULL?l->value:NULL;
}
int list_length(struct couple *p)
{
  int i;
  for (i=0;p!=NULL;p=p->rest,i++);
  return i;
}

STRING url(struct couple *list)
{
  STRING s=new_string(NULL);
  STRING p=new_string(NULL);
  for (;list!=NULL;list=list->rest)
    {
      p=string_cpy(p,list->name);
      p=escape_url(p);
      s=string_cat(s,p.s);
      s=string_cat(s,"=");
      p=string_cpy(p,list->value);
      p=escape_url(p);
      s=string_cat(s,p.s);
      s=string_cat(s,";");
    }
  delete_string(p);
  return s;
}


static FILE *out=stdout;

void set_html_output(FILE *output)
{
out=output;
}
FILE *get_html_output(void)
{
return out;
}

void out_ps(void)
{
  fprintf(out,"Content-type: application/postscript%c%c",10,10);
}
void out_txt(void)
{
  fprintf(out,"Content-type: text/plain%c%c",10,10);
}
void out_html(void)
{
  fprintf(out,"Content-type: text/html%c%c",10,10);
}
void out_xbm(void)
{
  fprintf(out,"Content-Type: image/x-portable-bitmap%c%c",10,10);
}
void out_gif(void)
{
  fprintf(out,"Content-Type: image/gif%c%c",10,10);
}

void open_html(char *title,char *base_url)
{
  out_html();
  fprintf(out,"<HTML><HEAD>\n"
	 "<TITLE> %s </TITLE>\n"
	 "<LINK REV=\"made\" HREF=\"mailto:paolini@italia.hum.utah.edu\">\n",title);
  if (base_url)
    fprintf(out,"<BASE href=\"%s\">\n",base_url);
  fprintf(out,"</HEAD><BODY>\n"
	 "<H1>%s</H1>\n",title);
}

/*Fine del documento:*/
void close_html(void)
{
  fprintf(out,"</BODY></HTML>\n");
}




