#include <mpi.h>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <memory>

#define ONE_ELEMENT 1
#define TAG_2 2
#define FOUR_KB 4096 * 4
#define ONE_MB 1024 * 1024

void test_MPI(int this_rank) {

	int* four_kb_ptr = (int *) calloc (FOUR_KB, sizeof(int));

	for (int i = 1; i <= FOUR_KB; i *= 2) { 

		if (this_rank == 0) {

			clock_t start_time = clock(); // Maybe time() ?
			MPI_Send(four_kb_ptr, ONE_ELEMENT * i, MPI_INT, this_rank + 1, TAG_2, MPI_COMM_WORLD);
			clock_t end_time = clock(); // Maybe time() ?
			
			std::cout << "\nTime is " << end_time - start_time << " ticks" << std::endl;
			std::cout << "Or time is " << ((double) end_time - start_time) / CLOCKS_PER_SEC << " seconds" << std::endl; 
			std::cout << "Size on this step was " << i << std::endl;
		}
		else if (this_rank == 1) {

			sleep(2);
			MPI_Recv(four_kb_ptr, ONE_ELEMENT * i, MPI_INT, this_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	free(four_kb_ptr);
}

void test_MPI_S(int this_rank) {

	int* four_kb_ptr = (int *) calloc (FOUR_KB, sizeof(int));

	for (int i = 1; i <= FOUR_KB; i *= 2) { 

		if (this_rank == 0) {

			clock_t start_time = clock(); // Maybe time() ?
			MPI_Ssend(four_kb_ptr, ONE_ELEMENT * i, MPI_INT, this_rank + 1, TAG_2, MPI_COMM_WORLD);
			clock_t end_time = clock(); // Maybe time() ?
			
			std::cout << "\nTime is " << end_time - start_time << " ticks" << std::endl;
			std::cout << "Or time is " << ((double) end_time - start_time) / CLOCKS_PER_SEC << " seconds" << std::endl; 
			std::cout << "Size on this step was " << i << std::endl;
		}
		else if (this_rank == 1) {

			sleep(2);
			MPI_Recv(four_kb_ptr, ONE_ELEMENT * i, MPI_INT, this_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	free(four_kb_ptr);
}


void test_MPI_R(int this_rank) {

	int* four_kb_ptr = (int *) calloc (FOUR_KB, sizeof(int));

	for (int i = 1; i <= FOUR_KB; i *= 2) { 

		if (this_rank == 0) {

			clock_t start_time = clock(); // Maybe time() ?
			MPI_Rsend(four_kb_ptr, ONE_ELEMENT * i, MPI_INT, this_rank + 1, TAG_2, MPI_COMM_WORLD);
			clock_t end_time = clock(); // Maybe time() ?
			
			std::cout << "\nTime is " << end_time - start_time << " ticks" << std::endl;
			std::cout << "Or time is " << ((double) end_time - start_time) / CLOCKS_PER_SEC << " seconds" << std::endl; 
			std::cout << "Size on this step was " << i << std::endl;
		}
		else if (this_rank == 1) {

			sleep(2);
			MPI_Recv(four_kb_ptr, ONE_ELEMENT * i, MPI_INT, this_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	free(four_kb_ptr);
}

void test_MPI_B(int this_rank) {

	int* four_kb_ptr = (int *) calloc (FOUR_KB, sizeof(int));
	int* buffer_for_attach = (int *) calloc (ONE_MB, sizeof(int));

	MPI_Buffer_attach(buffer_for_attach, ONE_MB);

	for (int i = 1; i <= FOUR_KB; i *= 2) { 

		if (this_rank == 0) {

			clock_t start_time = clock(); // Maybe time() ?
			MPI_Bsend(four_kb_ptr, ONE_ELEMENT * i, MPI_INT, this_rank + 1, TAG_2, MPI_COMM_WORLD);
			clock_t end_time = clock(); // Maybe time() ?
			
			std::cout << "\nTime is " << end_time - start_time << " ticks" << std::endl;
			std::cout << "Or time is " << ((double) end_time - start_time) / CLOCKS_PER_SEC << " seconds" << std::endl; 
			std::cout << "Size on this step was " << i << std::endl;
		}
		else if (this_rank == 1) {

			sleep(2);
			MPI_Recv(four_kb_ptr, ONE_ELEMENT * i, MPI_INT, this_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}
	
	free(four_kb_ptr);
	free(buffer_for_attach);
}

int main(int argc, char** argv) {
	
	MPI_Init(&argc, &argv);

	int commsize, this_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);

	test_MPI(this_rank);

	std::cout << "test_MPI finished\n";

	test_MPI_S(this_rank);

	std::cout << "test_MPI_S finished\n";

	test_MPI_R(this_rank);

	std::cout << "test_MPI_R finished\n";

	test_MPI_B(this_rank);

	std::cout << "test_MPI_B finished\n";

	MPI_Finalize();

	std::cout << "FINISH\n";
}