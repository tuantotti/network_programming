// VD9.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include<stdio.h>

static long num_steps = 1000000;
double step, pi;
const int NUM_THREAD = 4;
double localSum[NUM_THREAD];

DWORD WINAPI calSum(LPVOID arg);
void sequencial();
void parallel();

void main()
{
	
}

void sequencial() {
	int i;
	double x, sum = 0.0;
	step = 1.0 / (double)num_steps;
	for (i = 0; i < num_steps; i++) {
		x = (i + 0.5) * step;
		sum = sum + 4.0 / (1.0 + x * x);
	}
	pi = step * sum;
	printf("Pi = %f\n", pi);
}

DWORD WINAPI calSum(LPVOID arg) {
	int idx = *(int*)arg;
	double step = 1.0 / (double)num_steps;
	for (int i = idx; i < num_steps; i += NUM_THREAD) {
		double x = (i + 0.5) * step;
		localSum[i]= 4.0 / (1.0 + x * x);
	}
}

void parallel() {

}