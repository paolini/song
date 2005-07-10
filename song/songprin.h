
struct song_list* ps_songs(struct song_list *selection,int page,FILE *fp);
void ps_authors_index(FILE* fp,struct song_list *index);
void ps_index(FILE *fp,struct song_list *selection);
void ps_verbose(int on);

extern void songprin_options_save(FILE *);
extern int  songprin_options_load(char *,char *);
