#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 400
#define MAX_ARGS 15

#define NORMAL_MODE 0
#define BACKGROUND_MODE 1
#define BATCH_MODE 2
#define REDIRECTION 3

#define REDIRECTION_INPUT 1
#define REDIRECTION_OUTPUT 2
#define REDIRECTION_APPEND 3

int main(int argc, char *argv[]){
    char input[BUF_SIZE];
    char path[BUF_SIZE];
    char command[BUF_SIZE];
    char *args[MAX_ARGS];
    char *token;
    char abspath[BUF_SIZE] = "/bin/";
    int args_count;
    int mode=-1;
    int redirection_mode=-1;


    while(1){
        printf("tush> ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input)-1] = '\0';

        // 토큰으로 나누는 부분
        args_count = 0;
        token = strtok(input, " ");
        while(token != NULL){
            args[args_count++] = token;
            token = strtok(NULL, " ");
        }
        args[args_count] = NULL;


        // 무슨 모드인지 (백그라운드, 병렬모드 등등 나누는 부분)
        if (input[strlen(input)-1]=='&'){
            mode = BACKGROUND_MODE;
        }

        else
            mode = NORMAL_MODE;
            
        
        mode = REDIRECTION;
        redirection_mode = REDIRECTION_APPEND;
        printf("MODE : %d\n", mode);


        //프로세스 실행하는부분
        pid_t pid = fork();

        if (pid < 0)
            fputs("fork 에러 발생\n", stderr);

        else if (pid==0){
            
            if (mode==REDIRECTION){ // 리다이렉션일때
                switch(redirection_mode){
                case REDIRECTION_OUTPUT:
                    close(STDOUT_FILENO);
                    open("test.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                    break;
                case REDIRECTION_APPEND:
                    close(STDOUT_FILENO);
                    open("test.txt", O_CREAT|O_WRONLY|O_APPEND, S_IRWXU);
                    break;
                default:
                    break;
                }
            }
            execvp(args[0], args);
            fputs("child 프로세스 error\n", stderr);
        }

        else {
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
