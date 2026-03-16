// N.Kozak // Lviv'2019
//#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
#include <errno.h>
#include <pthread.h>

// for compile: gcc -pthread -o pthread_df pthread_df.cpp
// for run:     ./pthread_df

// A*X*X*X*X*X*X*X + B*X*X*X*X*X*X + C*X*X*X*X*X + D*X*X*X*X + E*X*X*X + F*X*X + G*X + H
#define A 4 
#define X 5
#define B 7
#define C 10
#define D 13
#define E 16
#define F 19
#define G 22
#define H 25

#define RESULT ( A * X * X * X * X * X * X * X + B * X * X * X * X * X * X + C * X * X * X * X * X + D * X * X * X * X + E * X * X * X + F * X * X + G * X + H )

// (f0:A*X**7) (f1:B*X**6) (f2:C*X**5) (f3:D*X**4) (f4:E*X**3) (f5:F*X**2) (f6:G*X) (f7:H) 
//            ||                      ||                      ||                   || 
//            \/                      \/		              \/                   \/
//          (f8:+)                  (f9:+)                  (f10:+)              (f11:+)
//                       ||                                              ||
//                       \/		                                         \/
//                     (f12:+)                                         (f13:+)
//                                               ||
//                                               \/
//                                             (f14:+)
	
#define MAX_STAGE_COUNT 10
#define MAX_PE_COUNT 10	
	
int argArr[MAX_PE_COUNT * 2], resArr[MAX_PE_COUNT * 2];
int * argArr_ = argArr, * resArr_ = resArr;
#define GET_ARR(PARAM) \
int * argArr, * resArr;\
if (!(PARAM)) {\
    argArr = argArr_;\
    resArr = resArr_;\
}\
else{\
    argArr = resArr_;\
    resArr = argArr_;\
}
			
// stage with index 0

void * f0(void *param) {
    GET_ARR(param);
	resArr[0] = A * X * X * X * X * X * X * X;
	//sleep(1);
	pthread_exit(NULL);
}

void * f1(void *param) {
    GET_ARR(param);
	resArr[1] = B * X * X * X * X * X * X;
	//sleep(1);
	pthread_exit(NULL);	
}

void * f2(void *param) {
    GET_ARR(param);
	resArr[2] = C * X * X * X * X * X;
	//sleep(1);
	pthread_exit(NULL);	
}

void * f3(void *param) {
    GET_ARR(param);
	resArr[3] = D * X * X * X * X;
	//sleep(1);
	pthread_exit(NULL);
}

void * f4(void *param) {
    GET_ARR(param);
	resArr[4] = E * X * X * X;
	//sleep(1);
	pthread_exit(NULL);	
}

void * f5(void *param) {
    GET_ARR(param);
	resArr[5] = F * X * X;
	//sleep(1);
	pthread_exit(NULL);	
}

void * f6(void *param) {
    GET_ARR(param);
	resArr[6] = G * X;
	//sleep(1);
	pthread_exit(NULL);	
}

void * f7(void *param) {
    GET_ARR(param);
	resArr[7] = H;
	//sleep(1);
	pthread_exit(NULL);	
}

// stage with index 1

void * f8(void *param) {
    GET_ARR(param);
	resArr[0] = argArr[0] + argArr[1];
	//sleep(1);
	pthread_exit(NULL);
}

void * f9(void *param) {
    GET_ARR(param);
	resArr[2] = argArr[2] + argArr[3];
	//sleep(1);
	pthread_exit(NULL);	
}

void * f10(void *param) {
    GET_ARR(param);
	resArr[4] = argArr[4] + argArr[5];
	//sleep(1);
	pthread_exit(NULL);
}

void * f11(void *param) {
    GET_ARR(param);
	resArr[6] = argArr[6] + argArr[7];
	//sleep(1);
	pthread_exit(NULL);	
}

// stage with index 2

void * f12(void *param) {
    GET_ARR(param);
	resArr[0] = argArr[0] + argArr[2];
	//sleep(1);
	pthread_exit(NULL);	
}

void * f13(void *param) {
    GET_ARR(param);
	resArr[4] = argArr[4] + argArr[6];
	//sleep(1);
	pthread_exit(NULL);
}

// stage with index 3

void * f14(void *param) {
    GET_ARR(param);
	resArr[0] = argArr[0] + argArr[4];
	//sleep(1);
	pthread_exit(NULL);	
}

void*(*fArr[MAX_STAGE_COUNT][MAX_PE_COUNT])(void *param) = {
	{ f0,  f1,   f2,   f3,   f4,   f5,   f6,   f7   },
	{ f8,  NULL, f9,   NULL, f10,  NULL, f11,  NULL },
	{ f12, NULL, NULL, NULL, f13,  NULL, NULL, NULL },
	{ f14, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
};
#define LAST_STAGE_INDEX 3

void compute(){
    pthread_t threads[MAX_STAGE_COUNT][MAX_PE_COUNT];
	for (int iIndex = 0; iIndex < MAX_STAGE_COUNT; ++iIndex) {
        // create threads
		for (int jIndex = 0; jIndex < MAX_PE_COUNT; ++jIndex) {
			if (!fArr[iIndex][jIndex]) continue;
			int retunValue;
			while(EAGAIN == (retunValue = pthread_create(&threads[iIndex][jIndex], NULL, fArr[iIndex][jIndex], (void*)(long long int)(iIndex % 2))));
			if(retunValue){
			    printf("Thread create error. Error code %X\r\n", retunValue);
			    return;			    
			}			
		}		
        // join threads
		for (int jIndex = 0; jIndex < MAX_PE_COUNT; ++jIndex) {
		    if (!fArr[iIndex][jIndex]) continue;
		    pthread_join(threads[iIndex][jIndex], NULL);
		}
	}

	int * res = (LAST_STAGE_INDEX + 1) % 2 ? resArr : argArr;
	printf("result of execution = %d \r\n", *res);
	printf("expected result     = %d \r\n", RESULT);
	printf("-------------------------------\r\n");
	if (*res == RESULT){
		printf("verify status: succes\r\n");
	}
	else{
		printf("verify status: not success\r\n");
	}
}

int main(int argc, char* argv[]) {
	compute();

	return 0;
}