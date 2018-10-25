#define _XOPEN_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h> 
#include <stdbool.h> 
#include <errno.h>
#include <time.h>
#include<sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);


// Global functions and Variables
extern int  currentln;
extern bool ingnoreFailure;
extern bool printRepresentation;
extern bool printCommand;
extern bool silenceExe;


