#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define ONE_ELEMENT 1
#define NULL_EXECUTOR 0
#define TAG_2 2

int main(int argc, char** argv) {
	
	MPI_Init(&argc, &argv);
	
	if (argc != 2) {
		printf("Need 1 arg\n");
		exit(1);
	}

	int N = atoi(argv[1]);

	int commsize, this_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);
	
	int start = this_rank * (N / commsize) + 1;
	int end;

	if (this_rank >= (N % commsize)) {
	
		start += N % commsize;
		end = start + N / commsize;
	} else {
	
		start += this_rank;
		end = start + N / commsize + 1;
	}
	
	double summ = 0;
	for (; start < end; start++) {
		summ += 1 / (double) start;
	}
	printf("\nRank: %d, end: %d, summ: %lf\n", this_rank, end, summ);

	if (this_rank != 0) {

		MPI_Send(&summ, ONE_ELEMENT, MPI_DOUBLE, NULL_EXECUTOR, TAG_2, MPI_COMM_WORLD);
	}

	if (this_rank == 0) {
		
		double part_summ;
		for (int i = 0; i < commsize - 1; i++) {
			MPI_Recv(&part_summ, ONE_ELEMENT, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
			summ += part_summ;
		}
		printf("\n\nSumm is %lf\n", summ);
	}
	MPI_Finalize();
}