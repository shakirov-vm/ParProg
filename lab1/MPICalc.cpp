#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <mpi.h>

#define X_MAX_LEN 1
#define T_MAX_TIME 1.0
#define h 0.0005
#define tau 0.0001

#define NULL_EXECUTOR 0
#define ONE_ELEMENT 1
#define TAG_2 2

int M = static_cast<int>(X_MAX_LEN / h);
int N = static_cast<int>(T_MAX_TIME / tau);

double func_initical_cond(double x) {
	return sin(3.1416 * x);
}
double func_bound_cond_l(double t) {
	return 0;
}
double func_bound_cond_r(double t) {
	return 0;
}
double func_f(double x, double t) {
	return 0;
}

void calc_u(std::vector<double>& initical_cond, std::vector<double>& bound_cond_l, std::vector<double>& bound_cond_r, std::vector<std::vector<double>>& u) {

	int commsize, this_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);
	// [left_m; right_m)

	int range = M - 2;

	int left_m = this_rank * (range / commsize) + 1;
	int right_m;

	if (this_rank >= (range % commsize)) {
	
		left_m += range % commsize;
		right_m = left_m + range / commsize;
	} else {
	
		left_m += this_rank;
		right_m = left_m + range / commsize + 1;
	}
	//std::cout << range << std::endl;
	//std::cout << "Rank[" << this_rank << "]: " << left_m << " - " << right_m << std::endl;

	for (int m = left_m; m < right_m; m++) {

			u[1][m] = tau * (func_f(m * h, 0) - (u[0][m + 1] - u[0][m - 1]) / (2 * h) + (u[0][m + 1] + u[0][m - 1]) / (2 * tau));
	}

	MPI_Barrier(MPI_COMM_WORLD);

	for (int n = 1; n < N - 1; n++) {
		
		if (this_rank != 0)	MPI_Send(&u[n][left_m], ONE_ELEMENT, MPI_DOUBLE, this_rank - 1, TAG_2, MPI_COMM_WORLD);
		if (this_rank != commsize - 1) MPI_Send(&u[n][right_m - 1], ONE_ELEMENT, MPI_DOUBLE, this_rank + 1, TAG_2, MPI_COMM_WORLD);
		
		if (this_rank != 0)	MPI_Recv(&u[n][left_m - 1], ONE_ELEMENT, MPI_DOUBLE, this_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
		if (this_rank != commsize - 1) MPI_Recv(&u[n][right_m], ONE_ELEMENT, MPI_DOUBLE, this_rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);

		for (int m = left_m; m < right_m; m++) {

			u[n + 1][m] = 2 * tau * (func_f(m * h, n * tau) + (u[n][m + 1] - u[n][m - 1]) / (2 * h) + u[n - 1][m] / (2 * tau));
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

int main(int argc, char** argv) {

	MPI_Init(&argc, &argv);

	int commsize, this_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);

	if (this_rank == 0) std::cout << std::endl;

	std::vector<double> initical_cond(M);
	std::vector<double> bound_cond_l(N);
	std::vector<double> bound_cond_r(N);

	for (int i = 0; i < M; i++) initical_cond[i] = func_initical_cond(i * h);
	for (int i = 0; i < N; i++) bound_cond_l[i] = func_bound_cond_l(i * tau);
	for (int i = 0; i < N; i++) bound_cond_r[i] = func_bound_cond_r(i * tau);
	// Проверка соответствия?

	std::vector<std::vector<double>> u(N);
	for (int i = 0; i < N; i++) u[i].resize(M);
	//for (auto it : u) it.resize(M);

//	std::cout << "N: " << u.size() << ", M: " << u[0].size() << std::endl;
//	std::cout << "N: " << N << ", M: " << M << std::endl;

	u[0] = initical_cond;
	for (int i = 1; i < N; i++) {

		u[i][0] = bound_cond_l[i];
		u[i][M] = bound_cond_r[i];
	}

	calc_u(initical_cond, bound_cond_l, bound_cond_r, u);	
/*	
	if (this_rank == 2) {
		for (int n = 0; n < N; n++) {
			std::cout << n << ": " << u[n][10] << ", " << u[n][12] << ", " << u[n][14] << ", " << u[n][16] << std::endl; 
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	*/
	int range = M - 2;

	int left_m = this_rank * (range / commsize) + 1;
	int right_m;

	if (this_rank >= (range % commsize)) {
	
		left_m += range % commsize;
		right_m = left_m + range / commsize;
	} else {
	
		left_m += this_rank;
		right_m = left_m + range / commsize + 1;
	}

	int incremental = 0;

	for (int n = 1; n < N; n++) {

		if (this_rank != 0) {
		
			if (this_rank > 1) MPI_Recv(&incremental, ONE_ELEMENT, MPI_INT, this_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);

			MPI_Send(&u[n][left_m], right_m - left_m, MPI_DOUBLE, NULL_EXECUTOR, TAG_2, MPI_COMM_WORLD);
			//std::cout << "Rank[" << this_rank << "]: " << left_m << " - " << right_m << ":: ";
			//for (auto x_it : u[n]) std::cout << x_it << " ";
			//std::cout << std::endl;

			if (this_rank != commsize - 1) MPI_Send(&incremental, ONE_ELEMENT, MPI_INT, this_rank + 1, TAG_2, MPI_COMM_WORLD);
		}
		else {

			for (int cur_rank = 1; cur_rank < commsize; cur_rank++) {

				int cur_left = cur_rank * (range / commsize) + 1;
				int cur_right;

				if (cur_rank >= (range % commsize)) {
				
					cur_left += range % commsize;
					cur_right = cur_left + range / commsize;
				} else {
				
					cur_left += cur_rank;
					cur_right = cur_left + range / commsize + 1;
				}
				MPI_Recv(&u[n][cur_left], cur_right - cur_left, MPI_DOUBLE, cur_rank, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

//	MPI_File_delete("output.txt", MPI_INFO_NULL);
	if (this_rank == 0) {

		for (auto x_it : initical_cond) {

			//std::cout << x_it << " ";
//			std::cout << x_it << " ";
		}
		//std::cout << std::endl;
//		std::cout << std::endl;
		std::ofstream stream("output.txt");
		for (auto equal_time_it : u) {

			for (auto x_it : equal_time_it) {

				//std::cout << x_it << " ";
				stream << x_it << " ";
			}
			//std::cout << std::endl;
			stream << std::endl;
		}
	}

	MPI_Finalize();
}