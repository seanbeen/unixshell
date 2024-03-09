#define ALIASSIZE 10

typedef struct ALIAS{
    char name[513];
    char command[513];
}Alias;

int setpath(int index, char* token_array[]);
int getpath(int index);
int changeDirectory(int index, char* token_array[]);
int execute_command(char* token_array[]);
int tokenise(char input[], char* token_array[], int *index);
int print_history(char history[][513], int *total_commands, int* index);
int write_history(char history[][513]);
int read_history(char history[][513], int* hist_index, int*total_commands);
int history_command(char input[], char* token_array[], int* hist_index, int* index, int* total_commands, char history[][513]);
int add_alias(char* token_array[], int* alias_counter, Alias alias_array[ALIASSIZE], int* index);
int remove_alias(char* token_array[], Alias alias_array[ALIASSIZE], int* alias_counter, int* index);
int print_alias(Alias alias_array[ALIASSIZE], int* alias_counter);
int write_aliases(Alias alias_array[ALIASSIZE], int* alias_counter);
int read_aliases(Alias alias_array[ALIASSIZE], int* alias_counter);
int alias_check_substitute(Alias alias_array[ALIASSIZE], char* token_array[], bool* allChecked, int* index, int* alias_counter, int* aliases_substitued);