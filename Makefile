CC = gcc
CCOPTS = -c -g -Wall
LINKOPTS = -g

all: launch shell

launch: launch.o launch_tests.o testrunner.o
	$(CC) $(LINKOPTS) -o $@ $^

launch.o: launch.c
	$(CC) $(CCOPTS) -o $@ $<

shell: shell.o shell_tests.o testrunner.o
	$(CC) $(LINKOPTS) -o $@ $^

shell.o: shell.c
	$(CC) $(CCOPTS) -o $@ $<

testrunner.o: testrunner.c testrunner.h
	$(CC) $(CCOPTS) -o $@ $<

launch_tests.o: launch_tests.c testrunner.h
	$(CC) $(CCOPTS) -o $@ $<

shell_tests.o: shell_tests.c testrunner.h
	$(CC) $(CCOPTS) -o $@ $<

test: launch shell 
	./launch -test -f7 all
	./shell -test -f7 all

clean:
	rm -rf *.o launch launch.exe shell shell.exe *~
