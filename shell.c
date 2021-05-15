// Shell starter file 
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/wait.h>
#include <errno.h>
#include "array.h"
#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10

char history[HISTORY_DEPTH][COMMAND_LENGTH];
char cmd_transfer[COMMAND_LENGTH];
int num = 0;
int sig_flag = 0;
int flag_num = 0;
int find_m = 0;


/**
 * Command Input and Processing  
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens. 
 */
int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;

	int num_chars = strnlen(buff, COMMAND_LENGTH);
	char buff_tmp[COMMAND_LENGTH+1];
	strcpy(buff_tmp, buff);

	for (int i = 0; i < num_chars; i++) {
	
		switch (buff_tmp[i]) {
		// Handle token delimiters (ends):
		case ' ': 
		case '\t':
		case '\n':
			buff_tmp[i] = '\0';
			in_token = false;
			break;
		
		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff_tmp[i];
				token_count++;
				in_token = true;
			}
		}

	}

	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff,char* buff_history, char *tokens[], _Bool *in_background, int *flag)
{	
	int length = 0;
	*in_background = false;

	//if the the command is the previous one, copy the length of previous command
	if(*flag == 1){
		length = strlen(buff);
		*flag = 0;
	}
	//else, read the new command and find its length
	else{
		length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);
	}
	//if it is invalid length, return error message
	if ((length < 0) && (errno != EINTR)) {
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(-1);
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}

	if(find_m == 0 || find_m -1 <= 0)
	{
		strcpy(buff_history, buff);
	}
	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);

	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
	//add_flag is used to check if there has a previous command
	int* add_flag = NULL;
	add_flag = &flag_num;
	//if previous command, add to history
	if (*add_flag == 1)
		{
			array_cpy(buff_history, history, num);
			*add_flag = 0;
			num++;
		}

}

void handle_SIGINT(){ //when users press ctrl+C
	write(STDOUT_FILENO, "\nHere is a list of supported internal demands: \n'cd' is a built-in command for changing the current working directory. \n'pwd' displays the current working directory. \n'history' allows the user access up to the 10 most recently entered commands. \n'exit' will exit the shell program. \n'help' will display all supported internal commands with brief explanation. \n", strlen("\nHere is a list of supported internal demands: \n'cd' is a built-in command for changing the current working directory. \n'pwd' displays the current working directory. \n'history' allows the user access up to the 10 most recently entered commands. \n'exit' will exit the shell program. \n'help' will display all supported internal commands with brief explanation. \n"));
	sig_flag = 1;
}

/*
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{
	
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	char path[COMMAND_LENGTH];
		/* set up the signal handler,
		   copied directly from the provided Guide to signal page on the assignment page 
		*/
		struct sigaction handler;
		handler.sa_handler = handle_SIGINT;
		handler.sa_flags = 0;
		sigemptyset(&handler.sa_mask);
		sigaction(SIGINT, &handler, NULL);
		
		int flag = 0;

	while (true) {

		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().
		
		getcwd(path, COMMAND_LENGTH); //char *getcwd(char *buf, size_t size);
		strcat(path, "$ ");
		write(STDOUT_FILENO, path, strlen(path));
		_Bool in_background = false;

		char buffer_history[COMMAND_LENGTH];
		read_command(input_buffer,buffer_history, tokens, &in_background, &flag);

		if(sig_flag == 1){
			sig_flag = 0; //change sig_flag back
			continue;
		}
		if(!tokens[0]){ //if user did not enter anything
			continue; 
		}

		/*
			if the command contains symbols and words, return 0
			if the command only contains numbers, return this number
		*/
		int val = atoi(&tokens[0][1]);// no !n
		int tok = tokens[0][0]; //no !
		//exclude any commands with !
		 if (strcmp(tokens[0], "!!") != 0){
		    if ((tok != 33))
			{
				//add valid commands in the array
				array_cpy(buffer_history, history, num);
				//increase number of commands
				num++;
			}
		
		}
	
		/*
		// DEBUG: Dump out arguments:
		for (int i = 0; tokens[i] != NULL; i++) {
			write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
			write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
		if (in_background) {
			write(STDOUT_FILENO, "Run in background.\n", strlen("Run in background.\n"));
		}
		
		*/
		
//=================================================================================================================================

		/*part 2: internal commands: exit, pwd, cd, help. 
		  Internal commands are built-in features of the shell itself, 
		  	as opposed to a separate program that is executed. 
		  These do not need to fork a new process.
		*/	

//!! -----------------------------------------------------------------------------------------------------------------------
	
	//identify the "!!" command
	if (strcmp(tokens[0], "!!") == 0){
		//num >=1: be sure that there is at least one command
		if (num >= 1){
		flag_num = 1;
		int pos = num-1;
		//get the target command from history array
		char *prev_cmd = run_n_cmd (pos,history);
		strcpy(buffer_history, prev_cmd);
		flag = 1;
		//run the previous command again
		read_command(prev_cmd, buffer_history, tokens, &in_background, &flag);
	}
	//if there is no previous command, return error message
	else if (num <= 0){
		write(STDOUT_FILENO, "Error: No previous command availabile. \n", strlen(" Error: No previous command availabile. \n"));
		continue;
	}
}

//!n -----------------------------------------------------------------------------------------------------------------------
	
	//find the command with "!n"
	if (tok == 33)
	{
		//base case
		if(val != 0){
			int index = atoi(&tokens[0][1]);
			//return the error message
			if (index <= 0)
			{
				write(STDOUT_FILENO, "Error: invalid value.\n", strlen("Error: invalid value.\n"));
				continue;
			}
			//return the error message
			else if (index > num)
			{
				write(STDOUT_FILENO, "Error: invalid value.\n", strlen("Error: invalid value.\n"));
				continue;
			}
			//if the input number is not on the history list, return the error message
			else if ((num-index) >= 10){
				write(STDOUT_FILENO, "Error: invalid value.\n", strlen("Error: invalid value.\n"));
				continue;
	
			}
			else{
				flag_num = 1;
				//find the target command
				char* cmd_n = run_n_cmd((index-1), history);
				strcpy(buffer_history, cmd_n);
				flag = 1;
				//run the command again
				read_command(cmd_n, buffer_history, tokens, &in_background, &flag);
			}
		}
			
	}

	//exit---------------------------------------------------------------------------------------------------------------------											   
		
		if(strcmp(tokens[0], "exit") == 0){
			if(tokens[1]){ //if there is an argument, display error msg
				write(STDOUT_FILENO, "Error: exit does not take any arguments\n", strlen("Error: exit does not take any arguments\n"));
			}else{
				exit(0); //exit the shell
			}
			continue;
		}

	//pwd----------------------------------------------------------------------------------------------------------------------

		if(strcmp(tokens[0], "pwd") == 0){ 
			if(tokens[1]){  //if there is an argument, display error msg
				write(STDOUT_FILENO, "Error: pwd does not take any arguments\n", strlen("Error: pwd does not take any arguments\n"));
			}else{
				char pwd[COMMAND_LENGTH]; 
				getcwd(pwd, COMMAND_LENGTH); //getcwd returns the path
				write(STDOUT_FILENO, pwd, strlen(pwd)); //display path
				write(STDOUT_FILENO, " \n", strlen(" \n"));
			}
			continue; //jump to the next iteration
		}

	//cd-----------------------------------------------------------------------------------------------------------------------

		if(strcmp(tokens[0], "cd") == 0){
			/*
			if(tokens[1]){
				
				write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
			}
			*/
			//BONUS
			/*
			char *homedir; //pointer to store the directory path for home
			struct passwd *pw; //create a pointer of struct passwd

					
					The passwd structure is defined in <pwd.h> as follows:
						struct passwd {
						    char   *pw_name;       // username 
						    char   *pw_passwd;     // user password 
						    uid_t   pw_uid;        // user ID 
						    gid_t   pw_gid;        // group ID 
						    char   *pw_gecos;      // user information
						    char   *pw_dir;        // home directory 
						    char   *pw_shell;      // shell program 
						};
					


			pw = getpwuid(getuid()); getuid() returns the real user ID of the calling process, 
						   which is then used to return a pointer containing the broken-out fields of
						   record in the password database that matches the user ID uid
						 

			homedir = pw->pw_dir; //pw_dir is home directory
			*/
			//write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
			
			if(!tokens[1]){ //if no argument, return to home directory
				
				if(chdir(getenv("HOME")) != 0){
					write(STDOUT_FILENO, "Error: home directory not found \n", strlen("Error: home directory not found \n"));
				}else{ //update OLDPWD and PWD
					getcwd(path, COMMAND_LENGTH);
					setenv("OLDPWD", getenv("PWD"), 1); //set environment: int setenv(const char *name, const char *value, int overwrite)     
					
					setenv("PWD", path, 1);
				}
			}

			if (tokens[1]){	//if user entered arguments			
				int flag = -1; //use flag to determine the success of chdir
				//if(chdir(tokens[1]) != 0){ //chdir returns 0 if succeed, 1 if failed
				
					/*
					BONUS: if tokens[1] contains '~' and '/', then remove '~' and '/' from tokens[1],
					       chdir to homedir, then chdir to the modified tokens[1] which should be a valid dir
					*/
				//write(STDOUT_FILENO, "check\n", strlen("check\n"));
				//write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
				if(strcmp(tokens[1], "~") == 0){ //if cd ~, then  cd to home
					flag = chdir(getenv("HOME")); //cd to home
				}else if(strcmp(tokens[1], "-") == 0){ //if cd -, then cd to last working directory
					//write(STDOUT_FILENO, "$OLDPWD is: ", strlen("$OLDPWD is: "));
					//write(STDOUT_FILENO, getenv("OLDPWD"), strlen(getenv("OLDPWD")));
					flag = chdir(getenv("OLDPWD"));

				}else if((strstr(tokens[1], "~") != NULL) && (strstr(tokens[1], "/") != NULL)){ 
					//write(STDOUT_FILENO, "check\n", strlen("check\n"));
					memmove(tokens[1], tokens[1]+1, strlen(tokens[1])); //use memmove to remove the first two characters '~/'	
					strcat(getenv("HOME"), tokens[1]); //concatnate homedir and tokens[1]
					//write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
					//write(STDOUT_FILENO, "check \n\n", strlen("check \n\n"));
					flag = chdir(getenv("HOME"));
				}else{
					//write(STDOUT_FILENO, "check\n", strlen("check\n"));
					flag = chdir(tokens[1]);
				}
				if(flag == -1){ //chdir failed
					write(STDOUT_FILENO, "Error: no such file or directory\n", strlen("Error: no such file or directory\n")); //display error whn chdir failed
				}else{ //chdir succeed, update pwd and OLDPWD
					char update_path[COMMAND_LENGTH];
					getcwd(update_path, COMMAND_LENGTH);
					setenv("OLDPWD", getenv("PWD"), 1);
					setenv("PWD", update_path, 1);
				
				}
			}

			continue; //jump to next iteration
	
		}

	//help--------------------------------------------------------------------------------------------------------------------- 

		//0 arg - list all supported internal commands. 2 arg: display error message
		if(strcmp(tokens[0], "help") == 0){
			if(tokens[1] && !tokens[2]){ //if there is only one input, display the corresponding help
				if(strcmp(tokens[1], "cd") == 0){ //help cd case
					write(STDOUT_FILENO, "'cd' is a built-in command for changing the current working directory. \n", strlen("'cd' is a built-in command for changing the current working directory. \n"));
				}else if(strcmp(tokens[1], "pwd") == 0){ //help pwd case
					write(STDOUT_FILENO, "'pwd' is a built-in command for displaying the current working directory. \n", strlen("'pwd' is a built-in command for displaying the current working directory. \n"));
				}else if(strcmp(tokens[1], "exit") == 0){ //help exit
					write(STDOUT_FILENO, "'exit' is a built-in command for exiting the shell program. \n", strlen("'exit' is a built-in command for exiting the shell program. \n"));
				}else if(strcmp(tokens[1], "help") == 0){
					write(STDOUT_FILENO, "'help' will display all supported internal commands with brief explanation. \n", strlen("'help' will display all supported internal commands with brief explanation. \n"));
				}else{ //unknown internal command
					write(STDOUT_FILENO, "'", strlen("'"));
					write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
					write(STDOUT_FILENO, "'", strlen("'"));
					write(STDOUT_FILENO, " is an external command or application. \n", strlen(" is an external command or application. \n"));
				}
			}else if(!tokens[1]){ //if there is no argument, display all internal commands and their description
				write(STDOUT_FILENO, "Here is a list of supported internal demands: \n'cd' is a built-in command for changing the current working directory. \n'pwd' displays the current working directory. \n'history' allows the user access up to the 10 most recently entered commands. \n'exit' will exit the shell program. \n'help' will display all supported internal commands with brief explanation. \n", strlen("Here is a list of supported internal demands: \n'cd' is a built-in command for changing the current working directory. \n'pwd' displays the current working directory. \n'history' allows the user access up to the 10 most recently entered commands. \n'exit' will exit the shell program. \n'help' will display all supported internal commands with brief explanation. \n"));
			}else{ //if there are more than 1 argument, display error msg
				write(STDOUT_FILENO, "Error: help only takes one argument. \n", strlen("Error: help only takes one argument. \n"));	
			}
			continue; //jump to next iteration
		}
//history-----------------------------------------------------------------------------------------------------------------------
   
    if (strcmp(tokens[0], "history") == 0) {
      array_display(history, num);
      continue;
    }

//PART 1-----------------------------------------------------------------------------------------------------------------------
		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */

		//if user entered commands aren't internal commands, fork a child and run the command
		pid_t pid = fork(); //fork a child
		if(pid == -1){
			write(STDOUT_FILENO, "Failed to Fork\n", strlen("Failed to Fork\n")); //write(int fildes, const void *buf, size_t nbytes);
			exit(1);
		}


		if(pid == 0){ //child
			if(execvp(tokens[0], tokens) == -1){ //execvp returns -1 on error
				write(STDOUT_FILENO, "Error: unknown command: ", strlen("Error: unknown command: ")); //display error message
				write(STDOUT_FILENO, tokens[0], strlen(tokens[0]));
				write(STDOUT_FILENO, " \n", strlen(" \n"));
				exit(1); //exit the child process
			}
			
		}else{
			if(in_background == false){ //if not in_background 
				int status;
				waitpid(pid, &status, 0); //wait for child process
					
			}
     		//copied from assignment page
			//Cleanup any previously exited background child processes
			//(The zombies)
			while(waitpid(-1, NULL, WNOHANG)>0)
				; //do nothing

		}	

	}
	return 0;
}
