/*int num_of_options(struct opt *p);
*struct opt *last_option(struct opt *p);
*struct opt *find_option(struct opt *p,char **name);
*/


int new_option(char *name,int type);
void check_options(void);
int parse(int argc,char *argv[]);
char *value_of_option(char *name,int n);
int number_of_values_of_option(char *name);
int is_on_option(char *name);
void end_options(void);

#define VOID_OPT 1
#define CONTINUED_OPT 2
#define WITH_VALUE_OPT 4
#define ABBREV_OPT 8 






