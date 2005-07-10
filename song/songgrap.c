#include <stdio.h>
#include "song.h"
#include "songout.h"
#ifdef BORLAND
#include <conio.h>
#include <graphics.h>
#include <string.h>

extern int NORMAL_COLOR;

extern int TITLE_COLOR;
extern int AUTHOR_COLOR;
extern int STROPHE_COLOR;
extern int REFRAIN_COLOR;
extern int SPOKEN_COLOR;
extern int CHORDS_COLOR;
extern int REFERENCE_COLOR;


struct mode
	{
	int style; /*tipo di carattere*/
	int charsize; /*grandezza font*/
	int width,height; /*dimensioni del carattere normale in sedicesimi*/
	float chords,title,author; /*dimensioni relative al carattere normale*/
	int normal_interlinea;
	};
int with_chords_interlinea;
#define MAX_MODES 10
static int nmodes=5;
static struct mode modes[MAX_MODES]={
	{SMALL_FONT,1,16,16,0.8,2,1.5,10},
	{SMALL_FONT,2,32,32,0.8,2,1.5,20},
	{SANS_SERIF_FONT,1,16,16,0.8,2,1.5,30},
	{TRIPLEX_FONT,1,16,16,0.8,2,1.5,30},
	{SANS_SERIF_FONT,2,32,32,0.8,2,1.5,60},
	};
struct mode *M;
static int current_text_mode;
static int color_stack[10];
static int color_stack_pointer=0;

#define INTER_VOICES_SPACE 15
#define INTER_CHORDS_SPACE 5
#define INTER_PARTS_SPACE 10
#define INDENT_CONTINUED_LINE 50
#define ALZA_ACCORDI 0.5

int maxx; /*larghezza schermo*/
#define MAXPARTS	30
static int height[MAXPARTS];
static int titles_height;
static int current_interlinea;
static int maxx,maxy;
static int out_of_screen;
static int next=0,start=0;

static int total_length=0;
static int chord_last_x,chord_last_y; /*dov'era il testo*/
static int chord_x;
static int is_strum=0,is_ref=0;
static int mode_chords=7;
static int alzo=0;

extern int strint(char *s);

void songcon_options_save(FILE *fp)
	{
	int i;
	fprintf(fp,"N_GRAPHIC_MODES=%d\n",nmodes);
	for (i=0;i<nmodes;i++)
		{
		fprintf(fp,"GRAPHIC_MODE[%d]={%d,%d,%d,%d,%f,%f,%f,%d}\n",i,
				(modes[i].style),(modes[i].charsize),
				(modes[i].width),(modes[i].height),
				(modes[i].chords),(modes[i].title),(modes[i].author),
				(modes[i].normal_interlinea));
		}
	fprintf(fp,"CURRENT_GRAPHIC_MODE=%d\n",current_text_mode);
	}
int songcon_options_load(char *name,char *value)
	{
	int i;
	if (!strcmp(name,"N_GRAPHIC_MODES"))
		{
		nmodes=strint(value);
		if (nmodes>MAX_MODES)
			nmodes=MAX_MODES;
		}
	else if (!strcmp(name,"CURRENT_GRAPHIC_MODE"))
		current_text_mode=strint(value);
	else if (!strncmp(name,"GRAPHIC_MODE[",13))
		{
		i=strint(name+13);
		if (i<0 || i>=nmodes)
			error("Graphic mode out of range");
		else
			{
			sscanf(value,"{%d,%d,%d,%d,%f,%f,%f,%d}",
				&(modes[i].style),&(modes[i].charsize),
				&(modes[i].width),&(modes[i].height),
				&(modes[i].chords),&(modes[i].title),&(modes[i].author),
				&(modes[i].normal_interlinea));
			}
		}
	else
		return 0;
	return 1;
	}

void con_set_mode(int mode)
	{
	if (mode!=-1)
		{
		if (mode==current_text_mode)
			return;
		else
			current_text_mode=mode;
		}
	M=&(modes[current_text_mode]);
	settextstyle(M->style,HORIZ_DIR,M->charsize);
/*	M->normal_interlinea=1.8*textheight("W");  <---non funziona */
	with_chords_interlinea=((M->chords+1.0))*M->normal_interlinea;
	}

int open_graphics(void)
	{
	static gia_aperto=0;
	int gdriver = DETECT, gmode, errorcode;
	if (gia_aperto)
		{
		setgraphmode(getgraphmode());
		}
	else
		{
		initgraph(&gdriver, &gmode, "\\tc\\bgi");
		errorcode = graphresult();
		if (errorcode==grFileNotFound)
			{
			initgraph(&gdriver,&gmode,".");
			errorcode=graphresult();
			}
		if (errorcode!=grOk)
			{
			error(grapherrormsg(errorcode));
			return 0;
			}
		}
	con_set_mode(-1);
	color_stack_pointer=0;
	maxx=getmaxx();
	maxy=getmaxy();
  gia_aperto=1;
	return 1;
	}
void close_graphics(void)
	{
	restorecrtmode();
	}

void put_color(int color)
	{
	setcolor(color_stack[color_stack_pointer++]=color);
	}
void pop_color(void)
	{
	setcolor(color_stack[--color_stack_pointer-1]);
	}
void con_out_string(char *s)
	{
	int l,x,y;
	l=textwidth(s);
	x=getx();
	if (x+l<maxx)
		outtext(s);
	else
		{
		moveto(INDENT_CONTINUED_LINE,y=(gety()+current_interlinea));
		chord_x=0;
		if (y>maxy && total_length>0)
			{
			out_of_screen=1;
			return;
			}
		outtext(s);
		}
	}
void con_out_line(struct line *p)
	{
	char *s,*ctrl,*buf;
	int n,l;
	if (p->ctrl[0]=='&')
		{
		if (getx()<maxx/2)
			moveto(maxx/2,gety());
		else
			moverel(INTER_VOICES_SPACE,0);
		}
	else
		{
		s=p->s;ctrl=p->ctrl;
		while (s[0]!='\0' && !out_of_screen)
			{
			buf=word_to_string(s,ctrl,&n,&l);
			s+=n;ctrl+=n;
			con_out_string(buf);
			if (*s==' ')
				{
				outtext(" ");
				s++;ctrl++;
				}
			}
		}
	}
void begin_chord(void)
	{
	if (is_ref)
		put_color(REFERENCE_COLOR);
	else
		put_color(CHORDS_COLOR);
	chord_last_x=getx();
	chord_last_y=gety();
	setusercharsize(M->chords*M->width,16,M->chords*M->height,16);
	if (!is_strum)
		{
		if (chord_last_x<chord_x+INTER_CHORDS_SPACE)
			moveto(chord_x+INTER_CHORDS_SPACE,chord_last_y-M->normal_interlinea*ALZA_ACCORDI);
		else
			moverel(0,-M->normal_interlinea*ALZA_ACCORDI);
		chord_x=getx();
		}
	}
void end_chord(void)
	{
	if (!is_strum)
		{
		chord_x=getx();
		moveto(chord_last_x,chord_last_y);
		}
	setusercharsize(M->width,16,M->height,16);
	pop_color();
	}
void con_out_phrase(struct phrase *p)
	{
	for (;p!=NULL;p=p->next)
		{
		switch(p->level)
			{
			case 1:
				begin_chord();
				break;
			case 2:
				put_color(CHORDS_COLOR);
				is_strum++;
				break;
			case 3:
				put_color(CHORDS_COLOR);
				is_strum++;
				break;
			case 5:
				is_ref=1;
				break;
			}
		if (p->p)
			con_out_phrase(p->p);
		else
			con_out_line(p->l);
		switch(p->level)
			{
			case 1:
				end_chord();
				break;
			case 2:
				pop_color();
				is_strum--;
				break;
			case 3:
				pop_color();
				is_strum--;
				break;
			case 5:
				is_ref=0;
				break;
			}
		}
	}
void con_out_verse(struct verse *p)
	{
	int y;
	for (;p!=NULL;p=p->next)
		{
		current_interlinea=phrase_has_chord(p->list)?with_chords_interlinea:M->normal_interlinea;
		moveto(0,y=(gety()+current_interlinea));
		chord_x=0;
		if (y>maxy && total_length>0)
			out_of_screen=1;
		else
			con_out_phrase(p->list);
		}
	}
void con_out_part(struct part *p)
	{
	int i,y;
	for (i=0;i<MAXPARTS && p!=NULL && !out_of_screen;p=p->next,i++)
		{
		y=gety();
		if (y+M->normal_interlinea<maxy && M->normal_interlinea+y-start>next) next=M->normal_interlinea+y-start;
		if (height[i]==0 || y+height[i]>0 || total_length==0 && !kbhit())
			{
			moverel(0,INTER_PARTS_SPACE);
			switch(p->type)
				{
				case 0: put_color(REFRAIN_COLOR);break;
				case 2: put_color(SPOKEN_COLOR);break;
				}
			moverel(0,INTER_PARTS_SPACE);
			con_out_verse(p->list);
			switch(p->type)
				{
				case 2:
				case 0: pop_color();break;
				}
			}
		else
			moverel(0,height[i]);
		height[i]=gety()-y;
		}
	}

void con_out_titles(struct song *p)
	{
	struct author *a;
	struct line v={", ","  "};
	int y;
	y=gety();
	if (titles_height==0 || y+titles_height>0 || total_length==0)
		{
		if (p->head.l)
			{
			setusercharsize(M->title*M->width,16,M->title*M->height,16);
			current_interlinea=M->title*M->normal_interlinea;
			put_color(TITLE_COLOR);
			con_out_line(p->head.l);
			moveto(0,gety()+current_interlinea);
			pop_color();
			}
		if (p->author)
			{
			setusercharsize(M->author*M->width,16,M->author*M->height,16);
			current_interlinea=M->author*M->normal_interlinea;
			put_color(AUTHOR_COLOR);
			con_out_line(p->author->name);
			for (a=p->author->next;a!=NULL;a=a->next)
				{
				con_out_line(&v);
				con_out_line(a->name);
				}
			moveto(0,gety()+current_interlinea);
			pop_color();
			}
		titles_height=gety()-y;
		}
	else
		moverel(0,titles_height);
	setusercharsize(M->width,16,M->height,16);
	}
void con_out_song(struct song *p)
	{
	next=0;
	set_lang(0);
	color_stack_pointer=0;
	out_of_screen=0;
	put_color(NORMAL_COLOR);
	con_out_titles(p);
	con_out_part(p->main);
	if (total_length==0)
		total_length=gety();
	if (next==0 || next+start<=0 || next+start>maxy)
		next=-start+maxy-M->normal_interlinea;
	setcolor(TITLE_COLOR);
	line(maxx-20,start+next,maxx,start+next);
	}
int console_out(struct song_list *index,int pos)
  {
  struct song *p;
  int quit=0;
  set_mode_chords(mode_chords);
  open_graphics();
  while (quit!=3)
    {
    quit=0;
    if (pos>=0)
      p=indexsong(pos,index);
    else p=NULL;
    if (p==NULL)
      quit=3;
    else
      {
      rereadsong(p);
      con_set_mode(-1);
      start=0;
      total_length=0;
      while (quit==0)
	{
	quit=0;
	if (start>0) start=0;
	if (start+total_length<0) start=-total_length+maxy-M->normal_interlinea;
	cleardevice();
	moveto(0,start);
	con_out_song(p);
	switch(getch())
	  {
	  case 27:quit=3;break;
	  case ' ':start=-next;break;
	  case 13 :start=0;break;
	  case 'x':
	    current_text_mode++;
	    if (current_text_mode>=nmodes) current_text_mode=0;
	      quit=1;
	  break;
	  case 'c':
	    if (mode_chords==0) mode_chords=4;
	    else if (mode_chords==4) mode_chords=1;
	    else if (mode_chords==1) mode_chords=3;
	    else if (mode_chords==3) mode_chords=5;
	    else if (mode_chords==5) mode_chords=7;
	    else mode_chords=0;
	    set_mode_chords(mode_chords);
	    quit=2;
	  break;
	  case '+':
	    set_alzo(++alzo);
	    quit=2;
	  break;
	  case '-':
	    set_alzo(--alzo);
	    quit=2;
	    break;
	  case '=':
	    set_alzo(alzo=0);
	    quit=2;
	  break;
	  case 0:
	    switch(getch())
	      {
	      case 'G':start=0;break;
	      case 'H':start+=M->normal_interlinea;break;
	      case 'I':start+=maxy-M->normal_interlinea;break;
	      case 'K':pos--;quit=2;break;
	      case 'M':pos++;quit=2;break;
	      case 'O':start=-total_length+maxy;break;
	      case 'P':start-=M->normal_interlinea;break;
	      case 'Q':start-=maxy-M->normal_interlinea;break;
	      }
	  break;
	  }
	}
      if (quit>=2)
	freemain(p);
      }
    }
  close_graphics();
  return pos;
  }


#endif
