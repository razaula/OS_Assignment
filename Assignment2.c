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

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

char currentDir[1024];
char originalDir[1024];

// Linked list node
typedef struct List {
  char *command;
  int position;
  struct List *next;
  struct List *prev;
} List;

List *history, *head, *tail;
int length = 0;

void createHistory()
{
  history = malloc(sizeof(List));
  history->command = NULL;
  history->next = NULL;
  history->prev = NULL;
  head = tail = history;
}

void historyList(char* option)
{
  List *temp = head;
  List *temp2;
  int i = 0;

  if (history->next == NULL && history->prev == NULL)
  {
    printf("history list is empty\n");
    return;
  }

  if (option == NULL)
  {
    printf("head: %s\n", head->command);
    while (temp->prev != NULL)
    {
      printf("%d: %s\n", i, temp->command);
      i++;
      temp = temp->next;
    }

    // printf("%d: %s\n", i++, tail->command);
  }
  else if (strcmp(option, "-c") == 0)
  {
    temp = head->next;
    printf("head command: %s\n", head->command);
    while (head != NULL)
    {
      free(head);
      head = temp;
      temp = temp->next;
    }
  }
}

void saveHistory(char* command)
{
  List *addCommand = NULL;

  if (command == NULL)
  {
    printf("Command is empty\n");
    return;
  }

  if (history == NULL)
  {
    printf("\nSystem Error...\n");
    return;
  }
  else if (length == 0)
  {
    addCommand = malloc(sizeof(List));
    addCommand->command = malloc(sizeof(char));
    strcpy(addCommand->command, command);

    tail->next = addCommand;
    addCommand->prev = tail;
    head = tail = addCommand;
    length++;
  }


  addCommand = malloc(sizeof(List));
  addCommand->command = malloc(sizeof(char));
  strcpy(addCommand->command, command);

  tail->next = addCommand;
  addCommand->prev = tail;
  tail = addCommand;
  length++;
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

// Function to take input
int takeInput(char* str)
{
    char* buf;

    buf = readline("\n# ");
    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}

// Function to print Current Directory.
void printDir()
{
    printf("\nDir: %s", currentDir);
}

void printTest()
{
  printf("\nTesting 124...\n");
}

// Function where the system command is executed
void execArgs(char** parsed)
{
    // Forking a child
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}

// Function where the piped system commands is executed
void execArgsPiped(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }

    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } else {
        // Parent executing
        p2 = fork();

        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        }

        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        } else {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

// Help command builtin
void openHelp()
{
    puts("\n***WELCOME TO MY SHELL HELP***"
        "\nCopyright @ Suprotik Dey"
        "\n-Use the shell at your own risk..."
        "\nList of Commands supported:"
        "\n>cd"
        "\n>ls"
        "\n>exit"
        "\n>all other general commands available in UNIX shell"
        "\n>pipe handling"
        "\n>improper space handling");

    return;
}

// Function to execute builtin commands
int ownCmdHandler(char** parsed)
{
    int NoOfOwnCmds = 8, i, switchOwnArg = 0;
    char* ListOfOwnCmds[NoOfOwnCmds];
    char* username;
    char *command;;

    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";
    ListOfOwnCmds[2] = "help";
    ListOfOwnCmds[3] = "hello";
    ListOfOwnCmds[4] = "whereami";
    ListOfOwnCmds[5] = "movetodir";
    ListOfOwnCmds[6] = "history";
    ListOfOwnCmds[7] = "history -c";

    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }

    switch (switchOwnArg) {
    case 1:
        printf("\nGoodbye\n");
        exit(0);
    case 2:
        chdir(parsed[1]);
        command = malloc(sizeof(char));
        strcpy(command, "cd");
        saveHistory(command);
        return 1;
    case 3:
        openHelp();
        command = malloc(sizeof(char));
        strcpy(command, "help");
        saveHistory(command);
        return 1;
    case 4:
        username = getenv("USER");
        printf("\nHello %s.\nMind that this is "
            "not a place to play around."
            "\nUse help to know more..\n",
            username);
        command = malloc(sizeof(char));
        strcpy(command, "hello");
        saveHistory(command);
        return 1;
    case 5:
    	printDir();
      command = malloc(sizeof(char));
      strcpy(command, "whereami");
      saveHistory(command);
    	return 1;
    case 6:
    	moveToDir(parsed[1]);
    	return 1;
    case 7:
      historyList(NULL);
      return 1;
    case 8:
      historyList("-c");
      return 1;
    default:
        break;
    }

    return 0;
}

int moveToDir(char* path)
{
    if (strcmp(path, "og") == 0)
    {
    	strcpy(currentDir, originalDir);
    	printf("Moved back to the original directory, %s", originalDir);
    	return 1;
    }
    else if (strcmp(path, "..") == 0)
    {
    	int counter = 0;
    	int otherCounter = 0;
    	for (int i = 0; i < strlen(currentDir); i++)
    	{
    	   if (currentDir[i] == '/')
    	   {
    	   	counter++;
    	   }
    	}
    	//printf("%d", counter);
    	for (int i = 0; i < strlen(currentDir); i++)
    	{
    	   if (currentDir[i] == '/')
    	   {
    	   	otherCounter++;
    	   }
    	   if (otherCounter == counter)
    	   {
    	   	currentDir[i] = '\0';
    	   }
    	}
    	printf("moving back one directory to %s", currentDir);
    	return 1;
    }
    int slashes = 0;
    for (int i = 0; i < strlen(path); i++)
    {
    	if (path[i] != '/')
    	{
    	    break;
    	}
    	slashes = i;
    }
    for (int i = 0; i < strlen(path)-slashes-1; i++)
    {
    	path[i] = path[i+slashes+1];
    	printf("%s\n", path);
    }

    if (slashes > 0)
    {
    	path[strlen(path)-slashes-1] = '\0';
    }

    int ln = strlen(currentDir);
    if (currentDir[ln-1] != '/')
    {
    	currentDir[ln] = '/';
    	currentDir[ln+1] = '\0';
    }
    char test[1024];
    strcpy(test, currentDir);
    DIR* dir = opendir(strcat(test, path));
    if (dir)
    {
    	strcat(currentDir, path);
    	printf("Moved to %s", currentDir);
    	return 1;
    }
    else if (ENOENT == errno)
    {
    	printf("directory %s does not exist.", path);
    	int newLn = strlen(currentDir);
    	if (currentDir[newLn-1] == '/')
    	{
    	    currentDir[newLn-1] = '\0';
    	}
    	return 0;
    }
    else
    {
    	printf("it failed for some reason.");
    	return 0;
    }
}

// function for finding pipe
int parsePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }

    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else {
        return 1;
    }
}

// function for parsing command words
void parseSpace(char* str, char** parsed)
{
    int i;

    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int processString(char* str, char** parsed, char** parsedpipe)
{

    char* strpiped[2];
    int piped = 0;

    piped = parsePipe(str, strpiped);

    if (piped) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);

    } else {

        parseSpace(str, parsed);
    }

    if (ownCmdHandler(parsed))
        return 0;
    else
        return 1 + piped;
}

int main()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    createHistory();
    init_shell();

    //currentDir = (char *) malloc(1024*sizeof(char*));
    getcwd(currentDir, sizeof(currentDir));
    strcpy(originalDir, currentDir);
    //currentDir = "test";
    printDir();
    //printf("%d", strlen(currentDir));

    while (1) {
        // print shell line

        // take input
        if (takeInput(inputString))
            continue;
        // process
        execFlag = processString(inputString,
        parsedArgs, parsedArgsPiped);
        // execflag returns zero if there is no command
        // or it is a builtin command,
        // 1 if it is a simple command
        // 2 if it is including a pipe.

        // execute
        if (execFlag == 1)
            execArgs(parsedArgs);

        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}
