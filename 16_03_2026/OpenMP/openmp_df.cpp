//#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void f0(int * argArr, int * resArr) {
	resArr[0] = argArr[0] + argArr[1];
	//_sleep(1000);
}

//...

#define MAX_STAGE_COUNT 10
#define MAX_PE_COUNT 10

void(*fArr[MAX_STAGE_COUNT][MAX_PE_COUNT])(int * argArr, int * resArr) = {
	{ f0,   f0,   f0,   f0,   f0,   f0,   f0   },
	{ f0,   NULL, f0,   NULL, f0,   NULL, f0   },
	{ f0,   NULL, NULL, NULL, f0,   NULL, NULL },
	{ f0,   NULL, NULL, NULL, NULL, NULL, NULL },
};

void compute(){
	int argArr[MAX_PE_COUNT * 2], resArr[MAX_PE_COUNT * 2];
	for (int iIndex = 0; iIndex < MAX_STAGE_COUNT; ++iIndex) {
        #pragma omp parallel for shared(argArr, resArr)
		for (int jIndex = 0; jIndex < MAX_PE_COUNT; ++jIndex) {
			if (!fArr[iIndex][jIndex]) continue;
			if (!(iIndex % 2)) fArr[iIndex][jIndex](argArr, resArr);
			else fArr[iIndex][jIndex](resArr, argArr);
		}
	}
}

int main(int argc, char* argv[]) {
	compute();

	return 0;
}

