#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include "launch_tests.h"

#define MAX_LINE_LEN 256
#define MAX_ARGS (MAX_LINE_LEN/2)
#define WHITESPACE " \t\n"

typedef struct {		/* Struct to contain a parsed command */
    char* name;
    int argc;
    char* argv[MAX_ARGS+1];
} command_t;

/* Given a command line and a pointer to an empty command_t structure,
 * this function will parse the command line and set values for name,
 * argc, and argv.
 * Preconditions:
 * * cmdline is null-terminated.
 * * strlen(cmdline) < MAX_LIN_LENGTH
 * Postconditions:
 * * 0 <= argc <= MAX_ARGS
 * * argc is the number of "tokens" or words on the command line
 * * argv[argc] is a null pointer
 * * argv[0] through argv[argc-1] are pointers to those tokens
 * * name = argv[0]
 */
void parse_command(char *cmdline, command_t *cmd)
{
    int argc = 0;
    char* word;

    /* Fill argv. */
    word = strtok(cmdline, WHITESPACE);
    while (word) {
        cmd->argv[argc] = (char *) malloc(strlen(word)+1);
		strcpy(cmd->argv[argc], word);
		word = strtok(NULL, WHITESPACE);
		argc++;
    }
    cmd->argv[argc] = NULL;

    /* Set argc and the command name. */
    cmd->argc = argc;
	if(cmd->argv[0] == NULL) {

	}
    cmd->name = (char *) malloc(strlen(cmd->argv[0])+1);
    strcpy(cmd->name, cmd->argv[0]);
}

/* Frees dynamically allocated strings from a command. */
void free_command(command_t *cmd) {
    int i;
    for (i=0; ((i < cmd->argc) && (cmd->argv[i] != NULL)); i++) {
        free(cmd->argv[i]);
    }
    free(cmd->name);
}

/* Given the name of a file containing a list of commands,
 * executes the commands in the file.
 * Returns EXIT_SUCCESS.
 */
int launch_commands(const char *filename) {
    int i;
    int pid, num_children;
    int status;
    FILE* file;
    char cmdline[MAX_LINE_LEN];
    command_t command;

    /* Open the file that conains the set of commands. */
    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "launch: Error opening file %s\n", filename);
	return EXIT_FAILURE;
    }

    /* Report the process id. */
    printf("launch: Parent process id is %d\n", getpid());

    /* Process each command in the launch file. */
    num_children = 0;
    while (fgets(cmdline, MAX_LINE_LEN, file)) {
		if(!strcmp(cmdline, "\n")) {
			continue;
		}
        parse_command(cmdline, &command);

		/* Create a child process to execute the command. */
		pid = fork();
		if (pid == 0) {
			/* The child executes the command. */
			execv(command.name, command.argv);
			fprintf(stderr, "launch: Error executing command '%s'\n",
						command.name);
				return EXIT_FAILURE;
		} else if (pid < 0) {
				fprintf(stderr, "launch: Error while forking\n");
			return EXIT_FAILURE;
		}

		/* if pid > 0, then this is the parent process and there was
		 * no error.  The parent reports the pid of the child process.
		 */
		printf("launch: Forked child process %d with command '%s'\n",
			   pid, command.name);
		num_children++;

		/* The parent frees dynamically allocated memory in the
			 * command data structure and continues to the next command.
		 */
		free_command(&command);
    }
    if (ferror(file)) {
        fprintf(stderr, "launch: Error while reading from file\n");
		return EXIT_FAILURE;
    }

    /* The parent closes the file. */
    fclose(file);
    printf("launch: Launched %d commands\n", num_children);

    /* The parent terminates after all children have terminated. */
    for (i=0; i < num_children; i++) {
        pid = wait(&status);
		if (pid < 0) {
			fprintf(stderr,
						"launch: Error while waiting for child to terminate\n");
			return EXIT_FAILURE;
		} else  {
				printf("launch: Child %d terminated\n", pid);
		}
    }
    printf("launch: Terminating successfully\n");
    return EXIT_SUCCESS;
}


int main(int argc, char* argv[]) {
    /* Entry point for the testrunner program */
    if (argc > 1 && !strcmp(argv[1], "-test")) {
        run_launch_tests(argc - 1, argv + 1);
        return EXIT_SUCCESS;
    }

    /* Read the command-line parameters for this program. */
    if (argc != 2) {
        fprintf(stderr, "Usage: launch <launch_set_filename>\n");
	return EXIT_FAILURE;
    }

    /* Engage! */
    return launch_commands(argv[1]);
}
