#include <thread>
#include <vector>
#include <stdlib.h>

void print_int(int a, int b, int c) {
	bool signal = true;
	do {
		printf("Printing values a=%i, b=%i, c=%i\n", a, b, c);
	} while(signal);
}


int main() {
	int numThreads = 4;
	std::vector<std::thread> thread_pool;
	thread_pool.reserve(numThreads);
	for (int i = 0; i < numThreads; ++i) {
		thread_pool.push_back(std::thread(print_int, 1, 2, 3));
	}

	for (auto& t : thread_pool) {
    	t.join();
	}
}