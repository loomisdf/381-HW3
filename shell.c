/**
 * shell.c
 **/

#include <stdlib.h>
#include <string.h>
#include "shell_tests.h"

/* Your stuff here */

int main (int argc, char** argv) {
    /* Declare variables here */

    /* Entry point for the testrunner program */
    /* DO NOT DELETE */
    if ((argc > 1) && !strcmp(argv[1], "-test")) {
        run_shell_tests(argc - 1, argv + 1);
        return EXIT_SUCCESS;
    }
    /* Your code here */

    return EXIT_SUCCESS;
}
