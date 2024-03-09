#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "functions.h"
#define HISTORYSIZE 20
#define ALIASSIZE 10

bool running = true;
char *path;                                             //pointer to store the current path before changing it
char history[HISTORYSIZE][513];                         // historyarray
int hist_index = 0;                                     //history index
int total_commands = 0;                                 //number of total commands in history
int alias_counter = 0;                                  //number of aliases 
Alias alias_array[ALIASSIZE];                           //alias array

int main(int argc, char * const argv[]){
path = getenv("PATH");                                                         //get the current path variable and save it in pointer set
chdir(getenv("HOME"));                                                         //change the directory to HOME
read_history(history, &hist_index, &total_commands);                           //read in the history when the shell starts 
read_aliases(alias_array, &alias_counter);                                     //read in aliases when shell starts 
while(running){
int aliases_subsitituted = 0;                                                  //keeps track of many alias substitutions have taken place
char* token_array[50];                                                         //array for storing tokens 
char input[513];                                                               //to store user input
int index = 0;                                                                 //keeps track of how many tokens have been entered
bool allChecked = false;                                                       //used to check if there are any more aliases left in the input

printf(">>");                                                                  //print to terminal for user input

if(fgets(input,513,stdin) == NULL){                           //read in the input, if input is NULL, i.e. in the case of <ctrl-D> close the program
    running = false;
    setenv("PATH", path,1);                                   //restore the PATH to the original value before exiting shell
    printf("Shell exiting, PATH has been restored to: %s\n", getenv("PATH"));
    write_history(history);                                   //write the history and aliases when closing this method
    write_aliases(alias_array, &alias_counter);
    exit(0);
}
if (strchr(input, '\n') == NULL) {                        //this checks to see if the input is greater than the input buffer and then clears the rest of IO stream and reports error 
    int c;
    while((c = getc(stdin)) != '\n' && c != EOF);
    fprintf(stderr, "Input too large, please try again\n");
    continue;
}   
if(strlen(input) <= 1){                                    //checks to see if anything has actually been entered or not
    continue;
}
printf("%s", input);
if(strcspn("!",input) != 0){                        //checks if history invocation or not, if it's not adds the command to history, if history is full shifts everything down by 1
    if(hist_index+1 >= 20){
        for(int i = 0; i < 20; i++){
            strcpy(history[i], history[i+1]);
        }
        strcpy(history[HISTORYSIZE-1],input);
    }
    else{
        strcpy(history[hist_index],input);
        hist_index = (hist_index + 1);
    }
    total_commands++;
}  

if((strcspn("!", input) == 0)  && (strcspn(input,"a") != 0)){
    if(history_command(input, token_array, &hist_index, &index, &total_commands,history) == -1){
        continue;
    }              //if this is a history invocation, function deals with it
    }


tokenise(input,token_array,&index);                                                                 //tokenise the user input

    if((strcmp(token_array[0],"alias") == 0) && (index > 2)){               //if alias command with appropriate parameters add to aliases
        add_alias(token_array, &alias_counter, alias_array, &index);
        continue;
    }

    if((strcmp(token_array[0], "alias") == 0) && (index = 2)){          //print aliases if requested 
        print_alias(alias_array, &alias_counter);
        continue;
    }

    if((strcmp(token_array[0], "unalias") == 0)){                               //remove an alias
        remove_alias(token_array, alias_array, &alias_counter, &index);
        continue;
    }

    alias_check_substitute(alias_array, token_array, &allChecked, &index, &alias_counter, &aliases_subsitituted);       //checks if there are aliases in the token arry and substitutes them for their command 
    while(!allChecked){
        alias_check_substitute(alias_array, token_array, &allChecked, &index, &alias_counter, &aliases_subsitituted);
        if(aliases_subsitituted > ALIASSIZE){
            printf("This is a circular reference\n");
            break;
        }
    }

    if(aliases_subsitituted > ALIASSIZE){                   //checks to see if we have a circular alias reference
        continue;
    }
    
    if(strcmp(token_array[0], "exit") == 0){            //if user types exit, exit program while writing history and aliases to file 
        setenv("PATH",path,1);                  //restore the PATH to original value before exiting shell
        printf("Shell exiting, PATH has been restored to: %s\n", getenv("PATH"));
        write_history(history);
        write_aliases(alias_array, &alias_counter);
        exit(0);
    }

    if(strcmp(token_array[0], "history") == 0){             //print the history
        print_history(history, &total_commands, &index);
        continue;
    }

    if(strcmp(token_array[0], "getpath") == 0){     //check if getpath command being invoked, if so then execute 
    getpath(index);
    continue;
    }
    
    if(strcmp(token_array[0], "setpath") == 0){       //check if setpath command being invoked, if so then execute
        setpath(index, token_array);
        continue;
    }
    
    if(strcmp(token_array[0], "cd") == 0){              //check if cd being invoked, if so then execute 
        changeDirectory(index, token_array);
        continue;
    }
    
    
    else{
     execute_command(token_array);                  //if none of the above, will attempt to exec command 
     continue;
    }

}
}