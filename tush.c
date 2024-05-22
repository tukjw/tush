#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

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
/* 참고 : https://m.blog.naver.com/coding-abc/221781138470 , 수정*/
char* DeleteSpace(char *s)
{
    char* str = (char*)malloc(strlen(s) + 1);
    int i, k = 0;

    for (i = 0; i < strlen(s); i++)
        if (s[i] != ' ') str[k++] = s[i];

    str[k] = '\0';
    return str;
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

/* 리다이렉션 실행 */
/* 구현중 */
void redirectionProcessrun(char *input, int redirection_mode, int run_mode){
	char *args[MAX_ARGS];
	printf("리다이렉션 모드 실행\n");
	
	if (redirection_mode==REDIRECTION_OUTPUT){
		printf("output\n");
		specialSplit(input, args, ">");
		printArgs(args);
		process_run(args, run_mode);
	}

	else if (redirection_mode==REDIRECTION_APPEND){
		printf("append\n");
		specialSplit(input, args, ">");
		printArgs(args);
	}
	

	/* TODO 리다이렉션 input 구현해야함 */
	else if (redirection_mode==REDIRECTION_INPUT){
		printf("input\n");
	}

	printArgs(args);
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
        process_run(args, NORMAL_MODE);
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
    char lastCharacter;
    int args_count;
    char *args[MAX_ARGS];
    int i;

    // TODO
    // char test[BUF_SIZE] = "가      나   다   라 마 바 사";
    // a
    // DeleteSpace(test);

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
        printf("tush> ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input)-1] = '\0';
        lastCharacter = getLastCharacter(input);

        /* 사용자의 명령어를 &기호로 나눔 */
        specialSplit(input, commands, "&");
        
        /* Args 확인 (테스트용) */
        // printf("command ");
        // printArgs(commands);

        /* &기호를 기준으로 로 토큰을 나눴을때 명령어 갯수*/
        command_count = getArgsCount(commands);

        /* 명령어가 단일일 경우 (중간에 &기호가 없는경우) */
        /* 내장명령어, 리다이렉션 모두 단일 명령어에서 실행 */
		if ( (command_count==1) && (lastCharacter!='&')){

			/* 리다이렉션 모드일경우 */
			if ((redirection_mode=isRedirectionMode(commands[0]))){
				redirectionProcessrun(commands[0], redirection_mode, NORMAL_MODE);
			}

			/* 리다이렉션이 아닐경우 */
			else{
				specialSplit(commands[0], args, " ");
				/* 명령어가 내장 명령어인지 확인겸 실행하고, 아닐시 외부 명령어 실행*/
				if (!internalCommand(args))
					process_run(args, NORMAL_MODE);
			}



		}

        /* 명령어가 단일이며 &로 끝나는 경우 */
        else if ( (command_count==1) && (lastCharacter=='&')){
            specialSplit(commands[0], args, " ");
            process_run(args, BACKGROUND_MODE);
        }

        /* command_count가 2 이상이며, input의 마지막 문자는 &기호가 아닌 경우 */
        else if ((command_count>=2) && (lastCharacter!='&')){
            for (i = 0; i < command_count; i++) {
                specialSplit(commands[i], args, " ");

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
            for (i = 0; i < command_count; i++) {
                specialSplit(commands[i], args, " ");
                process_run(args, BACKGROUND_MODE);
            }
        }


    }

    return 0;
}
