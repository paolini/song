#define GOSCRIPT_TEMP_FILE "delme.ps"
#define GOSCRIPT_EXECUTABLE "songgosc.bat"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "song.h"
#include "songout.h"
#include "songprin.h"

#ifdef DOS
#include <conio.h>
#include <process.h>
#include <dir.h>
#include "mouse.h"
#include "songgrap.h"
#endif

#ifdef BORLAND
#include <alloc.h>
#endif

#ifdef UNIX
#define stdprn stdout
#define BLACK 0
#define LIGHTRED 0
#define WHITE 0
#define BROWN 0
#define GREEN 0
#define YELLOW 0
#define LIGHTGRAY 0
#define LIGHTBLUE 0
#define RED 0
#define BLUE 0


int getch(void)
{
	char s[3];
	fflush(stdin);
	fgets(s,2,stdin);
	return s[0];
}

#endif

/*colori:*/
int BACKGROUND_COLOR=BLACK;
int SELECTION_COLOR=YELLOW;
int NORMAL_COLOR=WHITE;

int TITLE_COLOR=LIGHTRED;
int AUTHOR_COLOR=LIGHTBLUE;
int STROPHE_COLOR=WHITE;
int REFRAIN_COLOR=YELLOW;
int SPOKEN_COLOR=LIGHTGRAY;
int CHORDS_COLOR=GREEN;
int REFERENCE_COLOR=BROWN;

int write_contents=1;

int mouse_mode=0;

#include <ctype.h>

#include "chords.h"


struct song_list *index=NULL,*selection=NULL;

void deb(int n)
	{
	  printf("Ciao %d...\n",n);
#ifdef DOS
/*	printf("MEM(%d): %u check:%d \n",n,coreleft(),heapcheck());
*/
#endif
/*	getch();*/
	}

#ifdef DOS
/* Interfaccia utente */
int premi(void)
	{
	return getch();
	}
int mygetch(void)
{
  int x,y;
  if (mouse_mode)
    {
      while (1)
	{
/*	  if (mouse_pressed(0))
	    {
	      mouse_xy(&x,&y);
	      while(mouse_pressed(0));
	      return(y/8+256);
	    }
            */
	  if(kbhit())
	    return getch();
	}
    }
  else
    return getch();
}
char menu(char *title,char *letter,char *list)
{
  char *p,*q;
  int c;
  int n,y,i,pos=0,exit=0;
  for (n=0,p=list;(q=strchr(p,'\n'))!=NULL;p=q+1,n++)
    *q='\0';
  textbackground(BACKGROUND_COLOR);
  textcolor(TITLE_COLOR);
  _setcursortype(_NOCURSOR);
  printf("\n");
  cprintf("%s",title);
  printf("\n");
  y=wherey();
  if (y>=25-n) y=25-n;
  for (i=0;i<n;i++) printf("\n");
  while(!exit)
    {
      if (pos<0) pos=n-1;
      if (pos>=n) pos=0;
      textcolor(NORMAL_COLOR);
      gotoxy(1,y);
      for (i=0,p=list;i<n;i++,p=strchr(p,'\0')+1)
	{
	  textbackground(i==pos?SELECTION_COLOR:BACKGROUND_COLOR);
	  cprintf("%s",p);
	  printf("\n");
	}
      textbackground(BACKGROUND_COLOR);
      c=mygetch();
      if (c>=256)
	{
	  c=letter[c-256-y+1];
	  exit=1;
	}
      for (i=0;i<n;i++)
	if (c==letter[i]) pos=i;
      if (i==n)
	{
	  switch(c)
	    {
	    case 0:
	      switch(getch())
		{
		case 'H':pos--;break;
		case 'P':pos++;break;
		}
	      break;
	    case 27:exit=1;break;
	    case 13:c=letter[pos];exit=1;break;
	    case ' ':
	    case '\t':pos++;break;
		}
	}
    }
  for (i=0,p=list;i<n;i++,p++)
    {
      p=strchr(p,'\0');
      *p='\n';
    }
  textcolor(NORMAL_COLOR);
  textbackground(BACKGROUND_COLOR);
  _setcursortype(_NORMALCURSOR);
  clreol();
  cprintf("%c",13);
  return c;
}
#endif
#ifdef UNIX
int premi(void)
	{
	fflush(stdin);
	return getchar();
	}
char menu(char *title,char *letter,char *list)
{
  char *p,*q;
  char s[5];
  int n,i,exit=0;
  printf("\n%s\n",title);
  for (i=0,p=list;*p!='\0';i++)
    {
    printf("%d) ",i+1);
    for (;*p!='\n' && *p!='\0';p++)
      putchar(*p);
    putchar('\n');
    if (*p=='\n') p++;
    }
  n=i;
  while(!exit)
    {
      fflush(stdin);
      fgets(s,4,stdin);
      if (strchr(s,'\n')) *strchr(s,'\n')='\0';
      fflush(stdin);
      if (s[0]==27 || s[0]=='\0')
	{
	  s[0]=27;
	  exit=1;
	}
      else
	{
	    {
	      i=atoi(s)-1;
	      if (i>=0 && i<n)
		{
		  s[0]=letter[i];
		  exit=1;
		}
	    }
	}
    }
  return s[0];
}
#endif

void more(void)
{premi();}


#ifdef UNIX

#endif
int ask(char *s)
{
  switch(menu(s,"sn","SI\nNO\n"))
    {
    case 's':return 1;
    case 'n':return 0;
    }
  return 0;
}
void error(char *s)
{
  if (menu(s,"AB","VABBE!\nInterrompi l'esecuzione\n")=='B')
    exit(1);
}
void fatal_error(char *s)
	{
	fprintf(stderr,"Errore irrimediabile: %s\n\n",s);
	exit(1);
	}
int readint(void)
	{
	int n;
	fflush(stdin);
	scanf("%d",&n);
	return n;
	}

char *getfilename(char *ext)
	{
	static char name[40];
	fflush(stdin);
	fgets(name,35,stdin);
	*strchr(name,'\n')='\0';
	if (strchr(name,'.')==NULL)
		{
		strcat(name,".");
		strcat(name,ext);
		}
	return name;
	}

int max_found=0; /*0=infinito*/
FILE *output;/*file di output (vedi sotto)*/
char *output_filename=0;
int output_mode=0; /*0:console, 1:stdout, 2:file, 3:stampante, 4:goscript*/

int language=0; /*0:CONSOLE, 1:HTML, 2:SNG, 3:PS, 4:SOURCE, 5:TESTO, 10:XML*/
int first_page_number=0;
static int mode_chords=1;
/*
	0: non mette accordi
	1: mette accordi
	3: mette riferimenti
	4: mette copie (ma senza accordi)
	5: mette copie con accordi
	7: mette copie e riferimenti con accordi
	*/
static char *HREF;

static int alzo=0,mode_alzo=0;
#ifdef BORLAND
int console(struct song_list *sel,int pos)
	{
	pos=console_out(sel,pos);
	set_alzo(alzo);
	set_mode_alzo(mode_alzo);
	set_mode_chords(mode_chords);
	return pos;
	}
#else
int console(struct song_list *sel,int pos)
	{
	set_mode_chords(7);
	out_song_con(indexsong(pos,sel),more);
	set_alzo(alzo);
	set_mode_alzo(mode_alzo);
	set_mode_chords(mode_chords);
	return pos;
	}
#endif

#ifdef DOS
/*Visualizza index, evidenziando selection.
 poi torna il carattere premuto e mette la posizione in n*/

int outindex(struct song_list *index,struct song_list *selection,int *res)
{
  struct song *s;
  struct author *p;
  static char key[20]="";
  int len=23;
  struct info *h;
  char a,b;
  int c;
  int done=0;

  int pos=0,n,i,from=0,to;
  if (res) pos=*res;
  n=indexlen(index);
  _setcursortype(_NOCURSOR);
  if (n==0)
    {
    cprintf("\nNon c'e` nessuna canzone da visualizzare\n");
    return 0;
    }
  clrscr();
  do
    {
    if (pos<0) pos=0;
    if (pos>=n) pos=n-1;
    if (pos<(len-1)/2)
      from=0;
    else if (pos>=n-(len-1)/2 && n>=len-1)
      from=n-len;
    else
      from=pos-(len-1)/2;
    to=from+len;
    for (i=from;i<to;i++)
      {
      if (kbhit())
	{
	if(i<pos) i=pos;
	else break;
	}
      gotoxy(1,i-from+1);
      if (i>=0 && i<n)
	{
	s=indexsong(i,index);
	textbackground(i==pos?SELECTION_COLOR:BACKGROUND_COLOR);
	textcolor(NORMAL_COLOR);
	cprintf("%c%3d) ",selection!=NULL&&(selected(s,selection)!=NULL)?'*':' ',i+1);
	textcolor(TITLE_COLOR);
	cprintf("%s ",s->head.l->s);
	if (s->author!=NULL)
	  {
	  textcolor(AUTHOR_COLOR);
	  cprintf("(%s",s->author->name->s);
	  for (p=s->author->next;p!=NULL;p=p->next)
	  cprintf(", %s",p->name->s);
	  cprintf(")");
	  }
	clreol();
	if (i==pos)
	  {
	  gotoxy(1,24);
	  if (s->flag&SONG_INDEX)
	     cprintf("index; ");
	  if (s->file)
	    cprintf("file: %s; ",s->file->name);
	  if ((s->flag&SONG_NOOFFSET)==0)
	    cprintf("offset: %lu; ",s->offset);
	  for (h=s->head.next;h!=NULL;h=h->next)
	    {
	    cprintf("%s: %s; ",head_name(h->type),h->l->s);
	    }
	  clreol();
	  }
	}
      else
	{
	textcolor(NORMAL_COLOR);
	textbackground(BACKGROUND_COLOR);
	clreol();
	}
      }
    printf("\n");
    s=indexsong(pos,index);
    textcolor(NORMAL_COLOR);
    textbackground(BACKGROUND_COLOR);
    if (res)
      *res=pos;
      switch(c=mygetch())
	{
	case '/':
	case '?':
	  a=key[0];b=key[1];
	  _setcursortype(_NORMALCURSOR);
	  clreol();
	  gotoxy(1,wherey());
	  printf("Chiave di ricerca [%s]:",key);
	  fflush(stdin);
	  fgets(key,19,stdin);
	  _setcursortype(_NOCURSOR);
	  *strchr(key,'\n')='\0';
	  if (key[0]==0)
	    {
	      key[0]=a;
	      key[1]=b;
	    }
	  for (i=pos+1;i<n;i++)
	    {
	      s=indexsong(i,index);
	      if (songcmp(s,key))
		{
		  pos=i;
		  break;
		}
	    }
	  break;
	case 0:
	  switch(getch())
	    {
	    case 'G':pos=0;break;
	    case 'H':pos--;break;
	    case 'I':pos-=10;break;
	    case 'O':pos=n-1;break;
	    case 'P':pos++;break;
	    case 'Q':pos+=10;break;
	    }
	  break;
	default:
	  done=1;
	  break;
/*	  if (c==255+12)
			{
				if (choose) return s;
				pos=console_out(sel,pos);
			}
		else
			pos+=c-255-12;
		break;*/
	}
  } while (!done);
  textcolor(NORMAL_COLOR);
  textbackground(BACKGROUND_COLOR);
  _setcursortype(_NORMALCURSOR);
  return c;
}
#endif
#ifdef UNIX
int outindex(struct song_list *index,struct song_list *selection,int *res)
{
  struct song *s;
  struct author *p;
  static char key[20]="";
  struct info *h;
  char a,b;
  int c;
  char st[5];
  int done=0;
  int len=15;

  int pos=0,n,i,from=0,to;
  if (res) pos=*res;
  n=indexlen(index);
  if (n==0)
    {
    printf("\nNon c'e` nessuna canzone da visualizzare\n");
    return 0;
    }
  do
    {
    if (pos<0) pos=0;
    if (pos>=n) pos=n-1;
    if (pos<(len-1)/2)
      from=0;
    else if (pos>=n-(len-1)/2 && n>=len-1)
      from=n-len;
    else
      from=pos-(len-1)/2;
    to=from+len;
    for (i=from;i<to;i++)
      {
      if (i>=0 && i<n)
	{
	s=indexsong(i,index);
	printf("%c",selection!=NULL&&(selected(s,selection)!=NULL)?'*':' ');
	printf("%3d",i+1);
	if (i==pos)
	  printf("->");
	else 
	  printf(") ");
      	printf("%s ",s->head.l->s);
	if (s->author!=NULL)
	  {
	  printf("(%s",s->author->name->s);
	  for (p=s->author->next;p!=NULL;p=p->next)
	  printf(", %s",p->name->s);
	  printf(")");
	  }
	printf("\n");
	}
      }
    printf("\n");
    s=indexsong(pos,index);
    if (1)
      {
	printf("[");
	if (s->flag&SONG_INDEX)
	  printf("index; ");
	if (s->file)
	  printf("file: %s; ",s->file->name);
	if ((s->flag&SONG_NOOFFSET)==0)
	  printf("offset: %lu; ",s->offset);
	for (h=s->head.next;h!=NULL;h=h->next)
	  {
	    printf("%s: %s; ",head_name(h->type),h->l->s);
	  }
	printf("]\n");
      }
    if (res)
      *res=pos;
    fflush(stdin);
    fgets(st,4,stdin);
    if (strchr(st,'\n')) *strchr(st,'\n')=0;
    i=atoi(st);
    if (i)
      c='g';
    else
      c=st[0];
    fflush(stdin);
    switch(c)
      {
      case 'g':
	pos=i-1;
	break;
      case '/':
      case '?':
	a=key[0];b=key[1];
	printf("Chiave di ricerca [%s]:",key);
	fflush(stdin);
	fgets(key,19,stdin);
	*strchr(key,'\n')='\0';
	if (key[0]==0)
	  {
	    key[0]=a;
	    key[1]=b;
	  }
	for (i=pos+1;i<n;i++)
	  {
	    s=indexsong(i,index);
	    if (songcmp(s,key))
	      {
		pos=i;
		break;
	      }
	  }
	break;
      case 'H':pos=0;break;
      case 'p':pos--;break;
      case 'P':pos-=10;break;
      case 'E':pos=n-1;break;
      case 'n':pos++;break;
      case '\0':
      case 'N':pos+=10;break;
      default:
	done=1;
	break;
      }
  } while (!done);
  return c;
}

#endif


struct song *select_one(struct song_list *index)
  {
  int pos=0;
  for (;;)
    {
    switch(outindex(index,NULL,&pos))
      {
      case 'x':
      case 27:return NULL;
      case 'q':
      case 13:
	return indexsong(pos,index);
      case 0:return NULL;
      }
    }
  }
struct song_list *view_and_select(struct song_list *index,struct song_list *selection)
  {
  int pos=0;
  for (;;)
    {
    switch(outindex(index,selection,&pos))
      {
      case 'x':
      case 'q':
      case 27:return selection;
      case '+':alzo++;break;
      case '-':alzo--;break;
      case '=':alzo=0;break;
      case ' ':
	selection=toggle_selection(indexsong(pos,index),selection);
	pos++;
      break;
      case 'v':
      case 13:
	pos=console(index,pos);
	break;
      case 0:return selection;
      }
    }
  }
void only_view(struct song_list *index)
  {
  int pos=0;
  for (;;)
    {
    switch(outindex(index,NULL,&pos))
      {
      case 'x':
      case 'q':
      case 27:return;
      case '+':alzo++;break;
      case '-':alzo--;break;
      case '=':alzo=0;break;
      case 'v':
      case 13:
	pos=console(index,pos);
      break;
      case 0: return;
      }
    }
  }

#ifdef UNIX
struct song* outbiblio()
{
  struct author *p;
  struct song_list *list;
  int c;
  char str[5];
  char *s;
  int pos=0,n,i;
  do
    {
      n=bibliolen();
      if (n==0)
	{
	  printf("\nNon c'e` nessun autore da visualizzare\n");
	  return NULL;
	}
      if (pos<0) pos=0;
      if (pos>=n) pos=n-1;
      for (i=-7;i<8;i++)
	{
	  if (i+pos>=0 && i+pos<n)
	    {
	    p=biblioauthor(i+pos);
	    printf("%3d",i+pos+1);
	    if (i==0) printf("->");
	    else printf(") ");
	    s=surname(p->name->s);
	    printf("%s",s);
	    if (s!=p->name->s)
	      {
		c=*(s-1);
		*(s-1)='\0';
		printf(" (%s)",p->name->s);
		*(s-1)=c;
	      }
	    printf("\n");
	  }
      }
    printf("\n");
    p=biblioauthor(pos);
    fflush(stdin);
    fgets(str,4,stdin);
    if (strchr(str,'\n')) *strchr(str,'\n')='\0';
    i=atoi(str);
    fflush(stdin);
    if (i)
      c='g';
    else
      c=str[0];
    switch(c)
      {
      case 'g': pos=i-1;break;
      case 'q':
      case 'x':
      case 27: return NULL;
      case 'v':
	list=select_author(p,index);
	selection=view_and_select(list,selection);
	freelist(list);
	break;
      case '\0':
      case 'N':	pos+=10;break;
      case 'P': pos-=10;break;
      case 'n': pos++;break;
      case 'p': pos--;break;
      case 'H': pos=0;break;
      case 'E': pos=n-1;break;
      }
    } while (1);
}
#endif
#ifdef DOS
struct song* outbiblio()
{
struct author *p;
struct song_list *list;
int c;
char *s;
int pos=0,n,i;
do
  {
    n=bibliolen();
    if (n==0)
      {
	cprintf("\nNon c'e` nessun autore da visualizzare\n");
	return NULL;
      }
    if (pos<0) pos=0;
    if (pos>=n) pos=n-1;
    clrscr();
    for (i=-11;i<12;i++)
      {
	gotoxy(1,i+12);
	if (i+pos>=0 && i+pos<n)
	  {
	    p=biblioauthor(i+pos);
	    textbackground(i==0?SELECTION_COLOR:BACKGROUND_COLOR);
	    textcolor(NORMAL_COLOR);
	    cprintf("%3d) ",i+pos+1);
	    textcolor(AUTHOR_COLOR);
	    s=surname(p->name->s);
	    cprintf("%s",s);
	    if (s!=p->name->s)
	      {
		c=*(s-1);
		*(s-1)='\0';
		cprintf(" (%s)",p->name->s);
		*(s-1)=c;
	      }
	  }
      }
    printf("\n");
    p=biblioauthor(pos);
    textcolor(NORMAL_COLOR);
    textbackground(BACKGROUND_COLOR);
    switch(c=mygetch())
      {
      case 27: return NULL;
      case 13:
	list=select_author(p,index);
	selection=view_and_select(list,selection);
	freelist(list);
	break;
      case 0:
	switch(getch())
	  {
      case 'G':pos=0;break;
      case 'H':pos--;break;
	case 'I':pos-=10;break;
	case 'O':pos=n-1;break;
	case 'P':pos++;break;
	case 'Q':pos+=10;break;
	}
	break;
default:
	if (c==255+12)
		{
		selection=select_author(p,index);
		only_view(selection);
		freelist(selection);
		}
	else
		pos+=c-255-12;
	break;
}
} while (1);
}
#endif


static int save_language,save_output_mode;
void save_mode(void)
{
save_language=language;
save_output_mode=output_mode;
}
void load_mode(void)
{
language=save_language;
output_mode=save_output_mode;
}

/*
struct song *conindex(int sel, int choose)
  {
  struct song *p;
  save_mode();
  language=0;output_mode=0;output=NULL;
  p=outindex(sel,choose);
  load_mode();
  return p;
  }
*/

/* Programma principale */

void open_output(void)
	{
	char *ext;
	switch(output_mode)
		{
		case 1:
			output=stdout;
			break;
		case 2:
		  if (output_filename)
		    output=fopen(output_filename,"wb");
		  else {
		    printf("Nome del file di output:");
		    switch(language)
		      {
		      case 1:ext="html";break;
		      case 2:ext="sng";break;
		      case 3:ext="ps";break;
		      case 4:ext="sng";break;
		      case 10:ext="xng";break;
		      default:ext="";break;
		      }
		    output=fopen(getfilename(ext),"wb");
		  }
		  if (output==NULL)
		    {
		      error("Non riesco ad aprire il file");
		      output=stdout;
		    }
		  break;
		case 3:
			output=stdprn;break;
		case 4:
		  
		  output=fopen(GOSCRIPT_TEMP_FILE,"wt");
			if (output==NULL)
			{
			error("Non riesco ad aprire il file temporaneo in c:\\goscript\\");
			output=stdprn;
			}
			break;
		}
	}

void close_output(void)
{
  if (output_mode>1 && output_mode!=3)
	fclose(output);
  if (output_mode==4)
	{
	char *s;
	s=(char *)malloc(sizeof(char)*(strlen(GOSCRIPT_EXECUTABLE)+
		strlen(GOSCRIPT_TEMP_FILE)+2));
	sprintf(s,"%s %s",GOSCRIPT_EXECUTABLE,GOSCRIPT_TEMP_FILE);
	system(s);
	free(s);
	}
}
/* gestione opzioni */


char *config_file="song.cfg";


char *read_until(FILE *fp,int c)
{
  int i;
  static char buf[80];
	for(i=0;i<80 && (buf[i]=getc(fp))!=c && !feof(fp);i++);
	if (buf[i]==c)
		{
		buf[i]=0;
		return buf;
		}
	else
		return NULL;
}

int strint(char *s)
	{
	int r;
	sscanf(s,"%d",&r);
	return r;
	}

void songout_options_save(FILE *fp)
	{
	  fprintf(fp,"WRITE_CONTENTS=%d\n",write_contents);
	  fprintf(fp,"CHORD_REFERENCE=%d\n",get_mode_chords());
	if (HREF==NULL)
		fprintf(fp,"HREF=\n");
	else
		fprintf(fp,"HREF=%s\n",HREF);
	fprintf(fp,"TRANSPOSE=%d\n",get_alzo());
	fprintf(fp,"CHORD_LAYOUT=%d\n",get_mode_alzo());
	fprintf(fp,"MAX_FOUND=%d\n",max_found);
	fprintf(fp,"LANGUAGE=%d\n",language);
	fprintf(fp,"OUTPUT=%d\n",output_mode);
	fprintf(fp,"BACKGROUND_COLOR=%d\n",BACKGROUND_COLOR);
	fprintf(fp,"SELECTION_COLOR=%d\n",SELECTION_COLOR);
	fprintf(fp,"NORMAL_COLOR=%d\n",NORMAL_COLOR);
	fprintf(fp,"TITLE_COLOR=%d\n",TITLE_COLOR);
	fprintf(fp,"AUTHOR_COLOR=%d\n",AUTHOR_COLOR);
	fprintf(fp,"STROPHE_COLOR=%d\n",STROPHE_COLOR);
	fprintf(fp,"REFRAIN_COLOR=%d\n",REFRAIN_COLOR);
	fprintf(fp,"SPOKEN_COLOR=%d\n",SPOKEN_COLOR);
	fprintf(fp,"CHORDS_COLOR=%d\n",CHORDS_COLOR);
	fprintf(fp,"REFERENCE_COLOR=%d\n",REFERENCE_COLOR);
	}
int songout_options_load(char *name,char *val)
{
  if (!strcmp(name,"WRITE_CONTENTS"))
    write_contents=strint(val);
  else if (!strcmp(name,"CHORD_REFERENCE"))
    set_mode_chords(mode_chords=strint(val));
  else if (!strcmp(name,"HREF"))
    {
      if (*val=='\0')
	set_HREF(HREF=NULL);
      else
	set_HREF(HREF=strdup(val));
    }
  else if (!strcmp(name,"TRANSPOSE"))
    set_alzo(alzo=strint(val));
  else if (!strcmp(name,"CHORD_LAYOUT"))
    set_mode_alzo(mode_alzo=strint(val));
  else if (!strcmp(name,"MAX_FOUND"))
    max_found=strint(val);
  else if (!strcmp(name,"LANGUAGE"))
    language=strint(val);
  else if (!strcmp(name,"OUTPUT"))
    output_mode=strint(val);
  else if (!strcmp(name,"BACKGROUND_COLOR"))
    BACKGROUND_COLOR=strint(val);
  else if (!strcmp(name,"SELECTION_COLOR"))
    SELECTION_COLOR=strint(val);
  else if (!strcmp(name,"NORMAL_COLOR"))
    NORMAL_COLOR=strint(val);
  else if (!strcmp(name,"TITLE_COLOR"))
    TITLE_COLOR=strint(val);
  else if (!strcmp(name,"AUTHOR_COLOR"))
    AUTHOR_COLOR=strint(val);
  else if (!strcmp(name,"STROPHE_COLOR"))
    STROPHE_COLOR=strint(val);
  else if (!strcmp(name,"REFRAIN_COLOR"))
    REFRAIN_COLOR=strint(val);
  else if (!strcmp(name,"SPOKEN_COLOR"))
    SPOKEN_COLOR=strint(val);
  else if (!strcmp(name,"CHORDS_COLOR"))
    CHORDS_COLOR=strint(val);
  else if (!strcmp(name,"REFERENCE_COLOR"))
    REFERENCE_COLOR=strint(val);
  else if (!strcmp(name,"DIRECTORY"))
      chdir(val);
  else
    return 0;
  return 1;
}
#ifdef DOS
#endif
#ifdef UNIX
void songcon_options_save(FILE *fp){}
int songcon_options_load(char *s,char *t){return 0;}
#endif

void save_options(void)
{
	FILE *fp;
  fp=fopen(config_file,"wt");
	if (fp==NULL)
		{
		error("Non riesco ad aprire il file");
		return;
		}
	songout_options_save(fp);
        #ifdef BORLAND
	songcon_options_save(fp);
        #endif
	songprin_options_save(fp);
	fclose(fp);
}
void load_options(void)
{
  char *value;
  char name[40];
  FILE *fp;
  fp=fopen(config_file,"rt");
  if (fp==NULL) {
    char s[200];
    char *p=getenv("HOME");
    fprintf(stderr,"warning: cannot open %s\n",config_file);
    if (p) {
      strncpy(s,p,100);
      if (strlen(s)) {
	strcat(s,"/.songrc");
        config_file=strdup(s);
	fp=fopen(config_file,"rt");
        if (!fp) fprintf(stderr,"warning: cannot open %s\n",config_file);
      }
    }
    if (!fp) {
          return;
    }
  }
  fprintf(stderr,"reading config file %s\n",config_file);
  do
    {
      value=read_until(fp,'=');
      if (value!=NULL)
	{
	  strcpy(name,value);
	  value=read_until(fp,'\n');
	  if (!songout_options_load(name,value) &&
              #ifdef BORLAND
	      !songcon_options_load(name,value) &&
              #endif
	      !songprin_options_load(name,value))
	    fprintf(stderr,"Opzione sconosciuta: %s\n",name);
	}
    }
  while (value!=NULL);
  fclose(fp);
}
char *MODE_CHORDS(void)
	{
	switch(mode_chords)
		{
		case 0: return "NIENTE ACCORDI";
		case 1: return "SOLO ACCORDI SEGNATI";
		case 3: return "RIFERIMENTI AD ACCORDI";
		case 4: return "METTE ANCHE LE COPIE MA SENZA ACCORDI";
		case 5: return "METTE LE COPIE";
		case 7: return "RIFERIMENTI E COPIE DI ACCORDI";
		default: return "#error#";
		}
	}
char *LANGUAGE(void)
	{
	switch(language)
		{
		case 0:return "CONSOLE";
		case 1:return "HTML";
		case 2:return "SONG";
		case 3:return "POSTSCRIPT";
		case 4:return "SORGENTE";
		case 5:return "TESTO";
		case 10:return "XML";
		default:return "";
		}
	}
char *OUTPUT(void)
	{
	switch(output_mode)
		{
		case 0:return "CONSOLE";
		case 1:return "VIDEO";
		case 2:return "FILE";
		case 3:return "STAMPANTE";
		case 4:return "GOSCRIPT";
		default: return "";
		}
	}
char *MODE_ALZO(void)
{
static char s[10];
if (mode_alzo&LASCIA)
	strcpy(s,"AsIs");
else
	{
		strcpy(s,"dom/mi ");
		if (mode_alzo&MAIUSCOLO)
			s[0]='D';
		if (mode_alzo&TUTTO_MAIUSCOLO)
			s[1]='O';
		if (mode_alzo&MINORE_MENO)
			s[2]='-';
		if (mode_alzo&BASSO_TRA_PARENTESI)
			{s[3]='(';s[6]=')';}
		if (mode_alzo&BASSO_MAIUSCOLO)
			s[4]='M';
		if (mode_alzo&BASSO_TUTTO_MAIUSCOLO)
			s[5]='I';
	}
return s;
}

void scegli_output(void)
{
  printf("Output di stampa: %s\n",OUTPUT());
  switch(menu("Scegli l'output",
	      "VSGF",
	      "Video\nStampante\nStampante con GoScript\nFile\n"))
    {
    case 'V': output_mode=1;break;
    case 'S': output_mode=3;break;
    case 'G': output_mode=4;break;
    case 'F': output_mode=2;break;
    }
}

void scegli_language(void)
	{
	printf("\nLinguaggio: %s\n",LANGUAGE());
	switch(menu(
		"Scegli il formato di Stampa",
		"TSQPHX",
		"File di testo   .txt\n"
		"File sorgente   .sng\n"
		"File originario .sng\n"
		"File postscript .ps\n"
		"File HTML       .html\n"
		"File XML        .xng\n"))
		{
		case 'T':language=5;break;
		case 'S':language=2;break;
		case 'Q':language=4;break;
		case 'P':language=3;break;
		case 'H':language=1;break;
		case 'X':language=10;break;
		}
	}
void scegli_alzo(void)
	{
	printf("\nTrasposizione degli accordi: %d\n",alzo);
	printf("Nuovo valore: ");
	alzo=readint();
	}
void scegli_accordi(void)
	{
	printf("\nStampa degli accordi: %s\n",MODE_CHORDS());
	switch(menu(
		"Scegli la stampa degli accordi",
		"013457",
		"Senza accordi\n"
		"Solo accordi segnati\n"
		"Anche accordi nei riferimenti\n"
		"Anche copie ma senza accordi\n"
		"Anche accordi nelle copie\n"
		"Accordi sia nei riferimenti che nelle copie\n"
		))
		{
		case '0': mode_chords=0;break;
		case '1': mode_chords=1;break;
		case '3': mode_chords=3;break;
		case '4': mode_chords=4;break;
		case '5': mode_chords=5;break;
		case '7': mode_chords=7;break;
		}
	}


int scegli_canzoni(void)
{
  switch(menu("Scegli le canzoni",
	"ABCX",
	"Scegli una canzone dall'indice\n"
	"Scegli una canzone dalla selezione\n"
	"Tutte le canzoni della selezione\n"
	"Ho cambiato idea, non voglio stampare\n"
	))
    {
    case 'A':
	freelist(selection);
	selection=add_to_selection(select_one(index),NULL);
    break;
    case 'B':
	freelist(selection);
	selection=add_to_selection(select_one(selection),NULL);
    break;
    case 'C':
    break;
    default:
    return 0;
    }
return 1;
}
int scegli_canzoni_index(void)
{
  switch(menu("Scegli le canzoni",
			"ABX",
			"Tutte le canzoni della selezione\n"
			"Tutte le canzoni\n"
			"Ho cambiato idea, non voglio stampare\n"
		  ))
    {
	case 'A':
		break;
	case 'B':
		freelist(selection);
		selection=copy_list(index);
		break;
	default:
	  return 0;
	}
  return 1;
}
void options_menu(void)
{
	while(1)
	{
	  switch(menu(
		  "OPZIONI",
			"OLATSCX",
			"Scegli l'output di stampa\n"
			"Scegli il linguaggio di stampa\n"
			"Scegli modalita' accordi\n"
			"Scegli trasposizione degli accordi\n"
			"Salva la configurazione\n"
		  "Carica la configurazione\n"
		  "Menu` principale\n"))
	{
	case 'T':scegli_alzo();break;
	case 'O':scegli_output();break;
	case 'L':scegli_language();break;
  case 'A':scegli_accordi();break;
	case 'X':case 27:
	  return;
	case 'S':
		save_options();
	  break;
	case 'C':
	load_options();
		break;
	}
	}
}
void print_songs(void)
{
  struct song_list *p;
  if (selection==NULL) {
    printf("Nessuna canzone selezionata!\n");
    return;
  }
  open_output();
  if (output==NULL) return;
  if (language==2)
    {
      for (p=selection;p!=NULL;p=p->next)
	writesong(p->first,output);
    }
  else if (language==4)
    out_songs_src(selection,output);
  else if (language==1)
    out_songs_html(selection,output);
  else if (language==10)
    out_songs_xml(selection,output);
  else if (language==5)
    out_songs(selection,output);
  else if (language==3)
    {
      if (first_page_number<1) {
	printf("Numero della prima pagina: ");
	selection=ps_songs(selection,readint(),output);
      } else 
	selection=ps_songs(selection,first_page_number,output);
    }
  close_output();
}
void print_index(void)
{
  open_output();
  if (output==NULL) return;
  switch(language)
    {
    case 1:out_index_html(output,selection);break;
    case 2:writeindex(selection,output);break;
    case 3:ps_index(output,selection);break;
    case 5:out_index(output,selection);break;
    default:error("Non ancora implementato");break;
    }
  close_output();
}
void stampa_index_menu(void)
{
  printf("\nPreparazione alla stampa di un indice:\n");
  printf("Formato: %s; \nOutput: %s\n",LANGUAGE(),OUTPUT());
  if (scegli_canzoni_index())
    print_index();
}
void stampa_menu(void)
{
	printf("\nPreparazione alla stampa:\n");
	printf("Formato: %s; \nOutput: %s\nAccordi: %s\nTipo accordi: %s\n"
		"Trasposizione: %d\n",LANGUAGE(),OUTPUT(),MODE_CHORDS(),MODE_ALZO(),alzo);
	if (scegli_canzoni())
		print_songs();
}

void main_menu(void)
{
  char *key;
  ps_verbose(1);
  while(1)
	{
	  printf("\nCanzoni in indice: %d, canzoni selezionate: %d",
		 indexlen(index),indexlen(selection));
#ifdef BORLAND
printf(" (memoria libera:%lu)",(unsigned long)coreleft());
#endif
printf("\n");
switch(menu(
		  "MENU' PRINCIPALE",
			"VWTALCSIHQOR",
			"Elenco delle canzoni in indice\n"
			"Elenco delle canzoni selezionate\n"
			"Elenco degli autori\n"
			"Aggiungi un nuovo FILE\n"
			"Carica un FILE come selezione\n"
			"Cerca canzoni\n"
			"Stampa canzoni\n"
		  "Stampa indice\n"
		  "Controlla la sintassi di tutte le canzoni\n"
		  "Mette l'indice in ordine alfabetico\n"
		  "Opzioni\n"
			"Fine\n"
			))
	{
	case 'T':outbiblio();break;
	case 'O':options_menu();break;
	case 27:
	case 'R':return;
	case 'S':
		stampa_menu();
		break;
	case 'I':
		stampa_index_menu();
		break;
	case 'A':
	  printf("Nome del file da richiamare [.sng]:");
		if (setfile(searchfile(getfilename("sng"))))
			only_view(readfile(&index));
		break;
	case 'L':
	  printf("Nome del file da caricare [.ind]:");
	  if (setfile(searchfile(getfilename("ind"))))
	    {
	    only_view(*end_of_list(&selection)=readfile(&index));
	    }
	  break;
	case 'V':
		selection=view_and_select(index,selection);
		break;
	case 'W':
		only_view(selection);
		break;
	case 'H':
		check_all(index);
		break;
	case 'C':
	  key=(char *)malloc(sizeof(char)*40);
	  if (key==NULL)
		error("Memoria insufficiente");
	  else
		{
		  struct song_list *list=NULL;
		  printf("Chiave di ricerca:");
		  fflush(stdin);
		  fgets(key,39,stdin);
		  *strchr(key,'\n')='\0';
		  if ((list=findsong(key,index))!=NULL)
		    selection=view_and_select(list,selection);
		  else
		    printf("\nNon ho trovato nessuna corrispondenza\n");
		  freelist(list);
		  free(key);
		}
	  break;
	  case 'Q':
	    printf("Ordino l'indice...\n");
	    index=sort_index(index);
	    printf("...fatto\n");
	  break;
	}
	}
}

#include "cmdline.h"
int main_interactive(void)
{
#ifdef BORLAND
if (mouse_mode)
	mouse_on();
#endif;
printf("\n==========SONG reader ========  by Manu=========\n");
if (indexlen(index)==0)
  {
  printf("Nome del file da richiamare:");
  if (setfile(searchfile(getfilename("sng"))))
    readfile(&index);
  }
#ifdef DOS
  else
   {
   selection=view_and_select(index,selection);
   }
#endif
	while (1)
		{
		main_menu();
		if (ask("Sei sicuro di voler finire?"))
			break;
		}
	printf("Ciao!\n");
	return 0;
	}


int main_extraactive(int argc,char *argv[])
{
  int interact=0;
  int i,n,m;
  char *s;
#ifdef SNG2PS
  language=3;
  output_mode=2;
  set_mode_chords(1);
  set_mode_alzo(LASCIA);
#endif
  new_option("files",VOID_OPT);
  new_option("search",WITH_VALUE_OPT+ABBREV_OPT);
  new_option("find",WITH_VALUE_OPT+ABBREV_OPT);
  new_option("language",WITH_VALUE_OPT+ABBREV_OPT);
  new_option("chords",WITH_VALUE_OPT+ABBREV_OPT);
  new_option("max",WITH_VALUE_OPT+ABBREV_OPT);
  new_option("href",WITH_VALUE_OPT+ABBREV_OPT);
  new_option("authors",ABBREV_OPT);
  new_option("all",ABBREV_OPT);
  new_option("transpose",ABBREV_OPT+WITH_VALUE_OPT);
  new_option("mode",ABBREV_OPT+WITH_VALUE_OPT);
  new_option("directory",ABBREV_OPT+WITH_VALUE_OPT);
  new_option("output",ABBREV_OPT+WITH_VALUE_OPT);
  new_option("pag",ABBREV_OPT+WITH_VALUE_OPT);
  new_option("config",WITH_VALUE_OPT);
  parse(argc,argv);
  #ifndef SNG2PS
  s=value_of_option("config",0);
  if (s) {
    config_file=strdup(s);
  }
  load_options();
  #endif
  s=value_of_option("directory",0);
  if (s)
    {
      if (s[0]=='=') s++;
      chdir(s);
    }
  n=number_of_values_of_option("files");
  for (i=0;i<n;i++)
    {
      if(setfile(searchfile(value_of_option("files",i))))
	*end_of_list(&index)=readfile(NULL);
    }
  if (value_of_option("max",0))
    n=atoi(value_of_option("max",0));
  if (n!=0) max_found=n;
  s=value_of_option("href",0);
  if (s && *s) HREF=s+1;
  s=value_of_option("language",0);
  if (s!=NULL)
    {
      if (s[0]=='=') s++;
      strlwr(s);
      if (!strcmp(s,"html")) language=1;
      else if (!strcmp(s,"xng")) language=10;
      else if (!strcmp(s,"sng")) language=4;
      else if (!strcmp(s,"out")) language=2;
      else if (!strcmp(s,"ps")) language=3;
      else
	{
	  error("Non capisco il parametro '-language'");
	  exit(1);
	}
    }
  s=value_of_option("chords",0);
  if (s!=NULL)
    {
      if (s[0]=='=') s++;
      strlwr(s);
      
      if (!strcmp(s,"off")||s[0]=='0')
	mode_chords=0;
      else if (!strcmp(s,"on")||s[0]=='1')
	mode_chords=1;
      else if (!strcmp(s,"ref")||s[0]=='3')
	mode_chords=3;
      else if (!strcmp(s,"copyoff")||s[0]=='4')
	mode_chords=4;
      else if (!strcmp(s,"copy")||s[0]=='5')
	mode_chords=5;
      else if (!strcmp(s,"copyref")||s[0]=='7')
	mode_chords=7;
    }
  s=value_of_option("transpose",0);
  if (s)
    {
      if (s[0]=='=') s++;
      alzo=atoi(s);
    }
  s=value_of_option("mode",0);
  if (s)
    {
      if (s[0]=='=') s++;
      mode_alzo=0;
      if (s[0]=='D') mode_alzo|=MAIUSCOLO;
      if (s[1]=='O') mode_alzo|=TUTTO_MAIUSCOLO;
      if (s[2]=='-') mode_alzo|=MINORE_MENO;
      if (s[3]=='(') mode_alzo|=BASSO_TRA_PARENTESI;
      if (s[4]=='M') mode_alzo|=BASSO_MAIUSCOLO;
      if (s[5]=='I') mode_alzo|=BASSO_TUTTO_MAIUSCOLO;
      if (s[0]=='A') mode_alzo=LASCIA;
    }
  s=value_of_option("output",0);
  if (s) {
    output_filename=strdup(s);
  }
  s=value_of_option("pag",0);
  if (s) {
    first_page_number=atoi(s);
  }
  if (is_on_option("authors"))
    {
      open_output();
      out_authors(output);
      close_output();
    }
  else
    {
      n=number_of_values_of_option("search");
      m=number_of_values_of_option("find");
      if (n+m>0)
	{
	  selection=NULL;
   	  for (i=0;i<n;i++)
  	    {
	      s=value_of_option("search",i);
	      strlwr(s);
	      if (s[0]=='=') s++;
	      *end_of_list(&selection)=findsong(s,index);
	    }
	  for (i=0;i<m;i++)
	    {
	      s=value_of_option("find",i);
	      if (s[0]=='=') s++;
	      *end_of_list(&selection)=getsong(s,index);
	    }
	  if (max_found==0 || indexlen(selection)<=max_found)
            print_songs();
        else print_index();
	}
      else {
#ifdef SNG2PS
	*end_of_list(&selection)=findsong("",index);
	print_songs();
	return 0;
#endif
	interact=1;
      }
    }
  n=number_of_values_of_option("files");
  if (n==0) interact=1;
  end_options();
  return interact;
}

int main(int argc,char *argv[])
{
  if (main_extraactive(argc,argv)) return main_interactive();
  return 0;
}


