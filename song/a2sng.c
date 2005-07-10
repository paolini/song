#include <stdio.h>
help()
{
  printf("a2sng program, manu-fatto\n");
  printf("Description: Converts special characters to song format\n");
  printf("Usage: Converts stdin to stdout\n");
}
void myputs(char *s)
{
  printf("%s",s);
}
main()
{
  int c,last=0;
  while ((c=getc(stdin))!=EOF)
    {
      switch(c)
	{
	case 0x0D:
	  myputs("\n");
	  break;
	case 0x0A:
	  if (last!=0x0D)
	    myputs("\n");
	  break;
	case 0x85:
	case 0xE0:
	  myputs("\\`a");
	  break;
	case 0x8a:
	case 0xE8:
	  myputs("\\`e");
	  break;
	case 0x82:
	case 0xE9:
	  myputs("\\'e");
	  break;
	case 0x8d:
	case 0xEC:
	  myputs("\\`i");
	  break;
	case 0xF2:
	case 0x95:
	  myputs("\\`o");
	  break;
	case 0363:
	  myputs("\\'o");
	  break;
	case 0x97:
	case 0xF9:
	case 0xA8:
	  myputs("\\`u");
	  break;
	case 0xC0:
	  myputs("\\`A");
	  break;
	case 0xC8:
	  myputs("\\`E");
	  break;
	case 0xC9:
	  myputs("\\'E");
	  break;
	case 0323:
	  myputs("\\'O");
	  break;
	case 0xAB:
	case 0xBB:
	  myputs("\"");
	  break;
	case 0xF8:
	case 0xB0:
	  myputs("^");
	case 0xFF:
	  break;
	default:
	  if (c<128)
	    putc(c,stdout);
	  else
	    {
	      fprintf(stderr,"Carattere sconosciuto: 0x%02X\n",(int)c);
	      putc(c,stdout);
	    }
	}
      last=c;
    }
}
