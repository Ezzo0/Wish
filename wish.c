/******************************INCLUDES************************************/
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

/***************************Macro Declarations*****************************/
// ANSI escape codes for color formatting
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/***************************variable Definitions***************************/

// All search paths will be saved in this vector
vector search_path;

// Line read from a file stream will be stored here
char *lineptr = NULL;

FILE *fptr = NULL;


/***************************Function Declarations***************************/

int parsing(vector *);

void command_extraction(vector *, vector []);

void shell_run(vector *);

int redirection_testing(vector *);

int finding_path(char** , vector *);

void command_exe(vector *);

void display_error(void);


int main(int argc, char* argv[])
{
    vector_init(&search_path);
    vector_add(&search_path, "/bin");
    vector arguments;
    if (argc == 1)
    {
        while (1)
        {
            vector_init(&arguments);
            size_t siz = 0;
            printf(ANSI_COLOR_GREEN "wish> " ANSI_COLOR_RESET);
            if (getline(&lineptr, &siz, stdin) != -1 )
            {   
                shell_run(&arguments);
            }
            else
            {
                display_error();
            }
            vector_free(&arguments);
            free(lineptr);
            lineptr = NULL;
        }
    }
    else if (argc == 2)
    {
        fptr = fopen(argv[1], "r");
        if (fptr == NULL)
        {
            display_error();
            return 1;
        }
        vector_init(&arguments);
        char c;
        int cntr = 0;
        lineptr = (char *) malloc( sizeof(char) * 256);
        while ( (c = fgetc(fptr) ) != EOF) {
            lineptr[cntr] = c;
            if (c == '\n')
            {   
                if (strcmp(lineptr, "\n") == 0 || strcmp(lineptr, "&\n") == 0)
                {
                    lineptr = strcpy(lineptr, "");
                    cntr = 0;
                    continue;
                }
                shell_run(&arguments);
                lineptr = (char *) malloc( sizeof(char) * 256);
                cntr = 0;
                vector_delete_all(&arguments);
            }
            else
            {
                ++cntr;
            }
        }
        vector_free(&arguments);
        vector_free(&search_path);
        free(lineptr);
        fclose(fptr);
    }
    else
    {
        display_error();
        return 1;
    }
    return 0;
}


/***************************Function Definitions***************************/

/**
 * @Summary parsing the input
 * @Description This function parse the input line from characters like spaces, "&" sign, and ">" sign and store result in arguments vector. 
 * @Preconditions None
 * @param arguments - A vector to push all parsed lines in it
 * @return Number of commands in the line
 */

int parsing(vector *arguments){
    char *command;
    char *parallel_command;
    char *redirct;
    int i = 0;
    int cntr = 0;
    while ( ( command = strsep(&lineptr, "&") ) != NULL )
    {
        command = strsep(&command, "\n");
        if (command[i] == '\t')
        {
            while (command[i] == '\t')
            {
                command = &command[i+1];
            }
        }
        
        // If input was just \n or &, ignore it.
        if (strcmp(command, "") == 0)
            continue;
        
        while ( ( parallel_command = strsep(&command, " ") ) != NULL )
        {
            while ( ( redirct = strsep(&parallel_command, ">") ) != NULL )
            {
                // if white spaces was found, jus ignore it
                if (strcmp(redirct, "") == 0 && parallel_command == NULL)
                    continue;

                // If a ">" sign was found with spaces before and after it like that "command > fileName"
                if (strcmp(redirct, "") == 0 && strcmp(parallel_command, "") == 0)
                {
                    vector_add(arguments, ">");
                    continue;
                }
                
                vector_add(arguments, redirct);

                // If a ">" sign was found without spaces before and after it like that "command>fileName"
                if (parallel_command != NULL)
                {
                    vector_add(arguments, ">");
                }
            }
        }
        if (vector_size(arguments) == 0)
            continue;
        
        // To know where a command and its arguments end
        vector_add(arguments, NULL);
        cntr++;
    }
    return cntr;
}

/**
 * @Summary Extract single command
 * @Description This function extracts all commands from vector arguments and store each command in a vector in array commands. 
 * @Preconditions None
 * @param arguments - A vector that has all commands & their arguments in it
 * @param arguments - An array of vectors to store single command & its arguments in one element of the array
 * @return void
 */

void command_extraction(vector *arguments, vector commands[]){
    int cntr = 0;
    for (int i = 0; i < vector_size(arguments); i++)
    {
        void *item = vector_get(arguments, i);
        vector_add(&commands[cntr],item);
        
        // End of command & its arguments
        if (item == NULL)
            cntr++;
    }
}

/**
 * @Summary Execute instructions of wish
 * @Description This function execute the instructions that wish shell need to work. 
 * @Preconditions None
 * @param arguments - A vector that has a command & its arguments
 * @return void
 */

void shell_run(vector *arguments){
    int commands_cntr = parsing(arguments);

    vector commands[commands_cntr];
    for (int i = 0; i < commands_cntr; i++)
    {
        vector_init(&commands[i]);
    }
    command_extraction(arguments, commands);

    // Execute all input commands 
    for (int i = 0; i < commands_cntr; i++)
    {
        command_exe(&commands[i]);
    }
    // Wait for all input commands to be done
    for(int i = 0; i < commands_cntr; i++)
    {
        waitpid(-1, NULL, 0);
    }

    for (int i = 0; i < commands_cntr; i++)
    {
        vector_free(&commands[i]);
    }
}

/**
 * @Summary Test if valid redirection or not
 * @Description This function tests if there is a redirection symbol in the command or not. Also checks if it is a valid redirection or not.
 * @Preconditions None
 * @param arguments - A vector that has a command & its arguments
 * @return Status of redirection:
 *         #-1 if there is not redirection
 *         # 0 if there is a redirection and it is valid
 *         # 1 if there is a redirection and it is not valid
 */

int redirection_testing(vector *arguments){
    int redirection_symbol = 0;
    int file_cntr = 0;
    void *var = NULL;
    for (int i = 0; i < vector_size(arguments) - 1; i++)
    {
        var = vector_get(arguments,i);
        if (strcmp( (void *) var, ">") == 0)
        {
            redirection_symbol++;
        }
        else
        {
            if (redirection_symbol && var != NULL)
                file_cntr++;
        }
    }
    if ( redirection_symbol == 0 && file_cntr == 0 )
    {
        return -1; // There is not redirection
    }
    else if (redirection_symbol == 1 && file_cntr == 1)
    {
        return 0; // There is a redirection and it is valid
    }
    else
    {
        return 1; // There is a redirection and it is not valid
    }
}

/**
 * @Summary Get the search path
 * @Description This function loops through search path vector to check if there is a search path for the desired command or not.
 * @Preconditions None
 * @param path - A pointer to pointer to return the search path in it.
 * @param arguments - A vector that has a command & its arguments.
 * @return Status of accessing the executable file of the desired command:
 *         # 0 if it is accessible.
 *         # 1 if it is not accessible.
 */

int finding_path(char** path, vector *arguments){
    *path = (char *) malloc(sizeof(char)  * 256 );
    for (int i = 0; i < vector_size(&search_path); i++)
    {
        strcpy(*path, (char *) vector_get(&search_path, i) );
        strcat(*path, "/");
        strcat(*path, (char *) vector_get(arguments, 0) );
        if (access(*path, X_OK) == 0)
            return 0;
    }
    return -1;
}

/**
 * @Summary Execute the desired command
 * @Description This function checks if the command is a built-in or not. Also check if it written without any syntax errors of not.
 * @Preconditions None
 * @param arguments - A vector that has a command & its arguments
 * @return Void
 */

void command_exe(vector *arguments){
    if (strcmp( (char *) vector_get(arguments, 0), "exit") == 0 )
    {
        void *arg = vector_get(arguments,1);
        if (arg != NULL)
        {
            display_error();
            return;
        }
        
        free(lineptr);
        vector_free(arguments);
        vector_free(&search_path);
        if (fptr != NULL)
            fclose(fptr);
        
        exit(0);
    }
    else if (strcmp( (char *) vector_get(arguments, 0), "cd") == 0 )
    {
        if(vector_get(arguments, 1) == NULL || vector_get(arguments, 2) != NULL)
        {
            display_error();
        }
        else
        {
            int changed_dir = chdir( (char *) vector_get(arguments, 1) );
            if (changed_dir == -1)
            {
                display_error();
            }
        }
    }
    else if( strcmp( (char *) vector_get(arguments, 0), "path") == 0 )
    {
        if ( vector_get(arguments, 1) == NULL )
        {
            vector_delete_all(&search_path);
        }
        else
        {
            
            for (int i = 1; i < vector_size(arguments) - 1; i++)
            {
                char *path = (char *) vector_get(arguments, i);
                char *fullPath = realpath(path, NULL);
                vector_add(&search_path, fullPath);
            }
        }
    }
    else
    {
        int redirct_result = redirection_testing(arguments);
        if (redirct_result == 1)
        {
            display_error();
            return;
        }
        
        char* path = NULL;
        int accessable = finding_path(&path, arguments);
        
        if(!accessable)
        {
            int id = fork();
            if (id == 0)
            {
                if (redirct_result == 0)
                {
                    int idx = vector_size(arguments) - 2; 
                    char *file_name = (char *) vector_get(arguments,idx);
                    int file = open(file_name, O_WRONLY|O_TRUNC|O_CREAT, 0644);

                    if (file == -1)
                    {
                        display_error();
                        exit(1);
                    }

                    dup2(file, STDOUT_FILENO);
                    close(file);
                    vector_set(arguments, idx, NULL);
                    vector_set(arguments, idx - 1, NULL);
                }

                execv(path, (char **) arguments->items);
            }
        }
        else
            display_error();
    }
}

/**
 * @Description Display an error message
 * @Preconditions None
 * @return Void
 */

void display_error(void){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}