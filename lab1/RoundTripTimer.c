#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define ONE_ELEMENT 1
#define NULL_EXECUTOR 0
#define TAG_2 2

int main(int argc, char** argv) {
	
	MPI_Init(&argc, &argv);

	int commsize, this_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);

	if (commsize == 1) {
		printf("Our circle has degenerated into a point! Error");
		exit(1);
	}

	int incremental = 0;

	for (int i = 0; i < 10000; i++) {

		if (this_rank == 0) {

		//	printf("\nStart\n");
			MPI_Send(&incremental, ONE_ELEMENT, MPI_INT, this_rank + 1, TAG_2, MPI_COMM_WORLD);
			MPI_Recv(&incremental, ONE_ELEMENT, MPI_INT, commsize - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
			incremental += 1;
		//	printf("This is 0 rank and incremental is %d\n", incremental);
		}

		if (this_rank != 0) {

			MPI_Recv(&incremental, ONE_ELEMENT, MPI_INT, this_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
			incremental += 1;
		//	printf("Rank is %d, incremental is %d\n", this_rank, incremental);
			if (this_rank != commsize - 1) MPI_Send(&incremental, ONE_ELEMENT, MPI_INT, this_rank + 1, TAG_2, MPI_COMM_WORLD);
			else MPI_Send(&incremental, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, TAG_2, MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
}