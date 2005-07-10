/*FUNZIONI PER L'UTILIZZO DEL MOUSE*/
#ifdef DOS
#include <dos.h>
#include "mouse.h"

int there_is_mouse()
	{
	struct REGPACK r;
	r.r_ax=0x0000;
	intr(0x33,&r);
	return(r.r_ax);
	}
int mouse_xy(int *x,int *y)
	{
	struct REGPACK r;
	r.r_ax=0x0003;
	intr(0x33,&r);
	*x=r.r_cx;
	*y=r.r_dx;
	return(r.r_bx);
	}
int mouse_x()
	{
	struct REGPACK r;
	r.r_ax=0x0003;
	intr(0x33,&r);
    return(r.r_cx);
	}
int mouse_y()
	{
	struct REGPACK r;
	r.r_ax=0x0003;
	intr(0x33,&r);
	return(r.r_dx);
	}
void mouse_on()
	{
	struct REGPACK r;
	r.r_ax=0x0001;
	intr(0x33,&r);
	}
void mouse_off()
	{
	struct REGPACK r;
	r.r_ax=0x0002;
	intr(0x33,&r);
	}
void mouse_to(int x,int y)
	{
	struct REGPACK r;
	r.r_ax=0x0004;
	r.r_cx=x;
	r.r_dx=y;
	intr(0x33,&r);
	}
void mouse_rangex(int min,int max)
	{
	struct REGPACK r;
	r.r_ax=0x0007;
	r.r_cx=min;
	r.r_dx=max;
	intr(0x33,&r);
	}
void mouse_rangey(int min,int max)
	{
	struct REGPACK r;
	r.r_ax=0x0008;
	r.r_cx=min;
	r.r_dx=max;
	intr(0x33,&r);
	}
/*MOUSE 2*/
void mouse_reset()
	{
	struct REGPACK r;
	r.r_ax=0x21;
	r.r_bx=r.r_cx=r.r_dx=r.r_si=r.r_di=r.r_ds=r.r_es=0;
	intr(0x33,&r);
	}
int mouse_pressed(int pulsante)
	{
	int stato,conteggio;
	stato=mouse_getinfopressed(&conteggio,pulsante);
	return(conteggio?conteggio:stato);
	}
int mouse_getinfopressed(int *conteggio,int pulsante)
	{
	struct REGPACK r;
	r.r_ax=5;
	r.r_bx=pulsante;
	r.r_cx=r.r_dx=r.r_si=r.r_di=r.r_di=r.r_ds=r.r_es=0;
	intr(0x33,&r);
	*conteggio=r.r_bx;
	return(r.r_ax);
	}
int mouse_getinfounpressed(int *conteggio,int pulsante)
	{
	struct REGPACK r;
	r.r_ax=6;
	r.r_bx=pulsante;
	r.r_cx=r.r_dx=r.r_si=r.r_di=r.r_ds=r.r_es=0;
	intr(0x33,&r);
	*conteggio=r.r_bx;
	return(r.r_ax);
	}
#endif
