#ifndef TUSH
#define TUSH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>

/* 최대 글자 수, 최대 명령어 갯수 설정 */
#define BUF_SIZE 400
#define MAX_ARGS 15
#define MAX_PATHS 100

#define NORMAL_MODE 1
#define BACKGROUND_MODE 2
#define BATCH_MODE 3
#define REDIRECTION_MODE 4
#define PARALLEL_MODE 5

#define REDIRECTION_INPUT 1 
#define REDIRECTION_OUTPUT 2
#define REDIRECTION_APPEND 3

/* tush.c */
void error_handling(char *message);
void printArgs(char**args, char* name);

/* inarg.c */
int getArgsCount(char **args);
char getLastCharacter(char* str);
char* trim_left(char* str);
char* trim_right(char* str);
char* trim(char* str);
void specialSplit(char *input, char **commands, char *special);

/* runcommand.c */
int accessiblePathIndex(char *command);
void process_run(char **args, int mode, int redirection_mode);
int isRedirectionMode(char *input);
void redirectionSplit(char *input, char **args, int redirection_mode);
void batchMode(char **argv);

/* internalcommand.c */
int isInternalMode(char **args);
void internalCommandRun(char **args);
void changePath(char **args);

#endif