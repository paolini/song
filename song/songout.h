
/* extern int console_out(int sel,int pos);
 */
extern char* word_to_string(char *s,char *ctrl,int *n,int *l);
extern void set_HREF(char *new_HREF);
extern void set_lang(int new_language);
extern void out_songs(struct song_list *p,FILE *fp);
extern void out_indexx(FILE *fp,struct song_list *selection);
extern void out_indexx_html(FILE*fp,struct song_list *indexx);
extern void out_songs_html(struct song_list *p,FILE *fp);
extern void out_songs_src(struct song_list *p,FILE *fp);
extern void out_song_con(struct song *p, void (*more)(void));
extern void out_authors(FILE *fp);
extern void out_authors_html(FILE *fp);
extern void out_authors_con(void (*more)(void));
extern void out_one_author(int n,struct author *p);
