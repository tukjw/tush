#include "tush.h"

/* path를 담는 배열 */
char *paths[MAX_PATHS] = { NULL };
/* 실행할때 -d 옵션붙이고 실행하면 printArgs 출력 */
int debug_flag = 0;

int main(int argc, char *argv[]){
    char input[BUF_SIZE];
    char *commands[MAX_ARGS];
    char *args[MAX_ARGS];
    int command_count;
    int mode = 0;
    int redirection_mode = 0;
    int internal_mode = 0;
    char lastCharacter;
    int i;

    /* 쉘 실행 전에 기본 path를 /bin으로 설정 */
	paths[0] = strdup("/bin");
	paths[1] = NULL;

    if (argc==2) {
        if (!strcmp("-d", argv[1])){
            debug_flag = 1;
        }

        /* 배치모드 (파일내용에 있는 명령어들 실행) */
        else{
            batchMode(argv);
            exit(1);
        }
    }

    else if (argc > 2){
        printf("파일은 한개만 입력해주세요!\n");
        exit(1);
    }

    while(1){
        /* 사용자에게 명령어 입력받음 */
	//printArgs(paths, "현재 path : ");
        printf("tush> ");
        fgets(input, sizeof(input), stdin);
        if (strlen(input)==1) continue;
        input[strlen(input)-1] = '\0';
        lastCharacter = getLastCharacter(input);

        /* 사용자의 명령어를 &기호로 나눔 */
        specialSplit(input, commands, "&");

        /* &기호를 기준으로 토큰을 나눴을때 명령어 갯수 카운팅*/
        command_count = getArgsCount(commands);

        /* Args 확인 (테스트용) */
        printArgs(commands, "\"&\" 기준으로 split 결과 : ");

        /* &기호로 나눠진만큼 명령어를 실행함 */
        for (int i = 0; i < command_count; i++) {
            
            /* 리다이렉션 모드인지 확인 */
            redirection_mode = isRedirectionMode(commands[i]);
            if (redirection_mode) redirectionSplit(commands[i], args, redirection_mode);

            else specialSplit(commands[i], args, " ");

            /* 내장명령어 일시 내장명령어 실행하고 continue */
            if(isInternalMode(args)){
                internalCommandRun(args);
                continue;
            }

            /* 명령어가 단일 명령어일 경우 */
            /* & 기호로 끝나는경우 백그라운드, 아닌경우 일반실행 */
            if (command_count == 1 && lastCharacter!='&') mode = NORMAL_MODE;
            else if (command_count == 1 && lastCharacter=='&') mode = BACKGROUND_MODE;
            /* 명령어가 단일이 아닌경우 (병렬모드) 백그라운드로 실행 */
            else mode=BACKGROUND_MODE;

            /* 마지막 실행 명령어가 & 기호로 끝나면 백그라운드, 아닌경우 일반으로 실행 */
            if ((i+1) == command_count && lastCharacter!='&') mode = NORMAL_MODE;
            else if ((i+1) == command_count && lastCharacter=='&') mode = BACKGROUND_MODE;

            process_run(args, mode, redirection_mode);

        }
    }
    return 0;
}

/* 테스트용 Args프린트하기 */
void printArgs(char**args, char* name){
    if (debug_flag){
        printf("%s[", name);
        int i;
        for(i=0;args[i]!=NULL;i++){
            printf("\"%s\", ", args[i]);
        }
        printf("NULL]\n");
    }
}

/* 에러 발생 핸들링 */
void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
