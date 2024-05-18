#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    printf("====args====\n");
    int i;
    for(i=0;args[i]!=NULL;i++){
        printf("[%d] : %s\n", i, args[i]);
    }
    printf("[%d] : NULL\n", i);
    printf("=============\n");
}

/* 모드 선택기 */
int modeSelector(char *input){

    int ampersand_count = 0;
    int i;

    for(i=0;input[i]!=0;i++){

        if(input[i]=='<' || input[i]=='>')
            return REDIRECTION_MODE;

        if(input[i]=='&')
            ampersand_count++;
    }

    printf("마지막 문자 : %c\n", input[i-1]);
    if (ampersand_count==0) return NORMAL_MODE;
    else if ((ampersand_count==1) && (input[i-1]=='&')) return BACKGROUND_MODE;
    else return PARALLEL_MODE;

}

int main(int argc, char *argv[]){
    char input[BUF_SIZE];
    char command[BUF_SIZE];
    char *args[MAX_ARGS];
    char *token;
    char path[BUF_SIZE] = "/bin";
    int args_count;
    int mode=0;
    int redirection_mode=0;

    /* TODO 배치모드 (파일내용에 있는 명령어들 실행) */
    if (argc==2) {
        
    }

    while(1){
        /* 사용자에게 명령어 입력받음 */
        printf("tush> ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input)-1] = '\0';
        printf("사용자가 입력한 명령어 : %s\n", input);

        /* 모드 선택 */
        mode = modeSelector(input);

        /* 백그라운드 모드이면 문자열 맨끝 &기호를 제거 */
        if (mode==BACKGROUND_MODE){
            input[strlen(input)-1]=0;
            printf("%s\n", input);
        }
        printf("MODE : %d\n", mode);

        /* 스페이스 기준으로 문자열을 나눠서 args 포인터 배열에 저장 (배열 마지막은 NULL) */
        /* 참고 : https://hackerpark.tistory.com/entry/C%EC%96%B8%EC%96%B4-strtok-%ED%95%A8%EC%88%98-%EB%AC%B8%EC%9E%90%EC%97%B4-%EC%9E%90%EB%A5%B4%EA%B8%B0 */
        args_count = 0;
        token = strtok(input, " ");
        while(token != NULL){
            args[args_count++] = token;
            token = strtok(NULL, " ");
        }
        args[args_count] = NULL;

        printArgs(args);

        /* 내부 명령어 */
        /* chdir 참고 : https://hackingis.art/30 */
        if(!strcmp(args[0], "cd")){
            if (chdir(args[1])!=0)
                fputs("Failed change directory!\n", stderr);
            continue;
        }
        
        else if(!strcmp(args[0], "exit")){
            exit(1);
        }

        /* TODO path 구현하기 */
        else if(!strcmp(args[0], "path")){
            
        }

        /* 프로세스 실행하는부분 */
        pid_t pid = fork();

        if (pid < 0){
            fputs("fork 에러 발생\n", stderr);
            continue;
        }

        else if (pid==0){ // 자식프로세스
            
            if (mode==REDIRECTION_MODE){ // 리다이렉션 모드 일때

                switch(redirection_mode){

                case REDIRECTION_OUTPUT: // cmd > filename
                    close(STDOUT_FILENO);
                    open("test.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                    break;

                case REDIRECTION_APPEND: // cmd >> filename
                    close(STDOUT_FILENO);
                    open("test.txt", O_CREAT|O_WRONLY|O_APPEND, S_IRWXU);
                    break;

                case REDIRECTION_INPUT:    //TODO cmd < filename

                default:
                    break;
                }   
            }
            execvp(args[0], args);
            fputs("child 프로세스 error\n", stderr);
        }

        else { // 부모 프로세스
            if (mode==BACKGROUND_MODE) {
                printf("자식 프로세스 PID : %d\n", pid);
                continue;
            }
            else{
                wait(NULL);
            }
        }
    }

    return 0;
}
