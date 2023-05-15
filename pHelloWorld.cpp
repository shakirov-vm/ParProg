#include <pthread.h>
#include <iostream>
#include <vector>

void* helloworld(void* num) {

	std::cout << "Helloworld, this is " << *(int*) num << " executor" << std::endl;
	return NULL;
}

int main(int argc, char* argv[]) {
	
	int executors = 4;
	std::vector<pthread_t> pid(executors);

	for (int i = 0; i < executors; i++) {

		pthread_create(&pid[i], NULL, helloworld, &i);
		pthread_join(pid[i], NULL);
	}
}