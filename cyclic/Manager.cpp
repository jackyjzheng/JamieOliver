#include <vector>
#include <thread>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "RGraph.cpp"

class SearchManager {
private:
	int numThreads;
	int init_rNum;
	int init_tabuSize;
	bool search_running = false;
	bool run_again = false;
	std::vector<std::thread> search;

public:
	void end_search() {
		this->search_running = false;
	}

	void init_search() {
		do {
			this->run_again = false;
			this->search.reserve(this->numThreads);
			this->search_running = true;
			for (int i = 0; i < this->numThreads; ++i) {
				(this->search).push_back(std::thread(&SearchManager::search_loop, this));
			}
			for (auto& t : this->search) {
				t.join();
			}
			search.clear();
		} while (this->run_again);
	}

	void search_loop() {
		RGraph *rGraphSearch = new RGraph(this->init_rNum, this->init_tabuSize);
		while(this->search_running) {
			rGraphSearch->tabu_search(1000);
			rGraphSearch->reinitialize(this->init_rNum, this->init_tabuSize);
		}
	}

	void reinitialize(int init_rNum, int tabuSize, int numThreads) {
		this->numThreads = numThreads;
		this->init_rNum = init_rNum;
		this->init_tabuSize = tabuSize;
		this->run_again = true;
	}

	SearchManager(int init_rNum, int tabuSize, int numThreads) {
		this->numThreads = numThreads;
		this->init_rNum = init_rNum;
		this->init_tabuSize = tabuSize;
	}	
};

class IncrementManager {
private:
	int init_rNum;
	int init_tabuSize;
	std::thread increment; 
	int *color = NULL;
	bool increment_running = false;
	bool run_again = false;

public:
	void end_increment() {
		this->increment_running = false;
	}

	void init_increment(int *color) {
		this->color = color;
		do {
			increment = std::thread(&IncrementManager::increment_loop, this, this->color);
			increment.join();
		} while (this->run_again);	
	}

	void reinitialize(int rNum, int tabuSize, int *color) {
		this->init_rNum = rNum;
		this->init_tabuSize = tabuSize;
		this->color = color;
		this->run_again = true;
	}

	void increment_loop(int *color) {
		RGraph *rGraph = new RGraph(this->init_rNum, this->init_tabuSize, color);
		this->increment_running = true;
		bool incrementSuccess = false;
		while (this->increment_running) {
			rGraph->incrementNumber(2);
			rGraph->tabu_search(1000);
			if (this->increment_running && rGraph->getCliqueCount() != 0)
				rGraph->tabu_search(1000);
		}
	}

	IncrementManager(int init_rNum, int tabuSize) {
		this->init_rNum = init_rNum;
		this->init_tabuSize = tabuSize;
	}
};