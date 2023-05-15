#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

#define X_MAX_LEN 3.1416
#define T_MAX_TIME 1
#define h 0.15
#define tau 0.1

int M = static_cast<int>(X_MAX_LEN / h);
int N = static_cast<int>(T_MAX_TIME / tau);

double func_initical_cond(double x) {
	return sin(x);
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


int main() {

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

	std::cout << "N: " << u.size() << ", M: " << u[0].size() << std::endl;
	std::cout << "N: " << N << ", M: " << M << std::endl;

	u[0] = initical_cond;
	for (int i = 1; i < N; i++) {

		u[i][0] = bound_cond_l[i];
		u[i][M] = bound_cond_r[i];
	}
	for (int n = 0; n < N - 1; n++) {
		
		for (int m = 1; m < M - 1; m++) {

			u[n + 1][m] = tau * (func_f(m * h, n * tau) - (u[n][m] - u[n][m - 1]) / h + u[n][m] / tau);
		}
	}

	std::ofstream stream("output.txt");
	for (auto equal_time_it : u) {

		for (auto x_it : equal_time_it) {

			stream << x_it << " ";
		}
		stream << std::endl;
	}
}