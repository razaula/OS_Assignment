// C Program to design a shell in Linux
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <signal.h>
#define _BSD_SOURCE
#define _DEFAULT_SOURCE

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
#define DATASIZE 1000 // max size of data file
#define MAXPATH 1024 // max number of characters for a file path
#define MAXARGS 50 //max number of arguments

void start(char *args);
void whereami(void);
void init_shell(void);
void movetodir(char* path);
void background(char *args);
int isCommand(char *string);
void processInput(char* str);
char *stripwhite(char *string);

//Variable for length of history
int histCount=0;

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

//Array of all commands
char originalDir[MAXPATH];
char currentDir[MAXPATH];

//Node for the history linked list
typedef struct node
{
    int position;
    char* command;
    char* arg;
    struct node *next;
    struct node *prev;
} node;

//Initializing history head node
node *head, *tail;

void createHistory()
{
  FILE *file = fopen("history_list.txt", "r");

  // Create a new history list if no file exists.
  head = malloc(sizeof(node));
  head->command = NULL;
  head->position = 0;
  head->command = NULL;
  head->arg = NULL;
  head->next = NULL;
  head->prev = NULL;
  tail = head;


  // Read the commands from the list.
  if (file != NULL)
  {
    char buffer[MAXCOM];
    node *temp;
    char *token = NULL;

    while (fgets(buffer, MAXCOM, file) != NULL)
    {
      temp = malloc(sizeof(node));
      temp->position = 0;
      temp->command = malloc(sizeof(char));
      temp->arg = malloc(sizeof(char));
      temp->next = NULL;
      temp->prev = NULL;

      token = strtok(buffer, " ");
      strcpy(temp->command, token);

      token = strtok(buffer, " ");

      if (token != NULL)
        strcpy(temp->arg, token);

      histCount++;
      tail->position++;

      tail->next = temp;
      temp->prev = tail;
      tail = temp;
    }
    tail->next = NULL;
  }

  fclose(file);
}

//Clear history function
void clearHistory()
{
    
    //Clear the linked list
    node *temp = head;

    while (temp->next)
    {
      temp = head->next;
      free(head);
      head = temp;
    }
    // free(temp);
    printf("head: %p\n", head);
}

//History function that prints out the recently typed commands
void history(char *args)
{
    addHistory('history', args);
    
    node *temp = tail;
    int i = 0;

    if (head == NULL)
    {
      printf("history list is empty\n");
      return;
    }

    if (head != NULL && args == NULL)
    {
      printf("head: %p\n", head);
      while (temp != NULL)
      {
        printf("%d: %s\n", i, temp->command);
        i++;
        temp = temp->prev;
      }
    }
    else if (strcmp(args, "-c") == 0)
    {
      clearHistory();
    }
    return;
}

//Function that saves the history to a file
void saveHistory()
{
    
    //Open/create a file and write the linked list to it
    char data[DATASIZE];
    FILE *file;
    node *temp = head;

    file = fopen("history_list.txt", "w");

    if (file == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        return;
    }

    while (temp != NULL)
    {
      fputs(temp->command, file);
      fputs("\n", file);
      temp = temp->next;
    }

    fclose(file);

    return;
}

//Replay function
void replay(char *args)
{
    //verify that args is a number
        //If not throw an error and return

    //Filter through history linked list until temp->position = args
        //If found print that node to the screen and execute the command
        //else throw error
// if(head == NULL)
    // {
    //     head = temp;
    //     tail = temp;
    //     return;
    // }
    return;
}

//Dalek function
void dalek(char *args)
{
    addHistory('dalek', args);
    
    //Intitializing the PID
    pid_t pid;

    //Verifiying that args is a number
    if(args == NULL)
    {
        printf("Error: No PID entered.\n");
    }
    else if(isdigit(*args)==0)
    {
        printf("Error: Invalid PID.\n");
    }
    else
    {
        pid = kill(*args, SIGKILL); //Killing the process
        if(!pid)
            printf("Processes terminated successfully.\n");
        else
            printf("Error: Process termination was unsuccessful.\n");
    }
    return;
}

//Add to history function
void addHistory(char *commands, char *args)
{
    if (commands == NULL)
    {
      printf("Command is empty\n");
      return;
    }

    //Creating temp nodes
    node *temp = malloc(sizeof(node));
    temp->command = malloc(sizeof(char));
    temp->arg = malloc(sizeof(char));
    
    //Saving the data to the new node
    temp->position = histCount;
    strcpy(temp->command, commands);

    if (args != NULL)
      strcpy(temp->arg, args);

    // if(head == NULL)
    // {
    //     head = temp;
    //     tail = temp;
    //     return;
    // }

    // node *t = tail;
    tail->next = temp;
    temp->prev = tail;
    tail = temp;

    if (histCount == 0)
      head = tail;

    tail->next = NULL;

    //Incrementing the history count
    histCount++;
}

//ByeBye Function
void byebye(char* args)
{
    addHistory('byebye', args);
    
    //throws error if there are parameters
    if(args != NULL)
    {
        printf("Too many arguments.\n");
        return;
    }
    else
    {
        //Adding 'byebye' to history
        addHistory("byebye", NULL);

        //Saving history to file
        saveHistory();

        //Exiting the program
        exit(0);
    }
}

// Greeting shell during startup
void init_shell()
{
    clear();
    printf("\n\n\n\n******************"
        "************************");
    printf("\n\n\n\t****MY SHELL****");
    printf("\n\n\t-USE AT YOUR OWN RISK-");
    printf("\n\n\n\n*******************"
        "***********************");
    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    clear();
}

// Function to print Current Directory.
void whereami()
{
    addHistory('whereami', args);
    printf("\nDir: %s\n", currentDir);
}

void movetodir(char* path)
{
    addHistory('movetodir', *path);
    char *temp;

    //Checking if the directory is a relative or absolute path
    if(path[0] == '/')
    {
        temp = realpath(path, NULL);
    }
    else
    {   //converts relative path to direct path
        if(currentDir[0] != '\0')
        {
            char rPath[MAXPATH];
            strcpy(rPath, currentDir);
            strcat(rPath, "/");
            strcat(rPath, path);

            temp = realpath(rPath, NULL);
        }
        else
        {
            printf("Cannot retrieve current directory.\n");
            return;
        }
    }

    if(temp != NULL)
    {
        //opening the directory
        DIR *dir = opendir(temp);

        //Checks if path exists
        if(dir)
        {
            strcpy(currentDir, temp);
        }
        else
        {
            free(temp);
            printf("Error: No such directory.\n");
            return;
        }

        free(temp);
        closedir(dir);
    }
    else
    {
        printf("Error: No such directory.\n");
        return;
    }
}

//Gets the length of the arguments
int argsLength(char **args)
{
    int j = 0;
    while(args[j] != NULL)
    {
        j++;
    }

    return j;
}

//Function to start programs
void start_background(char *command, char *args)
{
    addHistory(command, args);
    
    char *params[MAXARGS];
    int i,n, j = 0;
    pid_t pid;

    //Error if there are no arguments passed with 'start'
    if (args == NULL)
    {
        if(command == 'start')
        {
            printf("Error:'start' requires an argument.\n");
            return;
        }
        else
        {
            printf("Error:'background' requires a program argument.\n");
            return;
        }
    }
    else
    {
        //Duplicating the args string because strsep won't work otherwise
        char *args2 = strdup(args);
        
        //Saving the program parameters to an array
        for (i = 0; i < MAXARGS; i++)
        {
            params[i] = strsep(&args2, " ");
            if (params[i] == NULL)
                break;
            if (strlen(params[i]) == 0)
                    i--;
        }

        if(command == 'background')
        {
            if(params[1] != NULL)
            {
                printf("Error: Too many arguments passed to 'background'.\n");
                return;
            }
        }
        
        //Getting the number of the parameters
        n = argsLength(params) + 1;

        //Saving the parameters to a new array of the correct size
        char *temp[n];
        while(params[j] != NULL)
        {
            temp[j] = params[j];
            j++;
        }
        //Setting the last value as null; need for execv to work
        temp[n] = NULL;

        //creating a new process
        pid = fork();

        //If fork fails an error is produced
        if(pid == -1)
        {
            printf("Error: Can't fork.\n");
            return;
        }
        else if(pid == 0) //Child process
        {
            //Absolute path to program
            if(command == 'start')
            {
                if(temp[0][0] == '/')
                {
                    if(execvp(temp[0], temp) < 0)
                    {
                        printf("Error: Program could not be executed.\n");
                        return;
                    }
                }
                else //relative path to program
                {
                    //convert relative to absolute path with respect to currentdir
                    char rPath[MAXPATH];
                    strcpy(rPath, currentDir);
                    strcat(rPath, "/");
                    strcat(rPath, temp[0]);

                    if(execv(temp[0], temp) < 0)
                    {
                        printf("Error: Program could not be executed.\n");
                        return;
                    }
                }
            }
            else
            {
                printf("The pid is: %d", (int)getpid());
            }
        }
        else if (pid > 0) //parent process
        {
            //Queues parent until child program exits
            wait(NULL);
            return;
        }
        free(temp);
    }
}

//Strips white spaces from the start and end of strings
char* stripwhite(char *str)
{
  char *end;
  while(isspace((unsigned char)* str)) str++; //trims leading spaces
  end = str+strlen(str)-1;
  while(end>str && isspace((unsigned char)* end)) end--; //trims trailing spaces
  *(end +1) = '\0';
  return (str);
}

void processInput(char* str)
{
    char *command, *args;
    char *str2 = strdup(str);

    //Separating the command from the input
    command = strsep(&str2, " ");

    //Everything that comes after the command, may be null
    args = str2;

    //saving the command to the history list
    // addHistory(command, args);

    //Executing commands
    if(strcmp(command, "movetodir") == 0)
    {
        movetodir(args);
        return;
    }
    else if(strcmp(command, "whereami") == 0)
    {
        whereami();
        return;
    }
    else if(strcmp(command, "history") == 0)
    {
        if (args == NULL)
          history(NULL);
        else
          history(args);

        return;
    }
    else if(strcmp(command, "byebye") == 0)
    {
        byebye(args);
        return;
    }
    else if(strcmp(command, "replay") == 0)
    {
        replay(args);
        return;
    }
    else if(strcmp(command, "start") == 0)
    {
        start_background(command, args);
        return;
    }
    else if(strcmp(command, "background") == 0)
    {
        start_background(command, args);
        return;
    }
    else if(strcmp(command, "dalek") == 0)
    {
        dalek(args);
        return;
    }
    else
    {
        printf("No such command.\n");
    }
}

int main()
{
    char *text, *line;
    createHistory();
    init_shell();

    getcwd(currentDir, sizeof(currentDir));
    strcpy(originalDir, currentDir);

    //Loop to read in input
    while (1)
    {
        do{
            text = readline("#");
        }while(iscntrl(*text)||isspace(*text)); //Loops the readline if input is null

        //stripping leading or trailing white spaces
        line = stripwhite(text);

        //processing the input
        processInput(line);
    }

}

