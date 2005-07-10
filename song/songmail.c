#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "cgi-util.h"
#include "song.h"
#include "chords.h"
#include "string.h"
#include "songprin.h"
#include "songout.h"
#include "mail.h"

#ifndef MY_ADDRESS
#define MY_ADDRESS "mailto:paolini@linuz.sns.it"
#endif

#define BASE_URL NULL

#ifndef TIME_OUT
#define TIME_OUT 0 /*180*/
#endif 
/*Interrompe dopo TIME_OUT secondi, se 0 non interrompe*/

struct song_list *index=NULL,*selection=NULL;
char *languages[]={"IT","UK","FU",NULL};
char *error_msg[]={"Errore!","Error!","Eror!",NULL};
int language=0;
static FILE *output=NULL;

void error(char *s)
{
  if (output)
    fprintf(output,"%%ERRORE: %s\n",s);
}
void fatal_error(char *s)
{
  error(s);
  exit(1);
}

struct sigaction Action;
void handler(int a)
{
  exit(1);
}
#define CLOSING_FILE NULL

#ifndef CLOSING_FILE
#define CLOSING_FILE "close.html"
#endif
#define SONG_DIRECTORY "/home/paolini/gnos/"
#define ALL_FILE  SONG_DIRECTORY"allfile.txt"

int choose(char **list,char *what)
{
int i;
for (i=0;list[i]!=NULL;i++)
  if (!strcmp(list[i],what)) return i;
return -1;
}
void all_file(void)
{
  FILE *fp;
  char buf[40];
  char *s;
  fp=fopen(ALL_FILE,"rt");
  if (fp==NULL) error("Non sono riuscito ad aprire tutti i files");
  else
    {
      while (fgets(buf,39,fp))
	{
	  s=strchr(buf,'\n');
	  if (s!=NULL) *s='\0';
	  if (buf[0]!='\0')
	    {
	      if (setfile(searchfile(buf)))
		{
		  *end_of_list(&index)=readfile(NULL);
		}
	    }
	}
    }
}

struct couple *delete_list(struct couple *list,struct couple *l)
{
  struct couple **p;
  for (p=&list;(*p)!=NULL && (*p)!=l;p=&((*p)->rest));
  if (*p==NULL) return NULL;
  *p=l->rest;
  free(l);
  return list;
}
struct couple *cgi_copy_list(struct couple *list)
{
  struct couple *l;
  if (list==NULL) return NULL;
  l=(struct couple*)malloc(sizeof(struct couple));
  if (l!=NULL)
    {
      l->name=list->name;
      l->value=list->value;
      l->rest=cgi_copy_list(list->rest);
    }
  return l;
}
void free_list(struct couple *list)
{
  if (list->rest) free_list(list->rest);
  free(list);
}

void out_change_lang(struct couple *list)
{
  int i;
  STRING s;
  struct couple *l;
  list=cgi_copy_list(list);
  s=new_string(NULL);
  s=string_cat(s,MY_ADDRESS);
  s=string_cat(s,"?");
  s=string_merge(s,url(list));
  free_list(list);
  fprintf(output," <A HREF=\"%soptions=on;\">opzioni</A>\n",s.s);
  fprintf(output,"<P>\n");
}
void out_closing(void)
{
  FILE *fp;
  char line[81];
  if (CLOSING_FILE)
    {
    fp=fopen(CLOSING_FILE,"rt");
    if (fp)
      {
	while (!feof(fp))
	  {
	    fputs(fgets(line,80,fp),output);
	  }
      }
    }
}
struct couple *delete_void_files(struct couple *list)
{
  struct couple *ll=NULL;
  struct couple *l;
  struct song_list *p;

  while (l=find_couple(list,"VOID"))
    list=delete_couple(list,l);
  for (p=selection;p!=NULL;p=p->next)
    {
      for (l=ll;l!=NULL;l=l->rest)
	{
	if (!strcmp(l->value,p->first->file->name))
	  break;
	}
      if (l==NULL)
	{
	  l=(struct couple*)malloc(sizeof(struct couple));
	  if (l==NULL) error("memoria insufficiente");
	  l->name=strdup("VOID");
	  l->value=strdup(p->first->file->name);
	  if (l->name==NULL || l->value==NULL) error("memoria insufficiente");
	  l->rest=ll;
	  ll=l;
	}
    }
  if (ll!=NULL)
    {
      for (l=ll;l->rest!=NULL;l=l->rest);
      l->rest=list;
      list=ll;
    }
  return list;
}
struct couple *strip_default(struct couple *list)
{
  struct couple *l,*ll;
  do
    {
      for (l=list;l!=NULL && !!strcmp(l->value,"[default]");l=l->rest);
      if (l!=NULL)
	list=delete_couple(list,l);
    }while (l!=NULL);
  return list;
}
void altre_opzioni(struct couple *list)
{
  STRING s;
  int i;
  int a;
  struct couple *l;
  s=new_string(NULL);
  s=string_cat(s,MY_ADDRESS);
  s=string_cat(s,"?");
  s=string_merge(s,url(list));
  fprintf(output,"<form>");
  fprintf(output,"<strong>Output: </strong>");
  for (i=2;i<=5;i++)
    {
      if (i==3 ||i==4|| i==5)
	fprintf(output," <A HREF=\"%slanguage=%s;\">%s</A>\n",s.s,lang_list[i],lang_desc[i]);
    }
  fprintf(output,"<BR>\n");
  fprintf(output,"<strong>Modalit&agrave; accordi: </strong>");
  for (i=0;chord_list[i]!=NULL;i++)
    {
      fprintf(output," <A HREF=\"%schords=%s;\">%s</A>\n",s.s,chord_list[i],chord_desc[i]);
    }
  fprintf(output,"<BR>\n");
  fprintf(output,"<strong>Trasposizione accordi: </strong>");
  for (l=list;l!=NULL && !!strcmp(l->name,"transpose");l=l->rest);
  if (l==NULL) a=0;
  else a=atoi(l->value);
  for (i=-5;i<7;i++)
    {
      fprintf(output," <a href=\"%stranspose=%d;\">%c%d</a>",s.s,a+i,i<0?' ':'+',i);
    }
  fprintf(output,"<BR>\n");
  fprintf(output,"<strong>Visualizzazione accordi: </strong>");
  for (i=1;mode_list[i]!=NULL;i++)
    {
      fprintf(output," <a href=\"%smode=%s;\">%s</a>",s.s,mode_list[i],mode_list[i]);
    }
  fprintf(output,"<br>\n");
  fprintf(output,"<strong>Massimo di canzoni visualizzabili complete:</strong>");
  for (i=-1;i!=1000;)
    {
      fprintf(output," <a href=\"%smax=%d;\">",s.s,i);
      if (i==-1)
	fprintf(output,"tutte");
      else if (i==0)
	fprintf(output,"nessuna");
      else
	fprintf(output,"%d",i);
      fprintf(output,"</a>");
      switch(i)
	{
	case -1:i=0;break;
	case 0:i=1;break;
	case 1:i=5;break;
	default:i=1000;break;
	}
    }
  fprintf(output,"<p>");
}
static struct mail Mail;

struct couple *mail2url(void)
{
  char *p;
  char *s;
  char *r;
  static char line[256];
  struct couple *list=NULL;
  do
    {
      if (!fgets(line,255,stdin)) return NULL;
      for (p=strchr(line,'\0')-1;*p=='\n' || *p==10|| *p==13 || isspace(*p);p--);
      *(p+1)='\0';
    }while (line[0]=='\0');
  for (p=line;isspace(*p);p++);
  for (s=p;!(*s=='\0' || *s=='=' || *s==':');s++);
  if (*s=='\0') 
    {
      line[32]='\0';
      fprintf(output,"%% Non ho capito la riga '%s...'\n",line);
      return mail2url();
    }
  for (r=s-1;isspace(*r) && r>p;r--);
  *(r+1)='\0';
  list=(struct couple *)malloc(sizeof(struct couple));
  if (list==NULL)
    {
      errore("memoria insufficiente");
      return NULL;
    };
  list->name=strdup(p);
  list->value=strdup(s+1);
  list->rest=mail2url();
  return list;
}
struct couple *urlmail2url(void)
{
int length;
sscanf(Mail.content_length,"%d",&length);
return read_stdin(length);
}
void myexit(void)
{
  if (output)
    pclose(output);
}
void out_mime(void)
{
  fprintf(output,"MIME-Version: 1.0\n");
}
void out_nomime(void)
{
  fprintf(output,"\n");
}
int main()
{
  struct author *p,*q;
  struct couple *list,*l;
  struct song_list *g;
  char *c;
  STRING s;
  int i;
  int max=5,lang=5,authors=0;
  int nfound;
  if (TIME_OUT)
    {
      Action.sa_handler=handler;
      sigaction(SIGALRM,&Action,NULL);
      alarm(TIME_OUT);
    }

  read_intro(&Mail,stdin);
  if (strstr(Mail.subject,"#song"))
    {
      output=answer(&Mail,"RE: #song");
      fprintf(output,"From: Song <paolini@linuz.sns.it>\n");
      list=mail2url();
      atexit(myexit);
    }
  else if(!strcmp(Mail.content_type,"application/x-www-form-urlencoded"))
    {
      output=answer(&Mail,"RE: #song");
      fprintf(output,"From: Song <paolini@linuz.sns.it>\n");
      list=urlmail2url();
      atexit(myexit);
      lang=5;
    }
  else
    return 0; /*non era un mail per me*/
  list=strip_default(list);
  if (list==NULL)
    error("Non sono riuscito a leggere la query");
  language=0;
  set_mode_chords(1);
  set_mode_alzo(LASCIA);
  chdir(SONG_DIRECTORY);
  if (list->rest==NULL&&*(list->value)=='\0')
    {
      all_file();
      selection=findsong(list->name,index);
      list=delete_couple(list,list);
    }
  else
    {
      for (l=list;l!=NULL;l=l->rest)
	{
	  if (!strcmp(l->name,"VOID"))
	    {
	      if (!strcmp(l->value,"*"))
		{
		  all_file();
		}
	      else
		{
		  if (setfile(searchfile(l->value)))
		    *end_of_list(&index)=readfile(NULL);
		}
	    }
	  else if (!strcmp(l->name,"max"))
	    max=atoi(l->value);
	  else if (!strcmp(l->name,"language"))
	    {
	      strlwr(l->value);
	      lang=choose(lang_list,l->value);
	      if (lang==-1) lang=5;
	    }
	  else if (!strcmp(l->name,"chords"))
	    {
	      strlwr(l->value);
	      i=choose(chord_list,l->value);
	      if (i==-1) i=1;
	      set_mode_chords(chord_value[i]);
	    }
	  else if (!strcmp(l->name,"transpose"))
	    {
	      set_alzo(atoi(l->value));
	    }
	  else if (!strcmp(l->name,"mode"))
	    {
	      i=choose(mode_list,l->value);
	      if (i==-1) i=0;
	      set_mode_alzo(mode_value[i]);
	    }
	  else if (!strcmp(l->name,"authors"))
	    {
	      authors=2;
	    }
	  else if (!strcmp(l->name,"options"))
	    {
	      authors=10;
	    }
	  else if (!strcmp(l->name,"search"))
	    {
	      *end_of_list(&selection)=findsong(l->value,index);
	    }
	  else if (!strcmp(l->name,"find"))
	    {
	      *end_of_list(&selection)=getsong(l->value,index);
	    }
	  else
	    {
	      fprintf(output,"%%Campo sconosciuto: %s\n",l->name);
	    }
	}
    }
  if (authors==1 || authors==2)
    {
      switch(lang)
	{
	case 5:
	  /*	  out_txt();*/
	  out_nomime();
	  out_authors(output);
	  break;
	case 3:
	  out_mime();
	  out_ps();
	  ps_authors_index(output,index);
	  break;
	case 1:
	default:
	  out_mime();
	  set_html_output(output);
	  open_html("Canzoniere: elenco cantanti",BASE_URL);
	  if (authors==1)
	    {
	      while ((l=find_couple(list,"authors"))!=NULL
		     )
		list=delete_couple(list,l);
	      s=new_string(NULL);
	      s=string_cat(s,MY_ADDRESS);
	      s=string_cat(s,"?");
	      s=string_merge(s,url(list));
	      s=string_cat(s,"find=%s;");
	      set_HREF(s.s);
	      out_authors_html(output);
	    }
	  else
	    {
	      while ((l=find_couple(list,"authors"))!=NULL ||
		     (l=find_couple(list,"VOID"))!=NULL)
		list=delete_couple(list,l);
	      for (i=1,p=biblio;p!=NULL;p=p->next,i++)
		{
		  q=p->next;
		  p->next=NULL;
		  s=new_string(NULL);
		  s=string_cat(s,MY_ADDRESS);
		  s=string_cat(s,"?");
		  c=strchr(s.s,'?');
		  if (c==NULL) c=s.s;
		  for (g=index;g!=NULL;g=g->next)
		    {
		      if (has_written(p,g->first))
			{
			  if (strstr(c,g->first->file->name)==NULL)
			    {
			      string_cat(s,"VOID=");
			      string_cat(s,g->first->file->name);
			      string_cat(s,";");
			    }
			}
		    }
		  s=string_merge(s,url(list));
		  s=string_cat(s,"find=%s;");
		  set_HREF(s.s);
		  out_one_author(i,p);
		  p->next=q;
		}
	    }
	  out_closing();
	  close_html();
	  break;
	}
    }
  else if (authors==10) /*opzioni*/
    {
      while ((l=find_couple(list,"options"))!=NULL
	     )
	list=delete_couple(list,l);
      set_html_output(output);
      out_mime();
      open_html("Altre opzioni",BASE_URL);
      altre_opzioni(list);
      out_closing();
      close_html();
    }
  else
    {
      nfound=indexlen(selection);
      if (nfound>0)
	{
	  if (max<0 || nfound<=max)
	    {
	      switch(lang)
		{
		case 4:
		  /* out_txt();*/
		  out_nomime();
		  out_songs_src(selection,output);
		  break;
		case 5:
		  /*out_txt();*/
		  out_nomime();
		  out_songs(selection,output);
		  break;
		case 3:
		  out_mime();
		  out_ps();
		  ps_songs(selection,1,output);
		  break;
		case 1:
		default:
		  out_mime();
		  set_html_output(output);
		  open_html("Canzoni",BASE_URL);
		  list=delete_void_files(list);
		  out_change_lang(list);
		  fprintf(output,"<HR>\n");
		  if (nfound>1)
		    fprintf(output,"<strong> Ho trovato %d corrispondenze </strong><P>\n",nfound);
		  while ((l=find_couple(list,"search"))!=NULL
			 ||(l=find_couple(list,"find"))!=NULL
			 ||(l=find_couple(list,"VOID"))!=NULL
			 )
		    list=delete_couple(list,l);
		  s=new_string(NULL);
		  s=string_cat(s,MY_ADDRESS);
		  s=string_cat(s,"?");
		  s=string_merge(s,url(list));
		  set_HREF(s.s);
		  out_songs_html(selection,output);
		  out_closing();
		  close_html();
		  break;
		}
	    }
	  else
	    {
	      switch(lang)
		{
		case 5:
		  /*out_txt();*/
		  out_nomime();
		  out_index(output,selection);
		  break;
		case 3:
		  out_mime();
		  out_ps();
		  ps_index(output,selection);
		  break;
		case 1:
		default:
		  out_mime();
		  set_html_output(output);
		  open_html("Elenco Canzoni",BASE_URL);
		  out_change_lang(list);
		  fprintf(output,"<HR>");
		  while ((l=find_couple(list,"search"))
			 ||(l=find_couple(list,"find"))
			 ||(l=find_couple(list,"VOID")))
		    list=delete_couple(list,l);
		  s=new_string(NULL);
		  s=string_cpy(s,MY_ADDRESS);
		  s=string_cat(s,"?");
		  s=string_merge(s,url(list));
		  s=string_cat(s,"VOID=%s;find=%s;");
		  set_HREF(s.s);
		  fprintf(output,"<strong> Ho trovato %d corrispondenze </strong><P>\n",indexlen(selection));
		  out_index_html(output,selection);
		  out_closing();
		  close_html();
		  break;
		}
	    }
	}
      else
	{
	  if (lang==1)
	    {
	      out_mime();
	      set_html_output(output);
	      open_html("Canzoniere",BASE_URL);
	      fprintf(output,"Non ho trovato nessuna corrispondenza. <P>\n");
	      close_html();
	    }
	  else
	    {
	      out_nomime();
	      fprintf(output,"Non ho trovato nessuna corrispondenza.\n");
	    }
	}
    }
  return 0;
}	   



