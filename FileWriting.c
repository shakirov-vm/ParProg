#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ONE_ELEMENT 1
#define NULL_EXECUTOR 0
#define TAG_2 2
#define TEN_RADIX 10
#define FILENAME "txt.txt"

int main(int argc, char** argv) {
	
	MPI_Init(&argc, &argv);

	int commsize, this_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);

	if (commsize == 1) {
		printf("Our circle has degenerated into a point! Error");
		exit(1);
	}

	if (commsize >= 10) {

		MPI_Abort(MPI_COMM_WORLD, -1);
	}

	int incremental = 0;
    MPI_Status status;
    int writed;
    char writed_char[2];

	MPI_File_delete(FILENAME, MPI_INFO_NULL);
	MPI_File stream;
	MPI_File_open(MPI_COMM_WORLD, FILENAME, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &stream);
	
	if (this_rank == 0) {

		printf("\nStart\n");
		//MPI_Send(&incremental, ONE_ELEMENT, MPI_INT, this_rank + 1, TAG_2, MPI_COMM_WORLD);
		//MPI_Recv(&incremental, ONE_ELEMENT, MPI_INT, commsize - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);

		incremental += 1;
		writed = commsize - this_rank - 1;
		sprintf(writed_char, "%d", writed);

		MPI_File_seek(stream, sizeof(char) * this_rank, MPI_SEEK_SET); 
	    MPI_File_write(stream, writed_char, ONE_ELEMENT, MPI_CHAR, &status);

		//printf("This is 0 rank and incremental is %d\n", incremental);
	}

	if (this_rank != 0) {

		//MPI_Recv(&incremental, ONE_ELEMENT, MPI_INT, this_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
		incremental += 1;
		writed = commsize - this_rank - 1;
		sprintf(writed_char, "%d", writed);

		MPI_File_seek(stream, sizeof(char) * this_rank, MPI_SEEK_SET); 
	    MPI_File_write(stream, writed_char, ONE_ELEMENT, MPI_CHAR, &status);

		//printf("Rank is %d, incremental is %d\n", this_rank, incremental);
		//if (this_rank != commsize - 1) MPI_Send(&incremental, ONE_ELEMENT, MPI_INT, this_rank + 1, TAG_2, MPI_COMM_WORLD);
		//else MPI_Send(&incremental, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, TAG_2, MPI_COMM_WORLD);
	}

    MPI_File_close(&stream);
	MPI_Finalize();
}