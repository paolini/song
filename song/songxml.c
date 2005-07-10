void out_line_xml(struct line *l) {
}

void out_titles_xml(struct song *p) {
  struct author *a;
  struct info *i;
  for (i=&(p->head);i!=NULL;i=i->next) {
    if (!i->l) break;
    switch (i->type) {
    case 't':
      fprintf(output,"<title>");
      out_line_xml(i->l);
      fprintf(output,"</title>\n");
      break;
    default:
      fprintf(stderr,"Unsupported info type '%c'... ignored\n",i->type);
    }
  }

  if (p->head.l) {
    
  }
}

void out_song_xml(struct song *p) {
  rereadsong(p);
  fprintf(output,"<head>\n");
  out_titles_xml(p);
  fprintf(output,"</head>\n");
  fprintf(output,"<body>\n");
  out_part(p->main);
  fprintf(output,"</body>\n");
  freemain(p);
}

void out_songs_xml(struct song_list *p, FILE *fp) {
  language=10;
  output=fp;
  more=NULL;
  for (;p!=NULL;p=p->next) {
    fprintf(output,"<song>\n");
    out_song_xml(p->first);
    fprintf(output,"</song>\n\n");
  }
}
