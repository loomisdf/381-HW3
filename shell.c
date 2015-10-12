/**
 * Team 12:
 * Daltin Loomis - loomisdf
 * Evan Baker - bakerej4
 * Chris Schipper - schippjc
 * CSE 381
 * 10/12/15
 * Shell.c, HW#3
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell_tests.h"

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

		// args is for all the arguments passed in by the command line
		char* input = malloc(MAX_INPUT_SIZE);
		char** args = malloc(MAX_INPUT_SIZE);

		// Use fgets for buffer overflow protection
		char* exit_sig = fgets(input, MAX_INPUT_SIZE, stdin);

		if(exit_sig == NULL) {
			printf("\n");
			free(input);
			free(args);
			printf("Exiting process %d\n", getpid());
			return EXIT_SUCCESS;
		}

		// Parse out the commands and the arguments
		input = strtok(input, " \t\n");
		while(input != NULL) {
			args[arg_num] = input;
			arg_num++;
			input = strtok(NULL, " \t\n");
		}

		// ignore spaces and newlines
		if(args[0] == NULL) {
			arg_num = 0;
			free(input);
			free(args);
			continue;
		}

		// Check for exit command
		if(!strcmp(args[0], "exit")) {
			printf("Exiting process %d\n", getpid());
			return EXIT_SUCCESS;
		}

		//Check for cd
		if(!strcmp(args[0], "cd")) {
			if(arg_num > 2) {
				arg_num = 0;
				command_num++;
				free(input);
				free(args);
				fprintf(stderr, "cd: Too many arguments");
				continue;
			}
			// if cd has the right # of args
			else if(arg_num == 2) {
				chdir(args[1]);
				arg_num = 0;
				command_num++;
				free(input);
				free(args);
				continue;
			}
			// Look for home directory
			else if(arg_num == 1) {
				char* home;
				home = getenv("HOME");
				chdir(home);
				arg_num = 0;
				command_num++;
				free(input);
				free(args);
				continue;
			}
		}

		// Create a new process to execute the command
		child_pid = fork();
		if(child_pid == 0) {
			fprintf(stdout, "  Parent says 'child process has been forked with pid=%d'\n", getpid());
			execvp(args[0], args);
			fprintf(stderr, "Launch: error executing command: '%s'\n", args[0]);
			return EXIT_FAILURE;
		}
		else if(child_pid < 0) {
			fprintf(stderr, "Launch: error while forking\n");
			return EXIT_FAILURE;
		}
		child_pid = waitpid(0, &status, 0);
		if(status != EXIT_SUCCESS) {
			fprintf(stderr, "shell: Process %d exited with status %d\n", child_pid, status);
		} else {
			if(child_pid < 0) {
				fprintf(stderr, "Launch: error while waiting for child to terminate\n");
				return EXIT_FAILURE;
			}
			else {
				fprintf(stdout, "  Parent says 'wait() returned so the child with pid=%d is finished.'\n", child_pid);
			}
		}
		command_num++;
		arg_num = 0;
		free(input);
		free(args);
	}

    return EXIT_SUCCESS;
}
