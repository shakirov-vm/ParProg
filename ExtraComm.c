#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define ONE_ELEMENT 1
#define NULL_EXECUTOR 0
#define TAG_2 2

void calc_summ(int N, MPI_Comm* newcomm) {

	int commsize, this_rank;
	MPI_Comm_size(*newcomm, &commsize);
	MPI_Comm_rank(*newcomm, &this_rank);

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

	double new_summ;
	MPI_Reduce(&summ, &new_summ, ONE_ELEMENT, MPI_DOUBLE, MPI_SUM, NULL_EXECUTOR, *newcomm);

	if (this_rank != 0) {

		//MPI_Send(&summ, ONE_ELEMENT, MPI_DOUBLE, NULL_EXECUTOR, TAG_2, MPI_COMM_WORLD);
	}

	if (this_rank == 0) {
		
		/*double part_summ;
		for (int i = 0; i < commsize - 1; i++) {
			MPI_Recv(&part_summ, ONE_ELEMENT, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
			summ += part_summ;
		}*/
		printf("\n\nSumm is %lf\n", new_summ);
	}
}

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
	
	int* list_processes = (int *) calloc (commsize - 1, sizeof(int));

	for (int i = 0; i < commsize - 1; i++) {
		
		list_processes[i] = i;
	}

	MPI_Group newgroup;
	MPI_Group worldgroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldgroup);
	int err = MPI_Group_incl(worldgroup, commsize - 1, list_processes, &newgroup);

	MPI_Comm newcomm;
	err = MPI_Comm_create(MPI_COMM_WORLD, newgroup, &newcomm);

	if (this_rank != commsize - 1) 
		calc_summ(N, &newcomm);
	
	free(list_processes);

	//MPI_Comm_free(&newcomm); 
    MPI_Group_free(&worldgroup); 
    MPI_Group_free(&newgroup); 
	MPI_Finalize();
}