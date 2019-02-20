
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define clear() printf("\033[H\033[J"); //Used for clear command 
#define BUF_SIZE 1024                   // Used for a buffer size later


int new_cd(char **args);
int new_help(char **args);
int new_exit(char **args);
int new_clr(char **args);
int new_echo(char **args);
int new_environ(char **args);
int new_pause(char **args);


//List of commands that are available in the shell. 
char *commands_str[] = {
  "cd",
  "help",
  "exit",
  "clr",
  "echo",
  "pause",
  "ls",
  "environ"
};

//All the built in function that are available. This is done so more commands can be added easily in the fututre.
int (*command_func[]) (char **) = {
  &new_cd,
  &new_help,
  &new_exit,
  &new_clr,
  &new_echo,
  &new_environ,
  &new_pause
};

//Is the number of commands available
int num_builtins() {
  return sizeof(commands_str) / sizeof(char *);
}


//Implementation of the cd command
int new_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "An argument is expected to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("error");
    }
  }
  return 1;
}


//Implementation of the help command
int new_help(char **args)
{
  int i;
  printf("The available commands are:\n");

  for (i = 0; i < num_builtins(); i++) {
    printf("  %s\n", commands_str[i]);
  }
  return 1;
}


//Implementation of exit command
int new_exit(char **args)
{
  return 0;
}


//Implementation of clr command
int new_clr(char **args){
  clear();
  return 0;

}

//Implementation of echo command 
int new_echo(char **args){
  int i;
  printf("%lu\n", sizeof(args)/sizeof(int) );
  for(i=1; i<100; i++){
  //   printf("%s\n", args );
  //   printf("%s ", args[i]);
  }



//Implementation of environ command
int new_environ(char **args){
 while(1){
      char c = getchar();
      if(c == 0x0A){
        break;
      }
    }
}


//Implementation of pause command
int new_pause(char **args){
  while(1){
      char c = getchar();
      if(c == 0x0A){
        break;
      }
    }
}


//Launch is used to differentiate between parent process and child process, so that any changes made to the child process is also changed in the parent process
int launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    int output = execvp(args[0], args);
    if (output == -1) {
      printf("%s\n", args[0]);
      perror("error");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("error");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}


//Used to execute the command after the seperation of the command from the arguments
int execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], commands_str[i]) == 0) {
      return (*command_func[i])(args);
    }
  }

  return launch(args);
}



//Used to read every line entered by the user
char *read_line(void)
{
  int bufsize = BUF_SIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += BUF_SIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}


//Used to split every line that is inputted into commands and arguments for the commands.
char **split_line(char *line)
{
  int bufsize = 64, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "Allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, " \n\r\t\a");
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += 64;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, " \n\r\t\a");
  }
  tokens[position] = NULL;
  return tokens;
}


//Runs a loop over and over again to keep taking inputs from the users. 
void loop(void)
{
  char *inp;
  char **args;
  int stat;
  char buf[BUFSIZ];
  char *cp;

  cp = getcwd(buf, sizeof(buf));      //Used to show the directory as a command line prompt
  // printf("Current dir: %s\n", buf)

  do {
    printf("%s> ", buf);
    inp = read_line();
    args = split_line(inp);
    stat = execute(args);

    free(inp);
    free(args);
  } while (stat);
}



int main(int argc, char **argv)
{
   char buf[BUFSIZ];
  char *cp;

  cp = getcwd(buf, sizeof(buf));
  // printf("Current dir: %s\n", buf);

  // Run command loop.
  loop();


  return EXIT_SUCCESS;
}

