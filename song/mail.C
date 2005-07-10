#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "mail.h"

int is_empty(char *s)
{
  while (isspace(*s)) s++;
  return (*s=='\0');
}

char *begins(char *s,char *p)
{
  while (*s==*p) {s++;p++;}
  if (*p=='\0') return s;
  else return NULL;
}

int field_write(char *field,char *s, char *p)
{
s=begins(s,p);
if (s==NULL) 
    return 0;
strcpy(field,s);
if (strchr(field,'\n'))
  *strchr(field,'\n')='\0';
return 1;
}


void read_intro(struct mail *p,FILE *fp)
{
  char s[MAIL_MAXLINE];
  do
    {
      fgets(s,MAIL_MAXLINE-1,fp);
      field_write(p->date,s,"Date: ");
      field_write(p->from,s,"From: ");
      field_write(p->to,s,"To: ");
      field_write(p->subject,s,"Subject: ");
      field_write(p->content_type,s,"Content-Type: ");
      field_write(p->content_length,s,"Content-Length: ");
    }
  while (!is_empty(s) && !feof(fp));
}

/* torna un FILE* su cui si puo' scrivere per rispondere al mail!*/
FILE *answer(struct mail *m,char *subject)
{
  FILE *out;
  char buf[MAIL_MAXLINE];
  sprintf(buf,"sendmail \"%.40s\"",m->from); 
  out=popen(buf,"w");
  //out=stdout;
  if (out)
    {
      if (subject)
	fprintf(out,"Subject: %s\n",subject);
      else
	fprintf(out,"Subject: RE: %s\n",m->subject);
    }
  else
    {
      fprintf(stderr,"Non riesco ad aprire la pipe (%s)\n",buf);
    }
  return out;
}

