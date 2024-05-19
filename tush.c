#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>  // 추가된 헤더 파일

/* 최대 글자 수, 최대 명령어 갯수 설정 */
#define BUF_SIZE 400
#define MAX_ARGS 15

#define NORMAL_MODE 1
#define BACKGROUND_MODE 2
#define BATCH_MODE 3
#define REDIRECTION_MODE 4
#define PARALLEL_MODE 5

#define REDIRECTION_INPUT 0
#define REDIRECTION_OUTPUT 1
#define REDIRECTION_APPEND 2

/* 테스트용 Args프린트하기 */
void printArgs(char**args){
    printf("args : [");
    int i;
    for(i=0;args[i]!=NULL;i++){
        printf("\"%s\", ", args[i]);
    }
    printf("NULL]\n");
}

/* 리다이렉션 모드인지 확인 (문자열에 '<' 기호 또는 '>' 가 들어가면 리다이렉션 모드로봄) */
int isRedirectionMode(char *input){
    int i;
    for(i=0;input[i]!='\0';i++){
        if(input[i]=='<' || input[i]=='>')
            return 1;
    }
    return 0;
}

/* args의 갯수 (NULL까지 몇개 있는지 확인하기 위함) */
int getArgsCount(char **args){
    int i = 0;
    for(i=0;args[i]!=NULL;i++){}
    return i;
}

/* 문자열의 마지막 문자 확인 */
char getLastCharacter(char* str){
    int i = 0;
    for(i=0;str[i]!='\0';i++){}
    return str[i-1];
}

/* input에서 speicla로 입력받은 문자열을 기준으로 쪼개서 매개변수로받은 commands에 저장함 */
/* strtok 함수 사용법 참고 : https://hackerpark.tistory.com/entry/C%EC%96%B8%EC%96%B4-strtok-%ED%95%A8%EC%88%98-%EB%AC%B8%EC%9E%90%EC%97%B4-%EC%9E%90%EB%A5%B4%EA%B8%B0 */
void specialSplit(char *input, char **commands, char *special){
    char *token;
    int command_count = 0;

    token = strtok(input, special);
    while (token != NULL) {
        commands[command_count++] = token;
        token = strtok(NULL, special);
    }
    commands[command_count] = NULL;
}

/* 프로세스 실행하는부분 */
void process_run(char **args, int mode){
        pid_t pid = fork();
        int status;
        
        if (pid < 0){
            fputs("fork 에러 발생\n", stderr);
        }

        else if (pid==0){ // 자식프로세스
            execvp(args[0], args);
            fputs("child process error\n", stderr);
            exit(1);
        }

        else { // 부모 프로세스
            if (mode==BACKGROUND_MODE) {
                printf("child process PID : %d\n", pid);
            }
            else{
                /* 자식 프로세스가 종료될때까지 wait함 */
                waitpid(pid, &status, 0);
            }
        }
}

/* 내부 명령어인지 확인하고 내부명령어 일시 명령어를 실행 () */
int internalCommand(char **args){
    /* chdir 참고 : https://hackingis.art/30 */
    if(!strcmp(args[0], "cd")){
        if (chdir(args[1])!=0)
            fputs("Failed change directory!\n", stderr);
        return 1;
    }
    
    else if(!strcmp(args[0], "exit")){
        exit(1);
    }

    // TODO path 내장명령어 만들기
    else if(!strcmp(args[0], "path")){
        
        return 1;
    }

    else
        return 0;
}

int main(int argc, char *argv[]){
    char input[BUF_SIZE];

    char *commands[BUF_SIZE];
    int command_count;

    char *token;
    char path[BUF_SIZE] = "/bin";
    int mode=0;
    int redirection_mode=0;

    char lastCharacter;


    int args_count;
    char *args[MAX_ARGS];

    /* TODO 배치모드 (파일내용에 있는 명령어들 실행) */
    if (argc==2) {
        
    }

    while(1){
        /* 사용자에게 명령어 입력받음 */
        printf("tush> ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input)-1] = '\0';
        lastCharacter = getLastCharacter(input);

        /* 리다이렉션 모드인지 확인 */
        mode = isRedirectionMode(input);

        /* 사용자의 명령어를 &기호로 나눔 */
        specialSplit(input, commands, "&");
        
        /* Args 확인 (테스트용) */
        printf("command ");
        printArgs(commands);

        /* &기호를 기준으로 로 토큰을 나눴을때 명령어 갯수*/
        command_count = getArgsCount(commands);

        /* 명령어가 단일일 경우 (중간에 &기호가 없는경우) */
        /* 내장명령어, 리다이렉션 모두 단일 명령어에서 실행 */
        if ( (command_count==1) && (lastCharacter!='&')){
            printf("case 1 ");
            specialSplit(commands[0], args, " ");
            printArgs(args);

            /* 내장 명령어인지 확인겸 실행하고, 아닐시 외부 명령어 실행*/
            if (!internalCommand(args)){
                process_run(args, NORMAL_MODE);
            }

        }

        /* 명령어가 단일이며 &로 끝나는 경우 */
        else if ( (command_count==1) && (lastCharacter=='&')){
            printf("case 2 ");
            specialSplit(commands[0], args, " ");
            printArgs(args);
            process_run(args, BACKGROUND_MODE);
        }

        /* command_count가 2 이상이며, input의 마지막 문자는 &기호가 아닌 경우 */
        else if ((command_count>=2) && (lastCharacter!='&')){
            for (int i = 0; i < command_count; i++) {
                specialSplit(commands[i], args, " ");
                printf("case 3 ");
                printArgs(args);

                /* 마지막 명령어 이전까지는 BACKGROUND로 실행, 마지막 명령어는 일반으로 실행*/
                if (i<command_count-1)
                    process_run(args, BACKGROUND_MODE);
                else
                    process_run(args, NORMAL_MODE);
            }
        }

        /* command_count가 2 이상이며, input의 마지막 문자는 &기호 일 경우 */
        /* 모든 명령어를 백그라운드로 실행 */
        else if ((command_count>=2) && (lastCharacter=='&')){
            for (int i = 0; i < command_count; i++) {
                specialSplit(commands[i], args, " ");
                printf("case 4 ");
                printArgs(args);
                process_run(args, BACKGROUND_MODE);
            }
        }


    }

    return 0;
}
