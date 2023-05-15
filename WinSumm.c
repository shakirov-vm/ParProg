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

	MPI_Win win;
	double* part_summ = (double*) calloc (commsize - 1, sizeof(double));
	MPI_Win_create(part_summ, sizeof(double) * commsize, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    MPI_Win_fence(0, win);

	if (this_rank != 0) {

		MPI_Put(&summ, ONE_ELEMENT, MPI_DOUBLE, NULL_EXECUTOR, this_rank - 1, ONE_ELEMENT, MPI_DOUBLE, win);
    	MPI_Win_fence(0, win);
	}

	if (this_rank == 0) {

    	MPI_Win_fence(0, win);
		
		for (int i = 0; i < commsize - 1; i++) {
			
			summ += part_summ[i];
		}
		printf("\n\nSumm is %lf\n", summ);
	}
    MPI_Win_free(&win);
	free(part_summ);

	MPI_Finalize();
}