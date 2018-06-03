#include <vector>

class TabuManager {
	int numThreads;
	std::vector<RGraph *> generators;

	TabuManager(int rNum, int tabuSize, int numThreads) {
		this->numThreads = numThreads;
	}
}

int main() {
	return 1;
}