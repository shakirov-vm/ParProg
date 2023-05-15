#include <stack>
#include <cmath>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <iostream>

struct stack_entry {
	
	double A;
	double B;
	double f_A;
	double f_B;
	double prev_J;

	stack_entry(double A_, double B_, double f_A_, double f_B_, double J_AB_) : 
					A(A_), B(B_), f_A(f_A_), f_B(f_B_), prev_J(J_AB_) {}
};

// 0.6774542641093082 << true result

struct routine_entry {

	int num;
	double* part_J;
};

int executors;
double eps;

double f(double x) {

	return cos(1 / (x - 2));
}

double left_bound = 0;
double right_bound = 1.995;

void* integrate_routine(void* re) {

	routine_entry* rout_ent = (routine_entry*) re; 
	int rank = rout_ent->num;

	std::stack<stack_entry> local_stack;

	double J = 0;

	double A = left_bound; // ??
	double B = right_bound; // ??
	double f_A = f(A);
	double f_B = f(B);
	double J_AB = (f_A + f_B) * (B - A) / 2;

	while(true) {

		double C = (A + B) / 2;
		double f_C = f(C);
		double J_AC = (f_A + f_C) * (C - A) / 2;
		double J_CB = (f_C + f_B) * (B - C) / 2;
		double J_ACB = J_AC + J_CB;

		if(std::abs(J_AB - J_ACB) >= eps * std::abs(J_ACB))	{

			local_stack.emplace(stack_entry(A, C, f_A, f_C, J_AC));
			A = C;
			f_A = f_C;
			J_AB = J_CB;
		} else {
			
			J += J_ACB;
			if (local_stack.empty()) break;

			auto entry = local_stack.top();
			A = entry.A;
			B = entry.B;
			f_A = entry.f_A;
			f_B = entry.f_B;
			J_AB = entry.prev_J;
			local_stack.pop();
		}
	}
	*(rout_ent->part_J) = J;

	return NULL;
}

int main(int argc, char** argv) {

	executors = std::atoi(argv[1]);
	eps = std::atof(argv[2]);

	std::vector<pthread_t> pid(executors);
	std::vector<double> part_J(executors);

	std::vector<routine_entry> results(executors);
	for (int i = 0; i < executors; i++) {

		results[i].num = i;
		results[i].part_J = &part_J[i];
	}

	for (int i = 0; i < executors; i++) pthread_create(&pid[i], NULL, integrate_routine, &results[i]);
	for (int i = 0; i < executors; i++) pthread_join(pid[i], NULL);

	std::cout << *(results[0].part_J) << std::endl;
}