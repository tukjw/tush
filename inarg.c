#include "tush.h"

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

/* input에서 special로 입력받은 문자열을 기준으로 쪼개서 매개변수로받은 commands에 저장함 */
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