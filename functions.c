#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "functions.h"
#define HISTORYSIZE 20
#define ALIASSIZE 10

int write_history(char history[][513]);

int setpath(int index, char* token_array[]){
    if(index < 3){
            printf("Too few arguments entered for this function\n");            //check the index to see how many commands have been entered and provide appropriate responses 
            return 0;  
        }
        else if(index > 3){
            printf("Too many arguments entered for this function\n");
            return 0;
        }
        else{
            if(setenv("PATH", token_array[1], 1) == -1){
            perror("Error occurred");
            return -1;
        }
        printf("The new PATH is now %s\n", getenv("PATH"));                 //sets the path to whatever in token array 
            return 0;
        }
        }


int getpath(int index){                                     //checks for correct number of arguments and gets the path if appropriate

    if(index > 2){
        printf("Too many arguments entered for this function\n");
        return 0;
    }
    else{
        printf("The current PATH is %s\n", getenv("PATH"));   
        return 0;  
    }
}

int changeDirectory(int index, char* token_array[]){                //checks number of argument and changes the directory if appropriate
    if(index == 2){
            chdir(getenv("HOME"));
            return 0;
        }
        if(index > 3){
            printf("Too many arguments entered for this function\n");
            return 0;
        }
        if(index == 3){
            if(chdir(token_array[1]) == -1){
                perror("Error occurred");
                printf("%s not valid\n", token_array[1]);
                return -1;
            }
        }
        return 0;
}
int execute_command(char* token_array[]){
    pid_t res = fork();
     if(res < 0){                                    // if no child process created return error 
        fprintf(stderr, "Couldn't spawn a child process\n");
        return -1;
        }
    if(res == 0){                                            //in child process 
        if(execvp(token_array[0], token_array) == -1){          //commands carried out by exec, checking for error 
            perror("Error occurred");
            printf("%s causing issue\n", token_array[0]);         //print name of thing causing error
            exit(0);
    	    return -1;
        }
    }
    else{
        wait(NULL); 
        return 0;             //else statement means we are not in the child process and parent process waits for child to exit
    }
    return 0;
}

int tokenise(char input[], char* token_array[], int *index){
    char delimit[] = "' ',\t,|,>,<,&,;,\n";
    char *token = strtok(input,delimit);            //split input into tokens as defined above
    while(token != NULL){                           //go through and    printf("%s",history[0]); print all tokens
    token_array[(*index)++] = token;
    token = strtok(NULL, delimit);                //this is necessary to continue printing all tokens
    }

    token_array[(*index)++] = NULL;                        //this ensures our string is NULL terminated
return 0;
}

int print_history(char history[][513], int *total_commands, int* index){        //checks arguements and prints history 
    if(*index > 2){
        printf("Too many commands enetered for this function\n");
        return 0;
    }
    if(*(total_commands) <= 20){
        for(int i = 0; i < (*total_commands); i++){
            printf("%d: %s",i+1, history[i]);
        }
    }
    else{
        for(int i = 0; i < 20; i++){
            printf("%d: %s", i+1, history[i]);
        }
    }
    return 0;
}

int write_history(char history[][513]){                                 //writes the history to a file, prints appropriate message if file not found
    chdir(getenv("HOME"));
    FILE *fp = fopen(".hist_list", "w+");
    if(fp == NULL){     
        printf("No file found\n");
        return 0;
    }
    for(int i = 0; i < 20; i++){
        fputs(history[i], fp);
    }
    fclose(fp);
    return 0;
}

int read_history(char history[][513], int* hist_index,int *total_commands){     //reads in the history file and adds to history, checks if file is corrupted
    char buffer[513];
    int i = 0;
    FILE *fp = fopen(".hist_list", "r");
    if(fp == NULL){
        printf("File not found to read in history\n");
        return 0;
    }
    while(fgets(buffer, 513, fp) != NULL && i < 20){
        if(strlen(buffer) <= 1){
            printf("History corrupted\n");
            break;
        }
        strcpy(history[i], buffer);
        i++;
        (*hist_index)++;
        (*total_commands)++;
    }
    fclose(fp);
    return 0;
}

int history_command(char input[], char* token_array[], int* hist_index, int* index, int* total_commands, char history[][513]){
    if(strcmp("!!\n",input) == 0){ 
        strcpy(input, history[(*hist_index-1)]);                             //if !!, executes the last command ran       
    }
    else{
        int hist_num;
        char *getinput = strtok(input,"' ',\t,|,>,<,&,;,\n,!");
        if(getinput == NULL){
            printf("Please enter a number for the command to be ran\n");
            return -1;
        }
        else{
        if((hist_num = atoi(getinput)) == 0){           //check if can be converted to int or not
            printf("Not valid entry\n");
            return -1;
        }                              //converts whatever is after ! to an int and then gets the relevant index from history array
        }
        if(hist_num)
        if(hist_num > HISTORYSIZE){
            printf("Not in history range\n");
            return -1;
        }
        if(hist_num > *total_commands){
            printf("There haven't been this many commands entered\n");
            return -1;
        }
        if(hist_num < 0){
            if(((*hist_index)-1) + hist_num < 0){
                printf("Not in a valid range\n");
                return -1;
            }
            else{
                strcpy(input,history[((*hist_index)-1)+hist_num]);      
                return 0;
            }
        }
        else{
            strcpy(input,history[hist_num-1]);                  //if everything is valid copies the history to the input buffer to be tokenised 
            return 0;
        }
    }
    return 0;
}
int add_alias(char* token_array[], int* alias_counter, Alias alias_array[ALIASSIZE], int* index){   //checks arguments and provide response if not enough
    if((*index) < 4){
        printf("Not enough arguments entered for this function\n");
        return 0;
    }
    for(int i = 0; i < (*alias_counter); i++){                      //checks to see if alias name already exists, then overwrite if so while providing message
        if(strcmp(token_array[1], alias_array[i].name) == 0){
            printf("Overwriting command\n");
            strcpy(alias_array[i].command, " ");
            for(int j = 2; j < (*index)-1; j++){
                strcat(alias_array[i].command, token_array[j]);
                strcat(alias_array[i].command, " ");
            }
            return 0;
        }
    }
    if((*alias_counter + 1) > ALIASSIZE){                       //if max number of aliases already reached, print error 
        printf("Max number of aliases reached\n");
        return 0;
    }
    Alias new_alias;                                            //else add the alias to the alias array
    strcpy(new_alias.command, " ");
    strcpy(new_alias.name, token_array[1]);
    for(int i = 2; i < ((*index) -1); i++){
        strcat(new_alias.command, token_array[i]);
        strcat(new_alias.command, " ");
    }
    alias_array[*alias_counter] = new_alias;
    (*alias_counter)++;
    return 0;
}

int remove_alias(char* token_array[], Alias alias_array[ALIASSIZE], int *alias_counter, int* index){ //provides parameter checking and appropriate message
    bool isAlias = false;
    if((*index) < 2){
        printf("Not enough arguments entered for this function\n");
    }
    if((*index > 3)){
        printf("Too many arguments entered for this function\n");
    }
    if(*alias_counter == 0){
        printf("There are currently no aliases\n");
    }
    for(int i = 0; i < (*alias_counter); i++){                              //checks to see if the alias requested to be removed exists, error message if not, if it does exist it deletes it
        if(strcmp(token_array[1], alias_array[i].name) == 0){               //it also shifts all the aliases above it down by 1
            isAlias = true;
            for(int j = i; j < (*alias_counter) - 1; j++){
                strcpy(alias_array[j].name, alias_array[j+1].name);
                strcpy(alias_array[j].command,alias_array[j+1].command);    
            }
            strcpy(alias_array[*alias_counter].name, " ");
            strcpy(alias_array[*alias_counter].command, " ");
            (*alias_counter)--;
        }
    }
    if(!isAlias){
        printf("No alias with that name\n");
    }
    return 0;
}

int print_alias(Alias alias_array[ALIASSIZE], int* alias_counter){              //checks if aliases exist, error message if not, prints if they do
    if((*alias_counter) == 0){
        printf("There are no aliases to be printed\n");
    }
    else{
        for(int i = 0; i < (*alias_counter); i++){
            printf("Alias number: %d Alias name : %s, Alias command : %s\n", i+1, alias_array[i].name ,alias_array[i].command);
        }
    }
    return 0;
}

int write_aliases(Alias alias_array[ALIASSIZE], int* alias_counter){            //writes aliases to file, appropriate error message provided 
    chdir(getenv("HOME"));
    FILE *fp = fopen(".aliases", "w+");
    if(fp == NULL){     
        printf("No file found\n");
        return 0;
    }
    for(int i = 0; i < (*alias_counter); i++){
        fprintf(fp, "alias %s %s\n", alias_array[i].name, alias_array[i].command);
    }
    fclose(fp);
    return 0;
}

int read_aliases(Alias alias_array[ALIASSIZE], int* alias_counter){             //reads in the alises, checks if file exists and error message provided 
    FILE *fp = fopen(".aliases", "r");                                      //checks to see if the file is corrupted. 
    if(fp == NULL){
        printf("No file found\n");
        return 0;
    }
    char input[513];
    while(fgets(input, 513, fp)){
        if(strlen(input) <= 1){
            printf("Alias file corrupted\n");
            return 0;
        }
        int index = 0;
        char* token_array[50];    
        char delimit[] = "' ',\t,|,>,<,&,;,\n";
        char *token = strtok(input,delimit);            
        while(token != NULL){                           
            token_array[index++] = token;
            token = strtok(NULL, delimit);                
        }
        token_array[index++] = NULL;                        
        if(index < 3){
            printf("Alias file corrupted\n");
            return 0;
        }
        Alias new_alias;
    strcpy(new_alias.command, " ");
    strcpy(new_alias.name, token_array[1]);
    for(int i = 2; i < ((index) -1); i++){
        strcat(new_alias.command, token_array[i]);
        strcat(new_alias.command, " ");
    }
    alias_array[*alias_counter] = new_alias;
    (*alias_counter)++;

    }
    return 0;
    
}

int alias_check_substitute(Alias alias_array[ALIASSIZE], char* token_array[], bool* allChecked, int* index, int* alias_counter, int* aliases_substituted){
    char buffer[513];                                       //this function checks the token array for any alises, if an alias is detected it will concatenate the command a new buffer
    strcpy(buffer, "");                                     //if it isn't an alias, it concatenates the token itself
    for(int i = 0; i < (*index)-1; i++){                    //new buffer is then tokenised and checked repeatedly until no more substitutions can be performed
        bool match = false;                                 //or the max number is reached and we can assume it is circular 
        for(int j = 0; j < *alias_counter; j++){
            if(strcmp(token_array[i], alias_array[j].name) == 0){
                match = true;
                strcat(buffer, alias_array[j].command);
                (*aliases_substituted)++;
                strcat(buffer, " ");
                if((match)){
                    break;
                }
            }
        }
        if(!match){
            strcat(buffer, token_array[i]);
            strcat(buffer, " ");
        }
    }

        *index = 0;
        tokenise(buffer, token_array,&(*index));
        *allChecked = true;
        for(int i = 0; i < (*index)- 1; i++){
            for(int j = 0; j < *alias_counter; j++){
            if(strcmp(token_array[i], alias_array[j].name)== 0){
                *allChecked = false;
            }
        }
    }
     
    return 0;
}
