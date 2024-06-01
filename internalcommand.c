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
        if (chdir(args[1])!=0)
            fputs("Failed change directory!\n", stderr);
    }
    
    else if(!strcmp(args[0], "exit")){
        exit(0);
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