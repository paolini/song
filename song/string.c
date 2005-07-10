#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int block_size=80; /*step di allocazione della memoria*/
struct string
{
  int nalloc; /*numero di caratteri allocati compreso il '\0'*/
  char *s; /*puntatore al primo carattere*/
};



typedef struct string STRING;

void set_block_size(int new)
{
  block_size=new;
}

STRING new_string(char *p)
{
  STRING s;
  if (p!=NULL)
    s.nalloc=strlen(p)+1;
  else
    s.nalloc=0;
  s.s=p;
  return s;
}

STRING delete_string(STRING s)
{
  if (s.s!=NULL)
    free(s.s);
  s.nalloc=0;
  return s;
}

STRING string_alloc(STRING s,int n)
{
  if (s.nalloc<n)
    {
      if ((n-s.nalloc)%block_size!=0)
	n+=(block_size-(n-s.nalloc)%block_size);
      if (s.s!=NULL)
	s.s=(char *) realloc(s.s,sizeof(char)*(s.nalloc=n));
      else
	{
	  if ((s.s=(char *) malloc(sizeof(char)*(s.nalloc=n)))!=NULL)
	    *(s.s)='\0';
	}
      if (s.s==NULL) 
	{
	  fprintf(stderr,"Memoria esaurita\n");
	  exit(1);
	  s.nalloc=0;
	}
    }
  return s;
}

STRING string_realloc(STRING s)
{
  if (s.s)
    {
      s.s=(char *) realloc(s.s,sizeof(char)*(s.nalloc=(strlen(s.s)+1)));
      if (s.s==NULL) s.nalloc=0;
    }
  return s;
}     
int string_len(STRING s)
{
  return s.s?strlen(s.s):0;
}

STRING string_cat(STRING s,char *p)
{
  s=string_alloc(s,string_len(s)+strlen(p)+1);
  if (s.s)
    strcat(s.s,p);
  return s;
}

STRING string_add_char(STRING s,char c)
{
  char *p;
  s=string_alloc(s,string_len(s)+2);
  if (s.s)
    {
      p=strchr(s.s,'\0');
      if (p)
	{
	  *p=c;
	  *(p+1)='\0';
	}
    }
  return s;
}     

STRING string_insert(STRING s,char *p,char c)
{
  char *q;
  if (s.s==NULL) return string_add_char(s,c);
  s=string_alloc(s,string_len(s)+2);
  if (s.s)
    {
      for (q=strchr(p,'\0');q>=p;q--)
	*(q+1)=*q;
      *p=c;
    }
  return s;
}

STRING string_merge(STRING s,STRING t)
{
  if (s.s==NULL) return t;
  if (t.s==NULL) return s;
  s=string_alloc(s,string_len(s)+string_len(t)+1);
  if (s.s)
      strcat(s.s,t.s);
  delete_string(t);
  return s;
}

STRING string_cpy(STRING s,char *p)
{
  s=string_alloc(s,strlen(p)+1);
  if (s.s)
    strcpy(s.s,p);
  return s;
}


