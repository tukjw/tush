#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>

/* path, 입력 리다이렉션 남음 */

/* 최대 글자 수, 최대 명령어 갯수 설정 */
#define BUF_SIZE 400
#define MAX_ARGS 15

#define NORMAL_MODE 1
#define BACKGROUND_MODE 2
#define BATCH_MODE 3
#define REDIRECTION_MODE 4
#define PARALLEL_MODE 5

#define REDIRECTION_INPUT 1 
#define REDIRECTION_OUTPUT 2
#define REDIRECTION_APPEND 3

/* 테스트용 Args프린트하기 */
void printArgs(char**args){
    printf("args : [");
    int i;
    for(i=0;args[i]!=NULL;i++){
        printf("\"%s\", ", args[i]);
    }
    printf("NULL]\n");
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

/* 문자열 공백 제거 함수 */
/* 참고 : https://www.dotnetnote.com/docs/c-language/topics/string-trim/  */

// trim_left 함수: 문자열의 왼쪽 공백을 제거
char* trim_left(char* str) {
    while (*str) {
        if (isspace(*str)) {
            str++;
        }
        else {
            break;
        }
    }
    return str;
}

// trim_right 함수: 문자열의 오른쪽 공백을 제거
char* trim_right(char* str) {
    int len = (int)strlen(str) - 1;

    while (len >= 0) {
        if (isspace(*(str + len))) {
            len--;
        }
        else {
            break;
        }
    }
    *(str + ++len) = '\0';
    return str;
}

// trim 함수: 문자열의 양쪽 공백을 제거
char* trim(char* str) {
    return trim_left(trim_right(str));
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
void process_run(char **args, int mode, int redirection_mode){
        pid_t pid = fork();
        int status;

        char *t_args[MAX_ARGS];
        char *tt_args[MAX_ARGS];

        if (pid < 0){
            fputs("fork 에러 발생\n", stderr);
        }
        
        /* 자식 프로세스 */
        else if (pid==0){
            
            if (redirection_mode==REDIRECTION_OUTPUT||redirection_mode==REDIRECTION_APPEND){
                /* OUTPUT REDIRECTION */
                /* args[0] : 명령어 / args[1] : 파일이름 */

                printf("output\n");
                close(STDOUT_FILENO);
                if (redirection_mode == REDIRECTION_OUTPUT)
                    open(args[1], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                else if (redirection_mode == REDIRECTION_APPEND)
                    open(args[1], O_CREAT|O_WRONLY|O_APPEND, S_IRWXU);

                specialSplit(args[0], t_args, " ");
                execvp(t_args[0], t_args);
                fputs("child process error\n", stderr);
                exit(1);
            }

            else{
                execvp(args[0], args);
                fputs("child process error\n", stderr);
                exit(1);
            }
        }

        /* 부모 프로세스 */
        else {
            if (mode==BACKGROUND_MODE) {
                printf("child process PID : %d\n", pid);
            }
            else{
                /* 백그라운드모드가 아니면 자식 프로세스가 종료될때까지 wait함 */
                waitpid(pid, &status, 0);
            }
        }
}

/* 리다이렉션 모드인지 확인 */
int isRedirectionMode(char *input){
    int i;
    for(i=0;input[i]!='\0';i++){
        if(input[i]=='<')
			return REDIRECTION_INPUT;
		if(input[i]=='>')
			if(input[i+1]=='>')
				return REDIRECTION_APPEND;
			else
				return REDIRECTION_OUTPUT;

    }
    return 0;
}

/* input을 리다이렉션 구분자를 기준으로 args에 [명령어/파일이름/NULL] 형태로 저장함 */
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
    
    printArgs(args);
}

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
        exit(1);
    }

    // TODO path 내장명령어 만들기
    else if(!strcmp(args[0], "path")){
        
    }
}

void batchMode(char **argv){
    int fd;
    char input[BUF_SIZE];
    int str_len;
    char *commands[BUF_SIZE];
    int command_count;
    char *args[MAX_ARGS];
    int i;

    fd = open(argv[1], O_RDONLY);

    if(fd<0) {
        fprintf(stderr, "\"%s\" open error\n", argv[1]);
        exit(1);
    }

    str_len = read(fd, input, sizeof(input));
    input[str_len]=0;
    specialSplit(input, commands, "\n");
    command_count = getArgsCount(commands);

    for(i=0;commands[i]!=NULL;i++){
        specialSplit(commands[i], args, " ");
        // printArgs(args);
        process_run(args, NORMAL_MODE, 0);
    }
}

int main(int argc, char *argv[]){
    char input[BUF_SIZE];
    char *commands[BUF_SIZE];
    int command_count;
    char *token;
    char path[BUF_SIZE] = "/bin";
    int mode=0;
    int redirection_mode=0;
    int internal_mode = 0;
    char lastCharacter;
    int args_count;
    char *args[MAX_ARGS];
    int i;

    /* 배치모드 (파일내용에 있는 명령어들 실행) */
    if (argc==2) {
        batchMode(argv);
        exit(1);
    }

    else if (argc > 2){
        printf("파일은 한개만 입력해주세요!\n");
        exit(1);
    }

    while(1){
        /* 사용자에게 명령어 입력받음 */

        //현재 경로 test 출력
        printf("현재 path = %s\n", path);
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
        // printf("command ");
        // printArgs(commands);
        
        /* 내장명령어 일시 실행하고 continue */
        if(isInternalMode(commands)){
            internalCommandRun(commands);
            continue;
        }

        /* &기호로 나눠진만큼 명령어를 실행함 */
        /* TODO 여기부터 시작 */
        for (int i = 0; i < command_count; i++) {
            
            redirection_mode = isRedirectionMode(commands[i]);
            
            if (redirection_mode) redirectionSplit(commands[i], args, redirection_mode);
            else specialSplit(commands[i], args, " ");

            // test
            // printf("실행직전 ");
            // printArgs(args);

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
