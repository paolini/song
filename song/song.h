#ifndef SONG_HEADER
#define SONG_HEADER
#include <stdio.h>
#include "chords.h"

#include "string.h"

extern char *two_voices;

char * strlwr(char *s);
char * strupr(char *s);


extern char *lang_list[];
extern char *lang_desc[];
extern char *chord_list[];
extern int chord_value[];
extern char *chord_desc[];
extern char *mode_list[];
extern int mode_value[];


void strsubst(char *s,char from,char to);


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

void myreset(void);
struct song_list *new_list(struct song *p);

void freesong(struct song*);
void freeindexx(struct song_list*); /*libera indice e canzoni*/
void freelist(struct song_list*); /*libera solo la lista*/

int bibliolen(void);
char *surname(char *);
struct song_list* select_author(struct author*,struct song_list *indexx);
struct author* biblioauthor(int n);
struct song *get_song_of_author(struct author *);
int has_written(struct author *a,struct song *s);

struct song_list *sort_indexx(struct song_list *indexx);
int indexxlen(struct song_list *indexx);
struct song* indexxsong(int,struct song_list *indexx);
struct song_list* indexxsonglist(int,struct song_list *indexx);
struct song *readsong(FILE *fp);
void readend(void);
void add_file(void);
struct song_list *selected(struct song *p,struct song_list *selection);
struct song_list* toggle_selection(struct song *p,struct song_list *selection);
struct song_list* add_to_selection(struct song *p,struct song_list *selection);
struct song_list* copy_list(struct song_list *indexx);
struct song_list* findsong(char *key,struct song_list *indexx);
struct song_list* getsong(char *key,struct song_list *indexx);
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

void writeindexx(struct song_list *list,FILE *output);
void writesong(struct song *p,FILE *output);
void writeline(struct line *,FILE *output);

void newhead(struct song *p, char type, struct line *l);
char *head_name(char type);
struct line* heading_line(struct song* p,char type);

#endif
