
struct song_list* ps_songs(struct song_list *selection,int page,FILE *fp);
void ps_authors_indexx(FILE* fp,struct song_list *indexx);
void ps_indexx(FILE *fp,struct song_list *selection);
void ps_verbose(int on);

extern void songprin_options_save(FILE *);
extern int  songprin_options_load(char *,char *);
