#ifndef SONG_HEADER
#define SONG_HEADER
#include <stdio.h>
#include "chords.h"

#ifdef __TURBOC__
#define DOS
#define BORLAND
#endif

#ifdef DJGPP
#define DOS
#undef index
#define index Index
#endif

#ifndef DOS
#define UNIX
#undef index
#define index Index
#endif

#include "string.h"

extern char *two_voices;

#ifdef UNIX

char * strlwr(char *s);
char * strupr(char *s);

#endif


extern char *lang_list[];
extern char *lang_desc[];
extern char *chord_list[];
extern int chord_value[];
extern char *chord_desc[];
extern char *mode_list[];
extern int mode_value[];


/*settaggio variabili globali:*/


void set_alzo(int new);
void set_mode_alzo(int new);
void set_mode_chords(int new);
int get_alzo(void);
int get_mode_alzo(void);
int get_mode_chords(void);



void strsubst(char *s,char from,char to);


struct file
{
  char *name;
  char *descr;
  FILE *p;
/*  struct ftime;*/ /*ultima modifica del file*/
  struct file *next;
};

struct line
{
  char *s;
  char *ctrl;
};
struct phrase
{
  int level; /* 1:chord, 2:note, 3:strum, 4:part, 5:p e' un riferimento ad accordo */
  struct phrase *p;
  struct line *l;    /* solo uno tra p,l e` diverso da NULL */
  struct phrase *next;
};
struct verse
{
  struct phrase *list;
  struct verse *next;
};
struct part
{
  int type; /* rit, strophe, spoken, tabular */
  char refmode; /* 0:niente, 1:copy, 2:riferimento, 3:gia' copiato;*/
  struct part *ref;
  struct verse *list;
  struct part *next;
};

struct author
{
  struct line *name;
  struct author *next;
};

struct info
{
  char type; /* 't'=titolo, 'g':page, 'x'=keys... */
  struct line *l;
  struct info *next;
};

/*#define EMPTY_SONG 0xFFFFFFFFUL*/
struct song
{
  struct info head;
  struct author *author;
  struct part *main;
  unsigned long offset;
  struct file *file;
  unsigned char flag;
};
/*costanti per song.flag*/
#define SONG_INDEX 1 /*non c'era il corpo della canzone*/
#define SONG_NOOFFSET 2/*non si conosce l'offset della canzone*/

struct song_list
{
  struct song *first;
  struct song_list *next;
};

int setfile(struct file *);
struct file* searchfile(char *);
void closefile(void);

void freesong(struct song*);
void freeindex(struct song_list*); /*libera indice e canzoni*/
void freemain(struct song*);
void freelist(struct song_list*); /*libera solo la lista*/

int bibliolen(void);
char *surname(char *);
struct song_list* select_author(struct author*,struct song_list *index);
struct author* biblioauthor(int n);
struct song *get_song_of_author(struct author *);
int has_written(struct author *a,struct song *s);

struct song_list *sort_index(struct song_list *index);
void check_all(struct song_list *index);
struct song_list* readfile(struct song_list **index);
int indexlen(struct song_list *index);
struct song* indexsong(int,struct song_list *index);
struct song_list* indexsonglist(int,struct song_list *index);
struct song *readsong(int all);
int rereadsong(struct song*);
void readend(void);
void add_file(void);
struct song_list *selected(struct song *p,struct song_list *selection);
struct song_list* toggle_selection(struct song *p,struct song_list *selection);
struct song_list* add_to_selection(struct song *p,struct song_list *selection);
struct song_list* copy_list(struct song_list *index);
struct song_list* findsong(char *key,struct song_list *index);
struct song_list* getsong(char *key,struct song_list *index);
struct song_list** end_of_list(struct song_list **);
STRING song_key(struct song *p);
STRING author_key(struct author *p);

void writesource(struct song *p,FILE *out);
int songcmp(struct song *p, char *s);

extern struct author *biblio;
extern struct file *file_list;


extern void error(char *);
extern void fatal_error(char *);


int has_chord(struct part *p);
int phrase_has_chord(struct phrase *p);

int phrase_is_note(struct phrase *p);
int phrase_is_strum(struct phrase *p);

void song_chords(struct part* p);
struct part * fill_in_chords(struct part *p);

struct author* split_authors(struct line* author);

void writeindex(struct song_list *list,FILE *output);
void writesong(struct song *p,FILE *output);
void writeline(struct line *,FILE *output);

void newhead(struct song *p, char type, struct line *l);
char *head_name(char type);
struct line* heading_line(struct song* p,char type);

#endif
