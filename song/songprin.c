#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "song.h"
extern void error(char *);
static FILE *output;
static int interact;

void ps_verbose(int on)
{
  interact=on;
}

#define ps_out output

float col_width;

float verse_width;  /*se e' 1000 si sta cercando la larghezza*/
float verse_max_width; /*la riga + lunga */
float first_voice_max_width;
float second_voice_max_width;
float last_voice_change; /*posizione dell'ultimo &*/
float current_height;
float current_width;
float chords_width;


static int try; /*se 1 non stampa veramente*/

char* PS_INDEX_FILE="SONGPAGE.IND";

#define INTERCHORD 0.1
#define NORMAL_INTERLINEA 0.4
#define WITH_CHORD_INTERLINEA 0.6
#define CHORDS_RAISE 0.3
#define TITLE_INTERLINEA 0.6
#define AUTHOR_INTERLINEA 0.45
#define INTER_PART_INTERLINEA 0.3
#define INTER_COL_SPACE 0.6
#define INTER_VOICES_SPACE 0.3
#define RAPP (1.0/0.8)

float MIN_FILL_PAGE=0.5;
int FILL_LAST_PAGE=0;
int DO_LINES=0;
int PRINT_PAGE_NUMBERS=1;


#define MAX_SONGS_PER_PAGE 4

#define TOTAL_PAGE_HEIGHT 29.6
#define TOTAL_PAGE_WIDTH 21.0
#define LEFT_MARGIN 0.4
#define RIGHT_MARGIN 1.0
#define UPPER_MARGIN 1.1
#define LOWER_MARGIN 1.1
#define ASYMMETRIC_MARGIN 1.0
#define PAGE_HEIGHT (TOTAL_PAGE_HEIGHT-UPPER_MARGIN-LOWER_MARGIN)
#define PAGE_WIDTH (TOTAL_PAGE_WIDTH-LEFT_MARGIN-RIGHT_MARGIN-ASYMMETRIC_MARGIN)
#define NUMBER_WIDTH 1.5

int songprin_options_load(char *name,char *value)
	{
	if (!strcmp(name,"PS_MINIMUM_FILLING"))
		sscanf(value,"%f",&MIN_FILL_PAGE);
	else if (!strcmp(name,"PS_FILL_LAST_PAGE"))
		FILL_LAST_PAGE=atoi(value);
	else if (!strcmp(name,"PS_DRAW_LINES"))
		DO_LINES=atoi(value);
	else if (!strcmp(name,"PS_FIRST_PAGE_NUMBER"))
		PRINT_PAGE_NUMBERS=atoi(value);
	else if (!strcmp(name,"PS_INDEX_FILE"))
		PS_INDEX_FILE=strdup(value);
	else return 0;
	return 1;
	}

void songprin_options_save(FILE *fp)
	{
	fprintf(fp,"PS_MINIMUM_FILLING=%f\n",MIN_FILL_PAGE);
	fprintf(fp,"PS_FILL_LAST_PAGE=%d\n",FILL_LAST_PAGE);
	fprintf(fp,"PS_DRAW_LINES=%d\n",DO_LINES);
	fprintf(fp,"PS_FIRST_PAGE_NUMBER=%d\n",PRINT_PAGE_NUMBERS);
	fprintf(fp,"PS_INDEX_FILE=%s\n",PS_INDEX_FILE);
	}


int char_size[3][96]={
	{250,333,408,500,500,833,778,333,333,333,500,564,250,333,250,278,500,500,500,500,500,500,500,500,500,500,278,278,564,564,564,444,
	921,722,667,667,722,611,556,722,722,333,389,722,611,889,722,722,556,722,667,556,611,722,722,944,722,722,611,333,278,333,469,500,
	333,444,500,444,500,444,333,500,500,278,278,500,278,778,500,500,500,500,333,389,278,500,500,722,500,500,444,480,200,480,541,250},
	{250,333,420,500,500,833,778,333,333,333,500,675,250,333,250,278,500,500,500,500,500,500,500,500,500,500,333,333,675,675,675,500,
	920,611,611,667,722,611,611,722,722,333,444,667,556,833,667,722,611,722,611,500,556,722,611,833,611,556,556,389,278,389,422,500,
	333,500,500,444,500,444,278,500,500,278,278,444,278,722,500,500,500,500,389,389,278,500,444,667,444,444,389,400,275,400,541,250},
	{250,333,555,500,500,1000,833,333,333,333,500,570,250,333,250,278,500,500,500,500,500,500,500,500,500,500,333,333,570,570,570,500,
	930,722,667,722,722,667,611,778,778,689,500,778,667,944,722,778,611,778,722,556,667,722,722,1000,722,722,667,333,278,333,581,500,
	333,500,556,444,556,444,333,500,556,278,333,556,278,833,556,500,556,556,444,378,333,556,500,722,500,500,444,394,220,394,520,250}};
	/*larghezza in centesimi di punto dei caratteri da 32 a 127 ASCII font 10*/

/*valori a bit di level*/
#define IS_CHORD 1
#define IS_NOTE  2
#define IS_STRUM 4
#define IS_TITLE 8
#define IS_AUTHOR 16
int level;

int font_size[]=
	{10,8,10,8,10,8,10,8,
	20,0,0,0,0,0,0,0,
	12,0,0,0,0,0,0,0}; /*indicizzato da level*/
int current_font_type;/* Roman, Italic, Bold*/
int current_font_size;
int current_part; /*rit, strophe, spok*/
float ps_interlinea;

float mymax(float a,float b)
	{return a>b?a:b;}

void ps_set_font(void)
	{
	int f;
	if (current_part==0 && level==0) f=1;
	else if (level==IS_TITLE || level==IS_AUTHOR) f=2;
	else f=0;
	if (f!=current_font_type || current_font_size!=font_size[level])
		{
		current_font_type=f;
		current_font_size=font_size[level];
		if (!try)
			fprintf(ps_out," Font%d%d setfont",current_font_type,current_font_size);
		}
	}


static char tab1[]={'`','\'','\"','^','~',',','\0'};
static char tab2[]={"aeiouAEIOUnNcC"};
static char *tab3[]={"grave","acute","dieresis","circumflex","tilde","cedilla",""};
char *ps_octal(int n)
{
  static char s[4];
  s[2]=n%8+'0';
  n/=8;
  s[1]=n%8+'0';
  n/=8;
  s[0]=n%8+'0';
  s[3]='\0';
  return s;
}
int ps_code(char c,char ctrl)
{
  int i,j;
  for (i=0;tab2[i]!=c && tab2[i]!='\0';i++);
  if (tab2[i]=='\0') return c;
  for (j=0;tab1[j]!=ctrl && tab1[j]!='\0';j++);
  if (tab1[j]=='\0') return c;
  return 128+i+j*14;
}

float ps_char_width(char a,char b) {
  if (b=='&')
    return INTER_VOICES_SPACE;
  if (a>=32 && a<=126)
    return char_size[current_font_type][a-32]
      *((float)current_font_size/10.0)*(1.0/100.0/72.0*2.54);
  else
    return 5.0/30.0*current_font_size/10.0; /*30 caratteri in 5 cm col font 10*/
}

void ps_char_out(char c,char ctrl)
{
  switch(ctrl)
    {
    case ' ':
      switch(c)
	{
	case '(':
	case ')':
	  fprintf(ps_out,"\\%s",ps_octal(c));
	  break;
	default:
	  fprintf(ps_out,"%c",c);
	}
      break;
o    case '&':
      fprintf(ps_out," %f cm 0 rmoveto",INTER_VOICES_SPACE);
      break;
    default:
      fprintf(ps_out,"\\%s",ps_octal(ps_code(c,ctrl)));
      break;
    }
}
void ps_cr(void)
{
  if (last_voice_change>0)
    if(current_width-last_voice_change>second_voice_max_width)
      {
	second_voice_max_width=current_width-last_voice_change;
	last_voice_change=0;
      }
  if (current_width>verse_max_width && current_part!=2) verse_max_width=current_width;
  chords_width=current_width=INTER_COL_SPACE;
  if (!try) fprintf(ps_out," cr");
}
void ps_lf(void)
{
  current_height+=ps_interlinea;
  if (!try) fprintf(ps_out," lf\n");
}
void ps_set_interlinea(float newval)
{
  if (ps_interlinea!=newval)
    {
      ps_interlinea=newval;
      if (!try)
	fprintf(ps_out," /interlinea %f cm def",ps_interlinea);
    }
}
void ps_set_width(float width)
{
  if (!try && (verse_width!=width))
    fprintf(ps_out," /verse_width %f cm def",width);
  verse_width=width;
}

int bracked_try=0,bracked=0;
int ps_word_try(char *a,char *b)
{
  int i;
  float len=0;
  ps_set_font();
  if (bracked_try==0 && (level&IS_NOTE))
    {
      bracked_try=1;
      len+=ps_char_width('[',' ');
    }
  for (i=0;a[i] && a[i]!=' ';i++)
		{
		  len+=ps_char_width(a[i],b[i]);
		}
  if (bracked_try==1 && a[i]=='\0')
    {
      len+=ps_char_width(']',' ');
      bracked_try=0;
    }
  if (level==IS_CHORD)
    {
      if (len+chords_width<=verse_width)
	{
	  chords_width+=len;
	  if (a[i]==' ')
	    {
	      chords_width+=ps_char_width(' ',' ');
	      i++;
	    }
	  return i;
	}
      else return 0;
    }
  else
    {
      if (len+current_width<=verse_width)
	{
	  current_width+=len;
	  if (a[i]==' ')
	    {
	      current_width+=ps_char_width(' ',' ');
	      i++;
	    }
	  return i;
	}
      else
	return -i;
    }
}
void ps_word_out(char *a,char *b)
	{
	ps_set_font();
	fprintf(ps_out," (");
	if (bracked==0 && (level&IS_NOTE))
		{
		bracked=1;
		ps_char_out('[',' ');
		}
	for (;*a && *a!=' ';a++,b++)
		ps_char_out(*a,*b);
	if (*a==' ') ps_char_out(' ',' ');
	if (bracked==1 && *a=='\0')
		{
		ps_char_out(']',' ');
		bracked=0;
		}
	if (level==IS_CHORD)
		{
		fprintf(ps_out,") chord_out");
		}
	else
		fprintf(ps_out,") word_out");
	}
void ps_line(struct line *p)
	{
	int i,n;
	if (p->s==two_voices && p->ctrl==two_voices)
		{
		if (try)
			{
			if (current_width>first_voice_max_width)
				first_voice_max_width=current_width;
			}
		else
			{
			fprintf(ps_out," second_voice");
			return;
			}
		}
	for (i=0;p->s[i];)
		{
		n=ps_word_try(p->s+i,p->ctrl+i);
		if (n<=0)
			{
			ps_cr();
			ps_lf();
			n=ps_word_try(p->s+i,p->ctrl+i);
			if (n<0) n=-n;
			}
		if (!try)
			ps_word_out(p->s+i,p->ctrl+i);
		i+=n;
		}
	if (p->s==two_voices && p->ctrl==two_voices)
		{
		last_voice_change=current_width;
		}
	}
int ps_line_trunc(struct line *p)
	{
	int i,n;
	for (i=0;p->s[i];)
		{
		n=ps_word_try(p->s+i,p->ctrl+i);
		if (n<=0)
			return 0;
		if (!try)
			ps_word_out(p->s+i,p->ctrl+i);
		i+=n;
		}
	return 1;
	}
void ps_phrase(struct phrase *p)
	{
	for (;p;p=p->next)
		{
		switch(p->level)
			{
			case 1:
				level|=IS_CHORD;
				if (current_width>chords_width)
					chords_width=current_width;
				if (!try)
					fprintf(ps_out," check_chords");
				break;
			case 2:
				level|=IS_NOTE;
				break;
			case 3:
				level|=IS_STRUM;
				break;
			}
		if (p->p)
			ps_phrase(p->p);
		else
			ps_line(p->l);
		switch(p->level)
			{
			case 1:
				level&=~IS_CHORD;
				chords_width+=INTERCHORD;
				if (!try)
					fprintf(ps_out," interchord");
				break;
			case 2:
				level&=~IS_NOTE;
				break;
			case 3:
				level&=~IS_STRUM;
				break;
			}
		}
	}
void ps_verse(struct verse *p)
	{
	for (;p;p=p->next)
		{
		if (phrase_has_chord(p->list))
			ps_set_interlinea(WITH_CHORD_INTERLINEA);
		else
			ps_set_interlinea(NORMAL_INTERLINEA);
		ps_cr();
		ps_lf();
		ps_phrase(p->list);
		ps_cr();
		}
	}
#define MAX_COL 3
#define MAX_PARTS 30
struct ps_columns
	{
	float x,y; /*dimensioni or e vert*/
	float yy; /*dimensione vert se x viene stretto di RAPP*/
	int ch_col[MAX_COL];
	float width_col[MAX_COL];
	/*cambi di colonna Es: se ho tre colonne con 3,4,3 parti ch_col={3,7,10}*/
	};
struct ps_song
	{
	struct song *p;
	int ncol; /*numero di strutture colonna calcolate*/
	int last_chose;/*ultima scelta del numero colonne*/
	struct ps_columns col[MAX_COL]; /*col[n] e' la divisione in n-1 colonne (n<ncol)*/
	};
void ps_parts(struct part *p,int from,int to)
	{
	int i;
	int last_type,last_size,last_part,last_level;
	float last_interlinea;
	for (i=0;p && i<to;i++,p=p->next)
		{
		if (i>=from)
			{
			current_part=p->type;
			ps_set_interlinea(INTER_PART_INTERLINEA);
			ps_cr();
			ps_lf();
			if (!try)
				{
				/*cerca di allineare eventuali seconde voci (&)*/
				last_type=current_font_type;
				last_size=current_font_size;
				last_part=current_part;
				last_interlinea=ps_interlinea;
				last_level=level;
				first_voice_max_width=second_voice_max_width=last_voice_change=0.0;
				try=1;
				ps_verse(p->list);
				try=0;
				current_font_type=last_type;
				current_font_size=last_size;
				current_part=last_part;
				ps_interlinea=last_interlinea;
				level=last_level;
				if (first_voice_max_width>0.0 || second_voice_max_width>0)
					fprintf(ps_out,"%f cm %f cm voices_set",first_voice_max_width,second_voice_max_width);
				}
			ps_verse(p->list);
			}
		}
	}
void ps_part_try(struct part *p,struct ps_song *s)
	{
	int i,j,n;
	float len[MAX_PARTS];
	float tot;
	float done,x,y;
	float max_width;
	struct part *a;
	int col;
	for (i=0,a=p;a;a=a->next,i++);
		n=i;
	if (n>MAX_PARTS)
		error("too many parts in song (internal #9264)");
	/* trova larghezza massima delle parti*/
	try=1;
	current_height=0;
	verse_max_width=0;
	ps_set_width(1000);
	ps_parts(p,0,n);
	ps_set_width(max_width=verse_max_width);

	/* trova la lunghezza di ogni parte*/
	for (i=0;i<n;i++)
		{
		current_height=0;
		ps_parts(p,i,i+1);
		len[i]=current_height;
		}

	for (tot=0,i=0;i<n;i++)
		tot+=len[i];
	s->ncol=MAX_COL;

	for (col=0;col<s->ncol;col++)
		{
		/* fa divisione in col+1 colonne */
		i=0; /*parti usate*/
		done=0; /*lunghezza colonne precedenti*/
		for (j=0;j<col+1-1;j++)
			{
			/*trova n. di parti colonna j*/
			y=(tot-done)/(col+1-j);
			for (x=0;(x<y)&&(x+len[i]<y || y-x>len[i]/2);i++)
				x+=len[i];
			if (x==0)
				s->ncol=col; /*c'e' una colonna vuota... non va bene*/
			done+=x;
			s->col[col].ch_col[j]=i;
			}
		if (col>0 && s->col[col].ch_col[j-1]==n)
			s->ncol=col; /*l'ultima colonna era vuota*/
		s->col[col].ch_col[j]=n;
		/*ricalcola larghezza di ogni colonna*/
		for (j=0;j<col+1;j++)
			{
			verse_max_width=0;
			ps_set_width(1000);
			ps_parts(p,j==0?0:s->col[col].ch_col[j-1],s->col[col].ch_col[j]);
			s->col[col].width_col[j]=verse_max_width>0?verse_max_width:max_width;
			}
		/*trova lunghezza massima delle colonne e somma larghezze*/
		y=0;
		x=0;
		for (j=0;j<col+1;j++)
			{
			ps_set_width(s->col[col].width_col[j]);
			x+=verse_width;
			current_height=0;
			ps_parts(p,j==0?0:s->col[col].ch_col[j-1],s->col[col].ch_col[j]);
			if (current_height>y) y=current_height;
			}
		s->col[col].y=y;
		s->col[col].x=x;

		/*trova lunghezza massima delle colonne strette di RAPP*/
		y=0;
		for (j=0;j<col+1;j++)
			{
			ps_set_width(s->col[col].width_col[j]/RAPP);
			current_height=0;
			ps_parts(p,j==0?0:s->col[col].ch_col[j-1],s->col[col].ch_col[j]);
			if (current_height>y) y=current_height;
			}
		s->col[col].yy=y;
		}
	}
void ps_intestazione(struct song *p)
	{
	struct author *a;
	struct line v={", ","  "};
	if (p->head.l)
		{
		ps_set_interlinea(TITLE_INTERLINEA);
		level=IS_TITLE;
		ps_cr();
		ps_lf();
		ps_line(p->head.l);
		level=0;
		}
	if (p->author)
		{
		ps_set_interlinea(AUTHOR_INTERLINEA);
		level=IS_AUTHOR;
		ps_cr();
		ps_lf();
		ps_line(p->author->name);
		for (a=p->author->next;a!=NULL;a=a->next)
			{
			ps_line(&v);
			ps_line(a->name);
			}
		level=0;
		}
	}
void ps_song_try(struct song *p,struct ps_song *s)
	{
	int i;
	try=1;
	ps_part_try(p->main,s);

	/* aggiunge lo spazio per i titoli */
	for (i=0;i<s->ncol;i++)
		{
		current_height=0;
		ps_set_width(s->col[i].x);
		verse_max_width=0;
		ps_intestazione(p);
		s->col[i].y+=current_height;

		/*titolo stretto di RAPP*/
		current_height=0;
		ps_set_width(s->col[i].x/RAPP);
		verse_max_width=0;
		ps_intestazione(p);
		s->col[i].yy+=current_height;

		}
	s->p=p;
	}
void ps_song_out(struct ps_song *p,float posx,float posy,float width,float height)
	{
	int i;
	float rap;
	struct ps_columns *c;
	c=&(p->col[p->last_chose]);
	fprintf(ps_out,"\n\n%% PRINTING NEW SONG: %s\n",p->p->head.l->s);
	fprintf(ps_out," %f cm %f cm position",posx,posy);
	try=0;
	ps_set_width(width);
	current_height=0;
	ps_intestazione(p->p);
	fprintf(ps_out," savecolumns");/*salva y assoluta*/
	rap=width/c->x;
	for (i=0;i<=p->last_chose;i++)
		{
		ps_set_width(rap*c->width_col[i]);
		ps_parts(p->p->main,i==0?0:c->ch_col[i-1],c->ch_col[i]);
		fprintf(ps_out," cr %f cm skipcolumn",rap*c->width_col[i]);
		}
	}
int n_ps;/*numero di canzoni calcolate*/
struct ps_song ps[MAX_SONGS_PER_PAGE];/*canzoni calcolate*/

char *layouts[]={
	"|-ab-cd","-|ab|cd",
	"-|abc","-a|bc","|a-bc","|-abc","--abc",
	"|ab","-ab",
	"a",
	NULL}; /*dal migliore al peggiore*/

/*layout Š del tipo: "||ab-cd"
	per rappresentare:       | c
											 a|b | -
													 | d
*/
char *next_layout(char *layout)
	{
	if (*layout=='-' || *layout=='|')
		return next_layout(next_layout(layout+1));
	return layout+1;
	}
int layout_over; /*il layout e' impossibile da far stare*/
/* valori negativi di width o height indicano che e' approssimata*/
/*torna l'area occupata*/
float chose_columns(struct ps_song *p,float width,float height,float *x,float *y)
	{
	float l;
	int i;
	l=0.0;
	if (height>0) /*scelgo la piu lunga che ci sta*/
		{
		for (i=0;i<p->ncol;i++)
			if (p->col[i].y<height && p->col[i].y>l)
				{p->last_chose=i;l=p->col[i].y;}
		}
	else if (width>0)
		{
		for (i=0;i<p->ncol;i++)
			if (p->col[i].x<width && p->col[i].x>l)
				{p->last_chose=i;l=p->col[i].x;}
		}
	else /*sceglie il miglior rapporto */
		{
		l=1000.0;
		for (i=0;i<p->ncol;i++)
			if (fabs(p->col[i].x/p->col[i].y-width/height)<l)
				{p->last_chose=i;l=fabs(p->col[i].x/p->col[i].y-width/height);}
		}
	*x=p->col[p->last_chose].x;
	if (width>0 && *x>width && *x/RAPP<=width)
		*y=p->col[p->last_chose].yy;
	else
		*y=p->col[p->last_chose].y;
	return *x * *y;
	}
/*torna l'area occupata*/
float ps_layout_try(char *layout,float width,float height,float *x,float *y)
	{
	float f,g,l,a;
	if (layout_over) return 0;
	if (*layout=='|')
		{
		l=width/2;
		if (l>0) l=-l;
		a=ps_layout_try(layout+1,l,height,&f,&g);
		a+=ps_layout_try(next_layout(layout+1),width>0?width-f:width+f,height,x,y);
		*x+=f;
		*y=mymax(*y,g);
		}
	else if (*layout=='-')
		{
		l=height/2;
		if (l>0) l=-l;
		a=ps_layout_try(layout+1,width,l,&f,&g);
		a+=ps_layout_try(next_layout(layout+1),width,height>0?height-f:height+f,x,y);
		*x=mymax(*x,f);
		*y+=g;
		}
	else if (*layout-'a'<n_ps)
		{
		a=chose_columns(&(ps[*layout-'a']),width,height,x,y);
		}
	else
		layout_over=1;
	if ((width>0 && *x>width) || (height>0 && *y>height)) layout_over=1;
	return a;
	}
void ps_layout_dim(char *layout,float *x,float *y)
	{
	float f,g;
	if (*layout=='|' || *layout =='-')
		{
		ps_layout_dim(layout+1,&f,&g);
		ps_layout_dim(next_layout(layout+1),x,y);
		if (*layout=='|')
			{
			*x+=f;
			*y=mymax(*y,g);
			}
		else
			{
			*x=mymax(*x,f);
			*y+=g;
			}
		}
	else if (*layout-'a' < n_ps)
		{
		*x=ps[*layout-'a'].col[ps[*layout-'a'].last_chose].x;
		*y=ps[*layout-'a'].col[ps[*layout-'a'].last_chose].y;
		}
	else
		error("internal error #9123");
	}
void ps_draw_line(float x1,float y1,float x2,float y2)
	{
	/*disegna linea*/
	if (DO_LINES)
		fprintf(ps_out," %f cm %f cm %f cm %f cm line",x1,y1,x2,y2);
	}
void ps_layout_out(char *layout,float posx,float posy,float width,float height)
	{
	float x1,y1,x2,y2,rap;
	if (*layout=='|' || *layout=='-')
		{
		ps_layout_dim(layout+1,&x1,&y1);
		ps_layout_dim(next_layout(layout+1),&x2,&y2);
		if (*layout=='|')
			{
			rap=width/(x1+x2);
			ps_layout_out(layout+1,posx,posy,x1*rap,height);
			ps_draw_line(posx+x1*rap+INTER_COL_SPACE/2,posy+INTER_COL_SPACE/2,posx+x1*rap+INTER_COL_SPACE/2,posy+height-INTER_COL_SPACE/2);
			ps_layout_out(next_layout(layout+1),posx+x1*rap,posy,x2*rap,height);
			}
		else
			{
			rap=height/(y1+y2);
			ps_layout_out(layout+1,posx,posy,width,y1*rap);
			ps_draw_line(posx+INTER_COL_SPACE,posy+y1*rap,posx+width,posy+y1*rap);
			ps_layout_out(next_layout(layout+1),posx,posy+y1*rap,width,y2*rap);
			}
		}
	else if (*layout-'a'<n_ps)
		{
		ps_song_out(&(ps[*layout-'a']),posx,posy,width,height);
		}
	else
		error("internal error #4376");
	}

void ps_save_index(char *layout,int page)
	{
	FILE *fp;
	int i,n;
	struct line *l;
	char line[10];
	if (!PRINT_PAGE_NUMBERS) return;
	fp=fopen(PS_INDEX_FILE,"at");
	if (fp!=NULL)
		{
		n=(strlen(layout)+1)/2;
		for (i=0;i<n;i++)
			{
			sprintf(line,"%d",page);
			if ((l=malloc(sizeof(struct line)))!=NULL
			   && (l->s=malloc((strlen(line)+1)*sizeof(char)))!=NULL)
			  {
			  strcpy(l->s,line);
			  l->ctrl=NULL;
			  newhead(ps[i].p,'g',l);
			  writeindexentry(ps[i].p,fp);
			  }
			}
		fclose (fp);
		}
	}
void ps_layout_page(char *layout,int page)
	{
          static int printed_page=0;
		try=0;
		ps_interlinea=-1;
		current_font_type=-1;
		current_font_size=-1;
		current_part=-1;
		verse_width=-1;
		if (interact)
			printf("...printing page %d\n",page);
		fprintf(ps_out,"\n%% PAGINA %d, layout %s \n\n",page,layout);
                fprintf(ps_out,"\n%%%%Page: %d %d\n\n",++printed_page,page);
		if (page%2)
			fprintf(ps_out," %f cm 0 translate",ASYMMETRIC_MARGIN);
		ps_save_index(layout,page);
		ps_layout_out(layout,0,0,PAGE_WIDTH,PAGE_HEIGHT);
		if (PRINT_PAGE_NUMBERS)
			{
			level=IS_TITLE;
			ps_set_font();
			fprintf(ps_out," %f cm %f cm position (%d)",page%2==0?INTER_COL_SPACE:PAGE_WIDTH,PAGE_HEIGHT,page);
			if (page%2==0)
				fprintf(ps_out," show");
			else fprintf(ps_out," wohs");
			level=0;
			}
		fprintf(ps_out," showpage\n");
	}

/* stampa una pagina, ritorna quante canzoni ha stampato (<=n_ps)*/
/* usa: n_ps, ps[]*/
char *chose_layout(float *area)
	{
	float x,y;
	int i;
	try=1;
	/*scegli layout*/
	layout_over=1;
	for (i=0;layouts[i]!=NULL && layout_over;i++)
		{
		layout_over=0;
		*area=ps_layout_try(layouts[i],PAGE_WIDTH,PAGE_HEIGHT,&x,&y);
		}
	return layouts[i-1];
	}
void ps_font(int type,int size)
	{
	fprintf(ps_out,"/Font%d%d /T%d findfont %d scalefont def\n",type,size,type,size);
	}
void ps_preamble(void)
{
  int i,j;
  
  fprintf(ps_out,
	  "%%!PS-Adobe-2.0\n"
	  "%%%%Creator: song, manu-fatto\n"
	  "%%%%Title: song\n"
	  "%%%%BoundingBox: 0 0 596 842\n"
	  "%%%%DocumentPaperSizes: A4\n"
	  "%%%%EndComments\n"
	  "\n\n%%   PREAMBLE\n\n"
	  "/cm {72 2.54 div mul} def\n"
	  "/PICT {0.5 setgray currentpoint currentpoint newpath moveto "
	  "-0.5 cm -0.6 cm rmoveto 2 cm 0 rlineto 0.5 cm 1.3 cm rlineto "
	  "-2 cm 0 rlineto -0.5 cm -1.3 cm rlineto 0.5 cm  0.6 cm rmoveto "
	  "closepath fill 0 setgray moveto} def\n"
	  );
  /*LM e' il margine sinistro*/
  

	fprintf(ps_out,"/cr {LM %f cm add currentpoint exch pop moveto "
	"/chords_width currentpoint pop def} def\n",INTER_COL_SPACE);
	fprintf(ps_out,"/lf {interlinea 0 exch sub 0 exch rmoveto} def\n");
	fprintf(ps_out,"/word_out {show} def\n");
	fprintf(ps_out,"/spc {( ) show} def\n");
	fprintf(ps_out,
	"/chord_out {currentpoint 3 -1 roll "
	"chords_width currentpoint exch pop %f cm add moveto "
	"show /chords_width currentpoint pop def moveto} def\n",CHORDS_RAISE);
	fprintf(ps_out,"/check_chords {currentpoint pop chords_width gt {/chords_width currentpoint pop def} if} def\n");
	fprintf(ps_out,"/interchord {/chords_width chords_width %f cm add def} def\n",INTERCHORD);
	fprintf(ps_out,"/coord {%f cm exch sub exch %f cm add exch} def\n",TOTAL_PAGE_HEIGHT-UPPER_MARGIN,LEFT_MARGIN);
	fprintf(ps_out,"/position {coord exch dup /LM exch def exch moveto} def\n");
	fprintf(ps_out,"/line {currentpoint 6 2 roll newpath coord moveto coord lineto stroke moveto} def\n");
	fprintf(ps_out,"/savecolumns {/savey currentpoint exch pop def} def\n");
	fprintf(ps_out,"/skipcolumn {currentpoint pop add dup /LM exch def savey moveto} def\n");
	fprintf(ps_out,"/voices_set {/second exch def /first exch def} def\n");
	fprintf(ps_out,"/second_voice {currentpoint pop LM sub first le"
		" verse_width currentpoint pop LM sub %f cm add sub second ge and "
		"{LM first add %f cm add currentpoint exch pop moveto} {%f cm 0 rmoveto} ifelse} def\n",INTER_VOICES_SPACE,INTER_VOICES_SPACE,INTER_VOICES_SPACE);
	fprintf(ps_out,"/wohs {dup stringwidth pop 0 exch sub 0 rmoveto show} def\n");

	fprintf(ps_out,
		"/reencsmalldict 14 dict def\n"
		"/ReEncodeSmall\n"
		"{reencsmalldict begin\n"
		"/newcodesandnames exch def\n"
		"/newfontname exch def\n"
		"/basefontname exch def\n"
		"/basefontdict basefontname findfont def\n"
		"/newfont basefontdict maxlength dict def\n"
		"basefontdict\n"
		"{exch dup /FID ne {dup /Encoding eq\n"
		"{exch dup length array copy\n"
		"newfont 3 1 roll put }\n"
		"{exch newfont 3 1 roll put}\n"
		"ifelse\n"
		"}{pop pop }\n"
		"ifelse }forall\n"
		"newfont /FontName newfontname put\n"
		"newcodesandnames aload pop\n"
		"newcodesandnames length 2 idiv\n"
		"{newfont /Encoding get 3 1 roll put}\n"
		"repeat\n"
		"newfontname newfont definefont pop\n"
		"end\n"
		"}def\n"
		"/scandvec[\n");
	for (i=0;tab2[i]!='\0';i++)
		for (j=0;tab1[j]!='\0';j++)
			fprintf(ps_out,"8#%s /%c%s\n",ps_octal(128+i+j*14),tab2[i],tab3[j]);
		fprintf(ps_out,"] def\n");
	fprintf(ps_out,
		"/Times-Roman /T0 scandvec ReEncodeSmall\n"
		"/Times-Italic /T1 scandvec ReEncodeSmall\n"
		"/Times-Bold /T2 scandvec ReEncodeSmall\n");
	for (i=0;i<8;i++)
		{
		for (j=0;j<i && font_size[i]!=font_size[j];j++);
		if (j==i)
			{
			ps_font(0,font_size[i]);
			ps_font(1,font_size[i]);
			}
		}
	ps_font(2,font_size[IS_TITLE]);
	ps_font(2,font_size[IS_AUTHOR]);
	}

int roll_songs(struct song_list **rest,int n,int n_ps,int roll)
/*
Considero l'elenco di canzoni formato dalle n_ps in ps[...] e dalla lista
*rest. In questo elenco devo scambiare la n+1-esima canzone (quella con
indice n) con la n+2+roll-esima canzone (con indice n+1+roll).
*/
{
  struct ps_song s;
  struct song *p;
  struct song_list *list;
  int i;
  if (n+roll+1<n_ps)
    {
      s=ps[n];
      ps[n]=ps[n+roll+1];
      ps[n+roll+1]=s;
    }
  else
    {
      for (i=n_ps,list=*rest;list!=NULL && i<roll+1+n;i++,list=(list)->next);
      if (list==NULL) return 0;
      p=list->first;
      list->first=ps[n].p;
      rereadsong(p);
      ps_song_try(p,&(ps[n]));
    }
  return 1;
}

struct song_list *get_index(char *layout)
{
  struct song_list *l;
  int i,n;
  l=NULL;
  n=(strlen(layout)+1)/2;
  for (i=0;i<n;i++)
    {
      l=add_to_selection(ps[i].p,l);
    }
  return l;
}
struct song_list* ps_songs(struct song_list *selection,int page,FILE *fp)
{
/* rest= canzoni non ancora lette*/
/* selection= canzoni stampate*/
/* n_ps= numero canzoni lette ma non ancora stampate*/
/* n= numero di canzoni da stampare nella pagina */
/* lastn= valore precedente di n*/
/* layout= stringa contenente la disposizione delle canzoni nella pagina*/
/* roll= numero di canzoni che ho provato a sostituire con la prima canzone
	 che non sta nella pagina */

  struct song_list *p,*rest,*q;
  int i,n,roll=0,lastn=1000;
  char *layout;
  float a,v;
  output=fp;
  if (page==0) page=PRINT_PAGE_NUMBERS;
  PRINT_PAGE_NUMBERS=page>0;
  if (page<=0) page=1;
  ps_preamble();
  n_ps=0;  /*numero di canzoni caricate in memoria (ps[...])*/
  rest=selection;
  selection=NULL;
  for (;rest!=NULL || n_ps>0;)
    {
      while (n_ps<MAX_SONGS_PER_PAGE && rest!=NULL)
	{
	  rereadsong(rest->first);
	  ps_song_try(rest->first,&ps[n_ps]);
	  n_ps++;
	  p=rest->next;
	  rest->next=NULL;
	  freelist(rest);
	  rest=p;
	}

      layout=chose_layout(&a);
      n=(strlen(layout)+1)/2;
      if ((v=a/(PAGE_WIDTH*PAGE_HEIGHT))<MIN_FILL_PAGE && n<MAX_SONGS_PER_PAGE)
	{
	if (n>lastn) roll=0;
	lastn=n;
	if (roll_songs(&rest,n,n_ps,roll))
	  {
	  roll++;
	  n=0;
	  }
	else
	  {
	  if (n==n_ps && rest==NULL) /*era l'ultima pagina*/
	    {
	    if (interact)
	      printf("Non riesco a riempire l'ultima pagina...");
	    if (FILL_LAST_PAGE)
	      {
	      if (interact)
		{
		printf(" non stampo le ultime canzoni:\n");
		for (i=0;i<n_ps;i++)
		  printf("%s\n",ps[i].p->head.l->s);
		}
	      return selection;
	      }
	    else
	      {
	      if (interact)		printf(" stampo comunque\n");
	      }
	    }
	  else
	    {
	    if (interact)	      printf("Non sono proprio riuscito a riempire la pagina\n");
	    }
	  }
	}
      if (n>0) /*devo stampare qualcosa!*/
	{
	  if (interact)
	    printf("rolled %d (%s:%d%%): ",roll,layout,(int)(v*100));
	  ps_layout_page(layout,page++);
	  p=get_index(layout);
	  if (interact)
	    {
	    for (q=p;q!=NULL;q=q->next)
	      printf("%s; ",q->first->head.l->s);
	    printf("\n");
	    }
	  *end_of_list(&selection)=p;
	  for (i=0;i<n;i++)
	    freemain(ps[i].p);
	  for (i=0;i<n_ps-n;i++)
	    ps[i]=ps[i+n];
	  n_ps-=n;
	  roll=0;
	  lastn=1000;
	}
    }
  return selection;
}
void ps_title_author(struct song *p)
	{
	struct author *a;
	struct line v;
	v.ctrl="    ";
	ps_line_trunc(p->head.l);
	if (p->author)
		{
		v.s=" (";
		ps_line_trunc(&v);
		v.s=", ";
		ps_line_trunc(p->author->name);
		for (a=p->author->next;a!=NULL;a=a->next)
			{
			ps_line_trunc(&v);
			ps_line_trunc(a->name);
			}
		v.s=")";
		ps_line_trunc(&v);
		}
	}
void ps_authors_index(FILE *fp,struct song_list *index)
{
  int n;
  char* empty="             ";
  char  p[10];
  struct author *a;
  struct song_list *list,*s;
  struct line l;
  float w,y;
  
  output=fp;
  ps_preamble();
  fprintf(ps_out," 0 %f cm position",ASYMMETRIC_MARGIN);
  current_height=0;
  current_part=1;
  verse_width=PAGE_WIDTH-2*ASYMMETRIC_MARGIN;
  level=IS_TITLE;
  try=0;
  ps_set_interlinea(TITLE_INTERLINEA);
  ps_cr();ps_lf();
  l.ctrl=empty;
  l.s="Indice autori";
  ps_line_trunc(&l);
  level=0;
  ps_set_interlinea(NORMAL_INTERLINEA);
  verse_width=(verse_width-INTER_COL_SPACE)/2;
  fprintf(ps_out," savecolumns");
  y=current_height;
  n=0;
  for (a=biblio;a!=NULL;a=a->next)
    {
      list=select_author(a,index);
      for (s=list;s!=NULL;s=s->next)
	{
	  if (current_height+ps_interlinea>PAGE_HEIGHT)
	    {
	      if (n==0)
		{
		  fprintf(ps_out," cr %f cm skipcolumn",verse_width+INTER_COL_SPACE);
		  current_height=y;
		  n=1;
		}
	      else
		{
		  fprintf(ps_out," showpage");
		  fprintf(ps_out," 0 0 position");
		  current_height=0;
		  fprintf(ps_out," savecolumns");
		  y=current_height;
		  n=0;
		}
	    }
	  ps_cr();
	  ps_lf();
	  if (heading_line(s->first,'g'))
	    {
	      w=verse_width;
	      verse_width=w-NUMBER_WIDTH;
	      ps_title_author(s->first);
	      verse_width=w;
	      sprintf(p," PAG. %s",heading_line(s->first,'g')->s);
	      l.s=p;
	      ps_line_trunc(&l);
	    }
	  else
	    ps_title_author(s->first);
	}
      freelist(list);
    }
  fprintf(ps_out," showpage");
}
void ps_index(FILE *fp,struct song_list *s)
	{
	int n;
	char* empty="             ";
	char  p[10];
	struct line l;
	float w,y;
	
	output=fp;
	ps_preamble();
	fprintf(ps_out," 0 %f cm position",ASYMMETRIC_MARGIN);
	current_height=0;
	current_part=1;
	verse_width=PAGE_WIDTH-2*ASYMMETRIC_MARGIN;
	level=IS_TITLE;
	try=0;
	ps_set_interlinea(TITLE_INTERLINEA);
	ps_cr();ps_lf();
	l.ctrl=empty;
	l.s="Indice";
	ps_line_trunc(&l);
	level=0;
	ps_set_interlinea(NORMAL_INTERLINEA);
	verse_width=(verse_width-INTER_COL_SPACE)/2;
	fprintf(ps_out," savecolumns");
	y=current_height;
	n=0;
	for (;s!=NULL;s=s->next)
		{
		  if (current_height+ps_interlinea>PAGE_HEIGHT)
		    {
		      if (n==0)
			{
			  fprintf(ps_out," cr %f cm skipcolumn",verse_width+INTER_COL_SPACE);
			  current_height=y;
			  n=1;
			}
		      else
			{
			  fprintf(ps_out," showpage");
			  fprintf(ps_out," 0 0 position");
			  current_height=0;
			  fprintf(ps_out," savecolumns");
			  y=current_height;
			  n=0;
			}
		    }
		  ps_cr();
		  ps_lf();
		  if (heading_line(s->first,'g'))
		    {
		      w=verse_width;
		      verse_width=w-NUMBER_WIDTH;
		      ps_title_author(s->first);
		      verse_width=w;
		      sprintf(p," PAG. %s",heading_line(s->first,'g')->s);
		      l.s=p;
		      ps_line_trunc(&l);
		    }
		  else
		    ps_title_author(s->first);
		}
	fprintf(ps_out," showpage");
	}
