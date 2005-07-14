#define GOSCRIPT_TEMP_FILE "delme.ps"
#define GOSCRIPT_EXECUTABLE "songgosc.bat"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "song.h"
#include "songout.h"
#include "songprin.h"

#define stdprn stdout
void fatal_error(char *s)
	{
	fprintf(stderr,"Errore irrimediabile: %s\n\n",s);
	exit(1);
	}

static FILE *output;/*file di output (vedi sotto)*/
static char *output_filename=0;
static int output_mode=1; /*0:console, 1:stdout, 2:file, 3:stampante, 4:goscript*/

static int language=5; /*0:CONSOLE, 1:HTML, 2:SNG, 3:PS, 4:SOURCE, 5:TESTO, 7:XML*/
static int first_page_number=0;
extern int mode_chords;
/*
	0: non mette accordi
	1: mette accordi
	3: mette riferimenti
	4: mette copie (ma senza accordi)
	5: mette copie con accordi
	7: mette copie e riferimenti con accordi
	*/
static char *HREF;


#include <ctype.h>

#include "chords.h"


static struct song_list *indexx=NULL,*selection=NULL;

extern int alzo,mode_alzo;

void open_output(void)	{
  char *ext;
  switch(output_mode) {
  case 1:
    output=stdout;
    break;
  case 2:
    if (output_filename)
      output=fopen(output_filename,"wb");
    else {
      fprintf(stderr,"Nome del file di output non specificato\n");
      exit(1);
    }
    if (output==NULL) {
      fprintf(stderr,"Non riesco ad aprire il file %s\n",output_filename);
      exit(1);
    }
    break;
  default:
    fprintf(stderr,"internal error #23846483\n");
    exit(1);
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

void print_songs(void) {
  struct song_list *p;
  if (selection==NULL) {
    fprintf(stderr,"Nessuna canzone selezionata!\n");
    exit(1);
  }
  open_output();
  if (output==NULL) return;
  if (language==2) {
    for (p=selection;p!=NULL;p=p->next)
      writesong(p->first,output);
  }
  else if (language==4)
    out_songs_src(selection,output);
  else if (language==1)
    out_songs_html(selection,output);
  else if (language==7)
    out_songs_xml(selection,output);
  else if (language==5)
    out_songs(selection,output);
  else if (language==6) { // list
    struct song_list *p;
    for (p=selection;p;p=p->next) {
      char *title;
      char *author;
      printf("%s: %s\n",p->first->head.l->s,
	     p->first->author?p->first->author->name->s:"");
    }
  }
  else if (language==3) {
    selection=ps_songs(selection,first_page_number,output);
  }
  close_output();
}

struct song_list *read_song_file(FILE *fp) {
  struct song_list *list=0;
  struct song_list **p;
  struct song *q;
  
  for (p=&list;(q=readsong(fp))!=0;p=&((*p)->next)) {
    *p=new_list(q);
  }
  return list;
}

struct song_list *read_song_filename(char *filename) {
  FILE *fp;
  struct song_list *list=0;
  fp=fopen(filename,"r");
  if (fp) list=read_song_file(fp);
  else {
    fprintf(stderr,"cannot open file %s\n",filename);
    abort();
  }
  fclose(fp);
  return list;
}

int main(int argc,char *argv[])
{
  int selected=0;
  int authors=0;
  int i;
  for (i=1;i<argc;++i) {
    if (argv[i][0]!='-') {
      fprintf(stderr,"loading file %s\n",argv[i]);
      /*
      if (setfile(searchfile(argv[i]))) {
	*end_of_list(&indexx)=readfile(NULL);
	fprintf(stderr,"loaded\n");
      } else {
	fprintf(stderr,"error!\n");
      }
      */
      *end_of_list(&indexx)=read_song_filename(argv[i]);
    } else if (!strcmp(argv[i],"-lang") && i+1<argc) {
      int j;
      ++i;
      for (j=1;lang_list[j] && strcmp(lang_list[j],argv[i]);++j);
      if (lang_list[j]) language=j;
      else {
	fprintf(stderr,"language %s sconosciuto.\n",argv[i]);
	exit(1);
      }
    } else if (!strcmp(argv[i],"-chords") && i+1<argc) {
      int j;
      ++i;
      for (j=0;chord_list[j] && !!strcmp(chord_list[j],argv[i]);++j);
      if (chord_list[j]) mode_chords=j;
      else {
	fprintf(stderr,"chord mode %s sconosciuto.\n",argv[i]);
	exit(1);
      }
    } else if (!strcmp(argv[i],"-transpose") && i+1<argc) {
      i++;
      alzo=atoi(argv[i]);
    } else if (!strcmp(argv[i],"-o") && i+1<argc) {
      output_filename=strdup(argv[++i]);
      output_mode=2;
    } else if (!strcmp(argv[i],"-pag") && i+1<argc) {
      first_page_number=atoi(argv[++i]);
    } else if (!strcmp(argv[i],"-version")) {
      printf("version: %s\n",VERSION);
      exit(0);
    } else if (!strcmp(argv[i],"-search") && i+1<argc) {
      char *s;
      s=strdup(argv[++i]);
      strlwr(s);
      *end_of_list(&selection)=findsong(s,indexx);
      selected=1;
    } else if (!strcmp(argv[i],"-authors")) {
      authors=1;
    } else {
      if (strcmp(argv[i],"-help"))
	fprintf(stderr,"opzione %s sconosciuta\n",argv[i]);
      printf("songcmd [options] files [options]\n"
	     "options:\n"
	     " -lang <html|xng|sng|src|txt|ps|list>\n"
	     " -chords <off|on|ref|copyoff|copy|copyref>\n"
	     " -transpose <n>\n"
	     " -o <filename>\n"
	     " -pag <n>\n"
	     " -search <key>\n"
	     " -authors\n"
	     " -version\n"
	     );

      exit(1);
    }
  }
  
  if (selected==0) 
    *end_of_list(&selection)=findsong("",indexx);
  {
    int n;
    struct song_list *p;
    for (n=0,p=selection;p;p=p->next,n++);
    fprintf(stderr,"%d song(s) selected\n",n);
  }
  if (authors) {
      open_output();
      out_authors(output);
      close_output();
  } else {
    print_songs();
  }
  return 0;
}


