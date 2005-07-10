/* programma per la lettura della riga di comando*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmdline.h"


struct opt
{
  char *name;  /*nome dell'opzione*/
  int type; /* vedi file .h*/
  char *value; /*valore letto, NULL se non c'era.*/
  struct opt *next; 
};

struct opt *options=NULL;
int num_of_options(struct opt *p)
{
int i;
for (i=0;p;p=p->next);
return i;
}
struct opt *last_option(struct opt *p)
{
  if (p)
    for (;p->next;p=p->next);
  return p;
}

struct opt *find_option(struct opt *p,char **name)
{
  struct opt *q=NULL;
  int i,maxlen=0;
  if (name!=NULL)
    {
      for (;p;p=p->next)
	{
	  if (!(p->type&(VOID_OPT|CONTINUED_OPT)))
	    {
	      for (i=0;p->name[i]==(*name)[i]&&(*name)[i]!='\0';i++);
	      if (i>maxlen && (p->name[i]=='\0'||p->type&ABBREV_OPT))
		{
		  maxlen=i;
		  q=p;
		}
	    }
	}
      *name+=maxlen;
    }
  else
    {
      for (;p && !(p->type&VOID_OPT);p=p->next);
      q=p;
		}
	if (q && q->value!=NULL)
		{
			for (;q->next && (q->next->type&CONTINUED_OPT);q=q->next);
			p=q->next;
			q->next=(struct opt *)malloc(sizeof(struct opt));
			if (q->next==NULL)
				{
				fprintf(stderr,"memoria insufficiente\n");
				return NULL;
				}
			q->next->name=NULL;
			q->next->type=CONTINUED_OPT|q->type;
			q->next->value=NULL;
			q->next->next=p;
			q=q->next;
		}
	return q;
}

int new_option(char *name,int type)
{
  struct opt *p,*q;
  p=(struct opt *)malloc(sizeof(struct opt));
  if (p)
    {
      p->name=name;
      p->type=type;
      p->value=NULL;
      p->next=NULL;
      q=last_option(options);
      if (q==NULL) options=p;
      else q->next=p;
    }
  else
    fprintf(stderr,"memoria insufficiente\n");
  return num_of_options(options);
}

void check_options(void)
{
  struct opt*p;
  for (p=options;p;p=p->next)
    {
      printf("OPZIONE: %s VALORE: %s\n",
       p->type&VOID_OPT?"VOID":(p->type&CONTINUED_OPT?"CONTINUED":p->name),
	     p->value==NULL?"NULL":p->value);
    }
}
int parse(int argc,char *argv[])
{
  char c;
  char *s;
  struct opt *p=NULL;
  int i;
  int wait=0;
  for (i=1;i<argc;i++)
    {
      if (argv[i][0]=='-'||argv[i][0]=='+')
	{
	  if (wait)
	    p->value="";
	  wait=0;
	  for (s=argv[i];*s!='\0';)
	    {
	      c=*s;
	      if (c=='-'||c=='+')
		s++;
	      p=find_option(options,&s);
	      if (p==NULL)
		{
		  fprintf(stderr,"Opzione sconosciuta: %s\n",s);
		  return 0;
		}
	      if (c=='+')
		p->value="+";
	      else
		p->value="-";
	      if (p->type&WITH_VALUE_OPT)
		{
		  if (*s=='\0')
		    wait=1;
		  else
		    {
		      p->value=s;
		      s=strchr(s,'\0');
		    }
		}
	    }
	}
      else
	{
	  if (wait)
	    {
	      p->value=argv[i];
	      wait=0;
	    }
	  else
	    {
	      p=find_option(options,NULL);
	      if (p==NULL)
		{
		  fprintf(stderr,"Manca l'opzione\n");
		  return 0;
		}
	      p->value=argv[i];
	    }
	}
    }
  return 1;
}
char *value_of_option(char *name,int n)
{
  struct opt *p;
  int i;
  for (p=options;p && (p->name==NULL?1:!!strcmp(p->name,name));p=p->next)
    {
    if (p->name!=NULL)
      if (!strcmp(p->name,name)) break;
    }
  for (i=0;p && i<n && (i==0 || p->type&CONTINUED_OPT);i++,p=p->next);
  if (p && (n==0 || p->type&CONTINUED_OPT))
    return p->value;
  else return NULL;
}
int is_on_option(char *name)
	{
	struct opt*p;
	for (p=options;p && (p->name==NULL || strcmp(p->name,name));p=p->next);
	if (p->value)
		switch((p->value)[0])
			{
			case '-': return -1;
			case '+': return 1;
			default: return 2;
			}
	else return 0;
	}
int number_of_values_of_option(char *name)
{
  struct opt *p;
  int i;
  for (p=options;p && (p->name==NULL || strcmp(p->name,name));p=p->next);
  if (p)
    {
      if (p->value==NULL) return 0;
      for (i=1,p=p->next;p && p->type&CONTINUED_OPT;i++,p=p->next);
      return i;
    }
  else return -1;
}
void end_options(void)
{
  struct opt *p,*q;
  for (p=options;p;p=q)
    {
      q=p->next;
      free(p);
    }
}

