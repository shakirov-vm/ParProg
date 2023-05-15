#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define ONE_ELEMENT 1
#define NULL_EXECUTOR 0
#define TAG_2 2
#define TEN_RADIX 10
#define FILENAME "output.txt"

void bitonic_simple(int new_sequence_size, int biton_size, int bucket_size, int part_job, MPI_Win win) {

	int k = part_job % (bucket_size / 2);
	int j = (part_job / (bucket_size / 2)) % (biton_size / bucket_size);
	int i = (part_job / (biton_size / 2)) % (new_sequence_size / biton_size);

	int tmp_1, tmp_2;

	if ((i % 2) == 0) {

		int up = i * biton_size + bucket_size * j + k;
		int low = i * biton_size + bucket_size * j + k + bucket_size / 2;

		MPI_Get(&tmp_1, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, up , ONE_ELEMENT, MPI_INT, win);
		MPI_Get(&tmp_2, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, low, ONE_ELEMENT, MPI_INT, win);

		if (tmp_1 > tmp_2) {
    		MPI_Put(&tmp_1, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, low, ONE_ELEMENT, MPI_INT, win);
			MPI_Put(&tmp_2, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, up , ONE_ELEMENT, MPI_INT, win);
		}

 	} 
    else {

        int up = i * biton_size + bucket_size * j + k;
        int low = i * biton_size + bucket_size * j + k + bucket_size / 2;

		MPI_Get(&tmp_1, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, up , ONE_ELEMENT, MPI_INT, win);
		MPI_Get(&tmp_2, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, low, ONE_ELEMENT, MPI_INT, win);

        if (tmp_1 < tmp_2) {

    		MPI_Put(&tmp_1, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, low, ONE_ELEMENT, MPI_INT, win);
			MPI_Put(&tmp_2, ONE_ELEMENT, MPI_INT, NULL_EXECUTOR, up , ONE_ELEMENT, MPI_INT, win);
        }
 	}
}

int main(int argc, char** argv) {
	
	MPI_Init(&argc, &argv);

	int commsize, this_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);

	size_t sequence_size =	atoi(argv[1]);
	size_t new_sequence_size = sequence_size;

	if ((sequence_size & (sequence_size - 1)) != 0) {

        for (int i = 1; i < 64; i <<= 1) new_sequence_size |= (new_sequence_size >> i);
        new_sequence_size++;
    }

	MPI_Win win;
	int* numbers;
	
	MPI_Alloc_mem(new_sequence_size * sizeof(int), MPI_INFO_NULL, &numbers);
	MPI_Win_create(numbers, sizeof(int) * new_sequence_size, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

	if (this_rank == 0) {

		for (int i = 0; i < sequence_size; i++)
			numbers[i] = sequence_size - i;
		for (int i = sequence_size; i < new_sequence_size; i++)
			numbers[i] = 0;
	}
/*
	if (this_rank == 0) {

		for (int i = 0; i < new_sequence_size - sequence_size; i++)
			numbers[i] = 0;
		for (int i = new_sequence_size - sequence_size; i < new_sequence_size; i++)
			numbers[i] = i - new_sequence_size + sequence_size;
	}
*/
	MPI_Barrier(MPI_COMM_WORLD);

	clock_t start, stop;
	start = clock();

	MPI_Win_lock_all(MPI_MODE_NOCHECK, win);
    for (int biton_size = 2; biton_size < new_sequence_size + 1; biton_size *= 2) {

        for (int bucket_size = biton_size; bucket_size > 1; bucket_size /= 2) {

			for (int part_job = this_rank; part_job < new_sequence_size / 2; part_job += commsize) {

				bitonic_simple(new_sequence_size, biton_size, bucket_size, part_job, win);
			}

			MPI_Barrier(MPI_COMM_WORLD);
        }
    }
	MPI_Win_unlock_all(win);

	stop = clock();

	if (this_rank == 0) {
		printf("\nLoop required %ld ticks", stop - start);
	}

    if (this_rank == 0) {

	    FILE* stream = fopen(FILENAME, "w");

	    int num_nulls = new_sequence_size - sequence_size;
    	for (int i = 0; i < new_sequence_size; i++) {

    		if ((num_nulls > 0) && (numbers[i] == 0)) {
    			num_nulls--;
    			continue;
    		}
			fprintf(stream, "%d ", numbers[i]);
		}
		fclose(stream);
	}

    MPI_Win_free(&win);

	MPI_Free_mem(numbers);
	MPI_Finalize();
}
