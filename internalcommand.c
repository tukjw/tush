#include "tush.h"

extern char* paths[MAX_PATHS];

/* 내장명령어 인지 확인 */
int isInternalMode(char **args){
    char *internalCommands[3] = {"cd", "exit", "path"};
    for(int i=0;i<3;i++){
        if (!strcmp(args[0], internalCommands[i])) return 1;
    }
    return 0;
}


/* 내장명령어 실행 */
void internalCommandRun(char **args){

    /* chdir 참고 : https://hackingis.art/30 */
    if(!strcmp(args[0], "cd")){
        /* 인자가 없거나 ~(틸드) 일 경우 "HOME" 환경변수를 읽어와 홈 디렉토리로 이동 */
        if (args[1] == NULL || !strcmp(args[1], "~")) {
            char *home = getenv("HOME");
            if (home != NULL) {
                if (chdir(home) != 0)
                    fputs("Failed to change directory to HOME!\n", stderr);
            } else {
                fputs("HOME environment variable not set!\n", stderr);
            }
        } 

        /* 인자가 있는 경우 해당 디렉토리로 이동 */
        else {
            if (chdir(args[1]) != 0)
                fputs("Failed to change directory!\n", stderr);
        }
    }

    // path
    else if(!strcmp(args[0], "path")){
        /* path만 단일로 입력하면 현재 경로를 보여줌 */
        if (args[1]==NULL) printArgs(paths, "현재 path : ");

        /* path <인수1> <인수2> 형태로 입력시 실행경로를 바꿈*/
        else changePath(args);
    }
}


/* 내장명령어 path 구현 */
void changePath(char **args){
    int i;
	
	for(i=0;args[i]!=NULL;i++){
		free(paths[i]);
	}

    for(i=1;args[i]!=NULL;i++){
        paths[i-1] = strdup(args[i]);
    }
    paths[i-1] = NULL;

}
