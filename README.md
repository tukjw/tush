# [tush](https://github.com/tukjw/tush) (tukorea shell)

## 24.06.06 update
- 내장명령어 cd 실행 시 인자가 없거나 틸드(~) 일 경우 홈 디렉토리로 이동

## 개요

tush는 [ostep-projects의 processes-shell](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/processes-shell)에 있는 요구사항을 충족시키는 간단한 쉘이다.

이 쉘은 기본적인 명령어 실행, 백그라운드 작업, 입출력 리다이렉션, 내장 명령어 처리 기능을 제공한다.


## 구성도

### 헤더파일 (tush.h)
- 매크로 및 함수 선언

### 메인 모듈 (tush.c)
- 프로그램의 진입점
- 사용자 입력을 받아 명령어를 파싱하고 실행
- 내장 명령어와 외부 명령어를 구분하여 처리
- 배치 모드와 입력 모드를 관리

### 인자 처리 모듈 (inarg.c)

- 명령어와 인자의 파싱 및 처리
- 문자열 처리를 위한 유틸리티 함수들

### 명령어 실행 모듈 (runcommand.c)
- 외부 명령어 실행
- 리다이렉션 처리 및 백그라운드 실행 관리

### 내장 명령어 처리 모듈 (internalcommand.c)
- 내장 명령어(cd, exit, path)의 처리


## 사용법

1. make를 입력하여 컴파일한다.
2. ./tush 를 입력하여 실행한다.
3. 명령어를 입력한다.

- 디버그모드를 이용하고싶다면 -d 옵션을 붙여 실행한다.
- ./tush filename 을 입력하면 파일 내용을 한줄씩 읽어가며 명령어들을 실행한다. (batchmode)


## 작동범위
- 외장명령어 실행 (ls, pwd, . . .)
- 백그라운드 실행
- 리다이렉션
- 내장명령어 (cd, exit, path)
- 배치모드 (파일 내용의 명령어를 한줄씩 읽어가며 실행)
- 병렬모드 (여러개의 명령어를 '&'로 나누어 병렬로 실행)
