/**
 * shell.c
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell_tests.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/* Your stuff here */
#define true 1
#define false 0
#define MAX_INPUT_SIZE 100

int main (int argc, char** argv) {
    /* Declare variables here */
	int pid = getpid();
	int command_num = 1;
	int child_pid;
	int arg_num = 0;
	int status;

    /* Entry point for the testrunner program */
    /* DO NOT DELETE */
    if ((argc > 1) && !strcmp(argv[1], "-test")) {
        run_shell_tests(argc - 1, argv + 1);
        return EXIT_SUCCESS;
    }
    /* Your code here */
	while(true) {
		fprintf(stdout, "Shell(pid=%d)%d> ", pid, command_num);
		char* input = malloc(MAX_INPUT_SIZE);
		char** args = malloc(MAX_INPUT_SIZE);

		// Use fgets for buffer overflow protection
		fgets(input, MAX_INPUT_SIZE, stdin);

		// Parse out the commands and the arguments
		input = strtok(input, " \t\n");
		while(input != NULL) {
			args[arg_num] = input;
			arg_num++;
			input = strtok(NULL, " \t\n");
		}
		if(args[0] == NULL) {
			command_num++;
			arg_num = 0;
			free(input);
			free(args);
			continue;
		}
		// Check for exit command
		if(!strcmp(args[0], "exit")) {
			return EXIT_SUCCESS;
		}

		child_pid = fork();
		if(child_pid == 0) {
			fprintf(stdout, "Parent says 'child process has been forked with pid=%d'\n", getpid());
			execv(args[0], args);
			fprintf(stderr, "Launch: error executing command: '%s'\n", args[0]);
			return EXIT_FAILURE;
		}
		else if(child_pid < 0) {
			fprintf(stderr, "Launch: error while forking\n");
			return EXIT_FAILURE;
		}

		child_pid = wait(&status);
		if(child_pid < 0) {
			fprintf(stderr, "Launch: error while waiting for child to terminate\n");
			return EXIT_FAILURE;
		}
		else {
			fprintf(stdout, "Parent says 'wait() returned so the child with pid=%d is finished.'\n", child_pid);
		}
		//fprintf(stdout, "your input is = (%s)\n", input);
		command_num++;
		arg_num = 0;
		free(input);
		free(args);
	}

    return EXIT_SUCCESS;
}
