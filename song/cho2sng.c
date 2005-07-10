#include <stdio.h>
#include <ctype.h>

int begins(char *s,char *p)
{
  int i;
  for (i=0;p[i] && p[i]==s[i];i++);
  return (p[i]=='\0');
}
char *skip(char *p)
{
  while (isspace(*p)) p++;
  return p;
}
int status=0; 
/* 0: devo ancora cominciare a leggere i titoli
   1: sto leggendo i titoli
   2: sto scrivendo il testo
   3: c'era una riga vuota nel testo
   4: sto scrivendo il rit.
   5: c'era una riga vuota nel rit.
   6: tab
*/
void main(void)
{
  char buff[256];
  char *p,*q;
  while (!feof(stdin))
  {
    fgets(buff,255,stdin);
    if (status!=6)
      p=skip(buff);
    else p=buff;
    if (*p=='{')
      {
	p++;
	p=skip(p);
	if (begins(p,"title:")||begins(p,"t:"))
	  {
	    status=1;
	    p=strchr(p,':')+1;
	    p=skip(p);
	    q=strchr(p,'}');
	    if (q) *q='\0';
	    printf("\\Title %s\n",p);
	  }
	else if (begins(p,"subtitle:")||begins(p,"st:"))
	  {
	    status=1;
	    p=strchr(p,':')+1;
	    p=skip(p);
	    q=strchr(p,'}');
	    if (q) *q='\0';
	    printf("\\Author %s\n",p);
	    
	  }
	else if (begins(p,"soc"))
	  {
	    status=5;
	  }
	else if (begins(p,"eoc"))
	  {
	  status=2;
	  }
	else if (begins(p,"c:") || begins(p,"comment:"))
	  {
	    p=strchr(p,':')+1;
	    p=skip(p);
	    q=strchr(p,'}');
	    if (q) *q='\0';
	    if (status==1) 
	      {
		printf("\\s\n");
		status=2;
	      }
	    printf("\\n{%s}\n",p);
	  }
	else if (begins(p,"sot"))
	  {
	    printf("\\b\n");
	    status=6;
	  }
	else if (begins(p,"eot"))
	  {
	    status=1;
	  }
	else
	  {
	    printf("%%cho: {%s",p);
	  }
      }
    else if (*p=='#')
      {
	printf("%%%s",p);
      }
    else if (*p=='\0')
      {
	if (status==2)
	  {
	    status=3;
	  }
	else if (status==4)
	  status=5;
      }
    else if (status==6)
      {
	printf("%s",p);
      }
    else
      {
	if (status==1 || status==3)
	  {
	    printf("\\s\n");
	    status=2;
	  }
	if (status==5)
	  {
	    printf("\\r\n");
	    status=4;
	  }
	printf("%s",p);
      }
  }
}

