/*************** YOU SHOULD NOT MODIFY ANYTHING IN THIS FILE ***************/

#define _GNU_SOURCE
#include <stdio.h>
#undef _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "testrunner.h"
#include "launch_tests.h"

#define quit_if(cond) do {if (cond) exit(EXIT_FAILURE);} while(0)

/* Prepare input, reroute file descriptors, and run the program. */
void run_test(const char *input, int argc, char **argv)
{
    /* Prepare input */
    FILE *in = fopen("test.in", "w");
    FILE *test = fopen("test_launch_blanks", "w");
    fprintf(in, input);
    fclose(in);
    fprintf(test, "/bin/echo test\n\n   \n	\n");
    fclose(test);
    /* Reroute standard file descriptors */
    freopen("test.in",  "r", stdin );
    freopen("test.out", "w", stdout);
    freopen("test.err", "w", stderr);
    /* Run the program */
    quit_if(main(argc, argv) != EXIT_SUCCESS);
    fclose(stdout);
    fclose(stderr);
}

/* Part A: Test of handling empty lines. */
int test_blanks(int argc, char **argv)
{
    char* args[] = {"./launch", "test_launch_blanks"};
    run_test("", 2, args);
    return EXIT_SUCCESS;
}

/*
 * Main entry point for this test harness
 */
int run_launch_tests(int argc, char **argv) 
{
    /* Tests can be invoked by matching their name or their suite name or
     * 'all'
     */
    testentry_t tests[] = {
            { "blanks",	"launch-a",	test_blanks},
    };
    int result = run_testrunner(argc, argv, tests,
                                sizeof(tests)/sizeof(testentry_t));
    unlink("test.in");
    unlink("test.out");
    unlink("test.err");
    unlink("test_launch_blanks");
    return result;
}
