#include <pthread.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#define ONE_ELEMENT 1
#define NULL_EXECUTOR 0
#define TAG_2 2

struct for_thread {
	int* N_;
	int* executors_;
	std::vector<double>* shared_;
	int num_;
};

void* summ(void* info) {

	int N = *((struct for_thread*)info)->N_;
	int executors = *((struct for_thread*)info)->executors_;
	int this_rank = ((struct for_thread*)info)->num_;

	int start = this_rank * (N / executors) + 1;
	int end;

	if (this_rank >= (N % executors)) {
	
		start += N % executors;
		end = start + N / executors;
	} else {
	
		start += this_rank;
		end = start + N / executors + 1;
	}
	
	double summ = 0;
	for (; start < end; start++) {
		summ += 1 / (double) start;
	}

	printf("\nRank: %d, end: %d, summ: %lf\n", this_rank, end, summ);
	(*((struct for_thread*)info)->shared_)[this_rank] = summ;

	return NULL;
}

int main(int argc, char** argv) {
	
	if (argc != 2) {
		printf("Need 1 arg\n");
		exit(1);
	}

	int N = atoi(argv[1]);
	int executors = 4;

	std::vector<double> shared(executors);
	std::vector<struct for_thread> info(executors);

	for (int i = 0; i < executors; i++) {

		info[i].N_ = &N;
		info[i].executors_ = &executors;
		info[i].shared_ = &shared;
		info[i].num_ = i;
	}

	std::vector<pthread_t> pid(executors);

	double final_summ = 0;
	for (int i = 0; i < executors; i++) pthread_create(&pid[i], NULL, summ, &info[i]);
	for (int i = 0; i < executors; i++) pthread_join(pid[i], NULL);
	for (int i = 0; i < executors; i++) final_summ += shared[i];

	printf("\n\nSumm is %lf\n", final_summ);
}