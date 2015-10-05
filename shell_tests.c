/*************** YOU SHOULD NOT MODIFY ANYTHING IN THIS FILE ***************/

#define _GNU_SOURCE
#include <stdio.h>
#undef _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "testrunner.h"
#include "shell_tests.h"

#define quit_if(cond) do {if (cond) exit(EXIT_FAILURE);} while(0)

/* Prepare input, reroute file descriptors, and run the program. */
void run_test(const char *input, int argc, char **argv)
{
    /* Prepare input */
    FILE *in = fopen("test.in", "w");
    fprintf(in, input);
    fclose(in);
    /* Reroute standard file descriptors */
    freopen("test.in",  "r", stdin );
    freopen("test.out", "w", stdout);
    freopen("test.err", "w", stderr);
    /* Run the program */
    quit_if(main(argc, argv) != EXIT_SUCCESS);
    fclose(stdout);
    fclose(stderr);
}

/* Part B: Test of basic shell. */
int test_basic(int argc, char **argv)
{
    char *args[] = { "./shell", NULL };
    FILE *out;
    int pid_tmp;
    /* Run the test */
    run_test("/bin/echo hello\n/bin/echo world!\nexit\n", 1, args);
    out = fopen("test.out", "r");
    quit_if(fscanf(out, "hello\n"
                        "world!\n"
                        "Shell(pid=%d)> "
                        "Shell(pid=%d)> "
                        "Shell(pid=%d)> ",
                   &pid_tmp, &pid_tmp, &pid_tmp) != 3);
    fclose(out);
    return EXIT_SUCCESS;
}

/* Part B: Test of handling empty lines. */
int test_blanks(int argc, char **argv)
{
    char *args[] = { "./shell", NULL };
    /* Run the test */
    run_test("/bin/echo hello\n\n   \nexit\n", 1, args);
    /* No need to check output: it will SEGFAULT if broken */
    return EXIT_SUCCESS;
}

/* Part C: Test of using the PATH. */
int test_path(int argc, char **argv)
{
    char *args[] = { "./shell", NULL };
    FILE *out;
    char username[10];
    int pid_tmp;
    /* Run the test */
    run_test("echo hello\necho world!\nwhoami\nexit\n", 1, args);
    out = fopen("test.out", "r");
    quit_if(fscanf(out, "hello\n"
                        "world!\n"
                        "%s\n"
                        "Shell(pid=%d)> "
                        "Shell(pid=%d)> "
                        "Shell(pid=%d)> ",
                   username, &pid_tmp, &pid_tmp, &pid_tmp) != 4);
    fclose(out);
    return EXIT_SUCCESS;
}

/* Part C: Test of cd. */
int test_cd(int argc, char **argv)
{
    char *args[] = { "./shell", NULL };
    char *old_dir_name, *new_dir_name;
    int result;
    /* Run the test */
    old_dir_name = get_current_dir_name();
    run_test("cd /usr\nexit\n", 1, args);
    new_dir_name = get_current_dir_name();
    result = strcmp("/usr", new_dir_name);
    chdir(old_dir_name);
    free(old_dir_name);
    free(new_dir_name);
    quit_if(result);
    return EXIT_SUCCESS;
}

/* Part C: Test of cd with no parameters. */
int test_cdhome(int argc, char **argv)
{
    char *args[] = { "./shell", NULL };
    char *old_dir_name, *new_dir_name;
    int result;
    setenv("HOME", "/", 0);
    /* Run the test */
    old_dir_name = get_current_dir_name();
    run_test("cd\nexit\n", 1, args);
    new_dir_name = get_current_dir_name();
    result = strcmp(getenv("HOME"), new_dir_name);
    chdir(old_dir_name);
    free(old_dir_name);
    free(new_dir_name);
    quit_if(result);
    return EXIT_SUCCESS;
}

/* Part C: Test of cd -. */
int test_cddash(int argc, char **argv)
{
    char *args[] = { "./shell", NULL };
    char *old_dir_name, *new_dir_name;
    int result;
    /* Run the test */
    old_dir_name = get_current_dir_name();
    run_test("cd /usr\ncd -\nexit\n", 1, args);
    new_dir_name = get_current_dir_name();
    result = strcmp(old_dir_name, new_dir_name);
    chdir(old_dir_name);
    free(old_dir_name);
    free(new_dir_name);
    quit_if(result);
    return EXIT_SUCCESS;
}

/* Part C: Test of error on unsuccessful exit*/
int test_error(int argc, char **argv)
{
    char *args[] = { "./shell", NULL };
    FILE *err;
    int pid_tmp, status_tmp;
    /* Run the test - no error */
    /* Run the test - with error */
    run_test("/bin/ls -z \nexit\n", 1, args);
    err = fopen("test.err", "r");
    quit_if(fscanf(err, 
                   "/bin/ls: invalid option -- 'z'\n"
                   "Try `/bin/ls --help' for more information.\n"
                   "shell: Process %d exited with status %d\n",
                   &pid_tmp, &status_tmp) != 2);
    fclose(err);
    return EXIT_SUCCESS;
}

/*
 * Main entry point for this test harness
 */
int run_shell_tests(int argc, char **argv) 
{
    /* Tests can be invoked by matching their name or their suite name or
     * 'all'
     */
    testentry_t tests[] = {
            { "basic",	"shell-b",	test_basic},
	    { "blanks", "shell-b",	test_blanks},
	    { "path", 	"shell-c",	test_path},
	    { "cd", 	"shell-c",	test_cd},
	    { "cdhome", "shell-c",	test_cdhome},
	    { "error", 	"shell-c",	test_error},
    };
    int result = run_testrunner(argc, argv, tests,
                                sizeof(tests)/sizeof(testentry_t));
    unlink("test.in");
    unlink("test.out");
    unlink("test.err");
    return result;
}
