#include "tush.h"

extern char *paths[MAX_PATHS];

/* path에서 실행 가능한 index 반환 */
int accessiblePathIndex(char *command){
	char absFilename[BUF_SIZE];
	int i;
	
	/* paths[i]에서 명령어가 실행 가능하면은 index를 리턴함 */
	for(i=0;paths[i]!=NULL;i++){
		snprintf(absFilename, sizeof(absFilename), "%s/%s", paths[i], command);
		if(access(absFilename, X_OK)!=-1) return i;
	}

	return -1;
}

/* 리다이렉션 모드인지 확인 */
int isRedirectionMode(char *input){
    int i;
    for(i=0;input[i]!='\0';i++){
        if(input[i]=='<')
			return REDIRECTION_INPUT;
		if(input[i]=='>'){
			if(input[i+1]=='>')
				return REDIRECTION_APPEND;
			else
				return REDIRECTION_OUTPUT;
		}
    }
    return 0;
}

/* input을 리다이렉션 구분자를 기준으로 args에 [ "명령어" , "파일이름" , NULL ] 형태로 저장함 */
void redirectionSplit(char *input, char **args, int redirection_mode){
    char *pt;
    if (redirection_mode==REDIRECTION_OUTPUT)
        specialSplit(input, args, ">");
    else if (redirection_mode==REDIRECTION_APPEND)
        specialSplit(input, args, ">>");
    else if (redirection_mode==REDIRECTION_INPUT)
        specialSplit(input, args, "<");

    args[0] = trim(args[0]);
    args[1] = trim(args[1]);
    args[2] = NULL;
    
    printArgs(args, "redirection split : ");
}

/* 프로세스 실행하는부분 */
void process_run(char **args, int mode, int redirection_mode){
    int status;
    char *t_args[MAX_ARGS];
    int i;

	int path_index;
	char temp_path[100];
	char *pt;

    pid_t pid = fork();
    if (pid < 0){
        fputs("fork error\n", stderr);
    }

    /* 자식 프로세스 */
    else if (pid==0){

        /* 리다이렉션 경우 (OSTEP 교재 참고) */
        /* args[0] : 명령어 / args[1] : 파일이름 */
        if(redirection_mode){

            /* 출력 리다이렉션 */
            if (redirection_mode == REDIRECTION_OUTPUT){
                    close(STDOUT_FILENO);
                    if (open(args[1], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU) < 0)
                        error_handling("open error");
            }
            else if (redirection_mode == REDIRECTION_APPEND){
                    close(STDOUT_FILENO);
                    if(open(args[1], O_CREAT|O_WRONLY|O_APPEND, S_IRWXU) < 0)
                        error_handling("open error");
            }		

            /* 입력 리다이렉션 */
            else if (redirection_mode==REDIRECTION_INPUT)
            {
                close(STDIN_FILENO);
                if(open(args[1], O_RDONLY))
                    error_handling("open error");
            }
            
            /* args[0]의 명령어와 옵션 분리해서 t_args에 넣음 */
            specialSplit(args[0], t_args, " ");
        }

        /* 리다이렉션 모드 아닌경우 */
        /* 리다이렉션 모드와 args를 통일하기위해 t_args에 args를 그대로 넣음 */
        else{
            for(i=0;args[i]!=NULL;i++){
                t_args[i] = args[i];
            }
            t_args[i] = NULL;
        }
        
        /* 프로그램 실행 전 path 설정 */
        if((path_index = accessiblePathIndex(t_args[0])) == -1){
            fputs("프로그램 실행 경로를 찾지 못하였습니다.\n", stderr);
            exit(1); // 자식프로세스 종료
        }

        else{
            pt = strdup(paths[path_index]);
            snprintf(temp_path, sizeof(temp_path), "%s/%s", pt, t_args[0]);
            t_args[0] = temp_path;
            // printf("실행 될 path : %s\n", temp_path);
            free(pt);
        }

        /* 명령어 실행 */
        printArgs(t_args, "실행 args : ");
        execvp(t_args[0], t_args);
        error_handling("child process run error");
    }

    /* 부모 프로세스 */
    else {
        if (mode==BACKGROUND_MODE) {
            printf("child process PID : %d\n", pid);
        }
        else{
            /* 백그라운드 모드가 아니면 자식 프로세스가 종료될때까지 wait함 */
            waitpid(pid, &status, 0);
        }
    }
}

/* 배치모드 (메모장에 있는 명령어들을 \n 기준으로 분리해서 실행)*/
void batchMode(char **argv){
    int fd;
    char input[BUF_SIZE * 10];
    int str_len;
    char *commands[BUF_SIZE];
    int command_count;
    char *args[MAX_ARGS];
    int i;

    fd = open(argv[1], O_RDONLY);
    
    if (fd < 0)
        error_handling("open error");

    str_len = read(fd, input, sizeof(input));
    input[str_len]=0;
    specialSplit(input, commands, "\n");
    command_count = getArgsCount(commands);

    for(i=0;commands[i]!=NULL;i++){
        specialSplit(commands[i], args, " ");
        // printArgs(args);
        process_run(args, NORMAL_MODE, 0);
    }

    close(fd);
}
