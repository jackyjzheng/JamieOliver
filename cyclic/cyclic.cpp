#include <stdlib.h>
#include <time.h>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <thread>
#include "CliqueCount.c"

struct TabuList {
	TabuList(int maxTabuSize)
	{
		this->maxTabuSize = maxTabuSize;
		this->tabu_elements = new std::unordered_map<int, bool>();
		this->tabu_fifo = new std::queue<int>();
	}

	bool isTabu(int element) {
		return !(this->tabu_elements->find(element) == this->tabu_elements->end());
	}

	int tabuInsert(int element) {
		if (this->tabu_fifo->size() == this->maxTabuSize)
		{	
			(*(this->tabu_elements))[(this->tabu_fifo)->front()] = false;
			this->tabu_fifo->pop();
		}
		(*(this->tabu_elements))[element] = true;
		this->tabu_fifo->push(element);
	}

	int maxTabuSize;
	std::unordered_map<int, bool> *tabu_elements;  
	std::queue<int> *tabu_fifo; 
};

class RGraph {
	/*
	*	Ramsey graph that utilizes Tabu search on cyclic graphs as search algorithm.
	*	-->	graph: Upper right hand triangle of adjacency matrix.
	*		adjGraph: Adjacency matrix, diagonals and lower right are all 0s.
	*		colors:	n/2 colors that defines the graph. Basically color[0] = every edge where vertices are 1 apart, color[2] = every edge where vertices are 2 apart etc..
	*			--> Honestly don't even need graph anymore, just colors to change edges, and adjGraph for CliqueCount
	*/

private:
	int *graph;
	int *colors;
	int *adjGraph;
	TabuList *tabu_list;
	int gSize;
	int rNum;

public:
	/*
	*	Initializes the graphs, colors are either randomly chosen or copied from and input array.
	*/
	void initialize(int rNum, int tabuSize, int *inputColors=NULL) {
		srand(time(NULL));
		this->rNum = rNum;
		this->gSize = 0;
		for (int i = 0; i < rNum; ++i)
			this->gSize += i;
		this->graph = new int[this->gSize];
		this->colors = new int[rNum/2];
		this->adjGraph = new int[rNum*rNum];
		this->tabu_list = new TabuList(tabuSize);

		int num_colors = rNum/2;
		if (inputColors == NULL) {
			for (int i = 0; i < num_colors; ++i)
				(this->colors)[i] = rand() % 2;
		} else {
			for (int i = 0; i < num_colors; ++i)
				(this->colors)[i] = inputColors[i];
		}

		this->create_graph();
		this->create_adj_graph();
	}

	/*
	*	Reinitialize to start over.
	*/
	void reinitialize(int rNum, int tabuSize) {
		delete[] graph;
		delete[] colors;
		delete[] adjGraph;
		delete tabu_list;
		this->initialize(rNum, tabuSize);
	}

	RGraph(int rNum, int tabuSize, int *inputColors=NULL) {
		if (inputColors != NULL)
			this->initialize(rNum, tabuSize, inputColors);
		else
			this->initialize(rNum, tabuSize);
	}

	~RGraph() {
		delete[] graph;
		delete[] colors;
		delete[] adjGraph;
		delete tabu_list;
	}

	/*
	*	At every iteration we go through every element in color[] array. Flip a color[], and check the CliqueCount. We store the flip that gives the best
	*	CliqueCount and only flip that edge. We also Tabu that index for TabuSize moves.
	*	When we reach 0 we write the color array into a text file.
	*/
	void tabu_search(int iterations=-1) {
		bool running = true;
		int moves = 0;
		int best_move = 0;
		int lowestClique = -1;

		while (running) {
			if (iterations != -1 && moves > iterations) {
				running = false;
				lowestClique = pick_new_color();
			}
			
			moves += 1;
			lowestClique = pick_new_color();

			if (lowestClique == -1) {
				this->reinitialize(this->rNum, (this->tabu_list)->maxTabuSize);
				moves = 0;
				best_move = 0;
				lowestClique = -1;
				continue;
			}
			else if (lowestClique == 0)
			{	
				std::string fileName = std::to_string(this->rNum) + ".txt"; 
				//this->write_adj_graph(fileName.c_str());
				this->write_color_graph(fileName.c_str());
				return;
			}
		}
		return;
	}

	/*
	*	Procedure to pick a new color. CliqueCountLimit works by exiting early if # of cliques exceed the input number, so we can leave early if we know CliqueCount will be too high.
	*/
	int pick_new_color() {
		int best_index = -1;
		int num_colors = (this->rNum)/2;

		// Select first element as the lowest, so you can make uphill moves
		flip_color(0);
		int lowestClique = CliqueCount(this->adjGraph, this->rNum);
		flip_color(0);
		
		for (int i = 1; i < num_colors; ++i) {
			if ((this->tabu_list)->isTabu(i))
				continue;

			flip_color(i);

			int currentClique = CliqueCountLimit(this->adjGraph, this->rNum, lowestClique);
			if (currentClique <= lowestClique)
			{	
				printf("Flipping edge %i will decrease CurrentClique=%i to %i.\n", i, lowestClique, currentClique);
				if (currentClique == 0) {
					printf("CliqueCount has reached 0 with index: %i\n", i);
					return 0;
				}
				best_index = i;
				lowestClique = currentClique;
			}

			flip_color(i);
		}
		
		if (best_index == -1) {
			printf("No edges will improve the CliqueCount.\n");
			return -1;
		}

		(this->tabu_list)->tabuInsert(best_index);
		flip_color(best_index);
		printf("New lowest number of cliques is %i, flipping edge %i.\n", lowestClique, best_index);
		return lowestClique;	
	}

	// Increment in multiples of 2 to make sure we are adding a new color
	void incrementNumber(int count) {
		printf("CliqueCount before increment is %i.\n", CliqueCount(this->adjGraph, this->rNum));

		int old_r_num = this->rNum;
		this->rNum += 2*count;
		
		int *new_colors = new int[this->rNum*this->rNum];
		for (int i = 0; i < count; ++i)
			new_colors[i] = rand() % 2;
		for (int i = 0; i < old_r_num/2; ++i)
			new_colors[count+i] = this->colors[i];

		delete[] this->colors;
		this->colors = new_colors;
		
		delete[] this->graph;
		for (int i = old_r_num; i < this->rNum; ++i)
			this->gSize += i;
		this->graph = new int[this->gSize];

		delete[] this->adjGraph;
		this->adjGraph = new int[this->rNum*this->rNum];

		this->create_graph();
		this->create_adj_graph();
		printf("We have incremented to RamseyNumber: %i with CliqueCount: %i.\n", this->rNum, CliqueCount(this->adjGraph, this->rNum));
	}

	// Color number starts from one
	void fill_color(int color_number) {
		int num_colors = (this->rNum)/2;
		int num_rows = (this->rNum)-1;

		int distance = 2 * (num_colors - color_number);
		if (this->rNum % 2 == 1)
			distance += 1;
		
		int color_index = color_number-1;
		int graph_index = color_index;
		
		for (int row = 1; row <= num_rows; ++row) {
			int n_elems_in_row = (this->rNum) - row;    // 5 4 3 2 1
			if (color_number <= n_elems_in_row) {
				(this->graph)[graph_index] = (this->colors)[color_index]; // switch from 1 indexing to 0 indexing for colors
				(this->adjGraph)[get_adj_index(graph_index)] = (this->colors)[color_index];
			}
			if (color_index + distance < n_elems_in_row) {
				(this->graph)[graph_index + distance] = (this->colors)[color_index];
				(this->adjGraph)[get_adj_index(graph_index + distance)] = (this->colors)[color_index];
			}
			graph_index += n_elems_in_row;
		}
	}

	void create_graph() {
		int num_colors = (this->rNum)/2;
		for (int i = 0; i < num_colors; ++i)
			fill_color(i+1);
	}

	void create_adj_graph() {
		int row = 1;
		int num_rows = (this->rNum)-1;
		
		int graph_index = 0;
		int adj_counter = 0;
		
		while (row <= num_rows) {
			for (int i = 0; i < row; ++i) {
				(this->adjGraph)[adj_counter] = 0;
				adj_counter += 1;
			}
			
			int n_elems_in_row = (this->rNum) - row;
			for (int i = 0; i < n_elems_in_row; ++i) {
				(this->adjGraph)[adj_counter] = (this->graph)[graph_index];
				graph_index += 1;
				adj_counter += 1;
			}
			row += 1;
		}	
	}

	int get_adj_index(int gIndex) {
		int row = 1;
		int counter = 0;
		int n_elems_in_row = (this->rNum) - row;
		
		while (counter + n_elems_in_row <= gIndex) {	
			row += 1;
			counter += n_elems_in_row;
			n_elems_in_row -= 1;
		}

		int distance = 0;
		for (int row_counter = 1; row_counter <= row; ++row_counter)
			distance += row_counter;
		return (gIndex + distance);
	}

	void flip_color(int color_index) {
			if (this->colors[color_index] == 1)
				this->colors[color_index] = 0;
			else
				this->colors[color_index] = 1;
			fill_color(color_index+1);
	}	

	void write_adj_graph(const char *file_name) {
		std::ofstream outFile(file_name);
		int r_num = this->rNum;
		outFile << r_num << std::endl;
		
		int adj_size = (r_num) * (r_num);
		for (int i = 0; i < adj_size; ++i)
				outFile << (this->adjGraph)[i]; 
	}	

	void write_color_graph(const char *file_name) {
		std::ofstream outFile(file_name);
		int r_num = this->rNum;
		outFile << r_num << std::endl;
		
		int numColors = r_num/2;
		for (int i = 0; i < numColors; ++i)
				outFile << (this->colors)[i]; 
	}
};

void tabu_loop_thread(int rNum, int tabuSize) {
	RGraph *search = new RGraph(rNum, tabuSize);
	search->tabu_search();
	//search->write_color_graph("colors_dir/color.txt");
	//search->read_color_graph("colors_dir/color.txt");
}
int *read_adj_graph(const char *file_name) {
	std::string rNum;
	std::string graph;
	std::ifstream myfile(file_name);
	
	if (myfile.is_open()) {
		getline(myfile, rNum);
		getline(myfile, graph);
		myfile.close();
	} else {
		printf("Unable to open file.\n");
		return NULL;
	}

	int ramseyNum = std::stoi(rNum);
	int numColors = ramseyNum / 2;
	int *color_graph = new int[numColors];
	printf("RAMSEY NUMBER: %i.\n", ramseyNum);
	for (int i = 1; i < numColors+1; ++i) {
		if (graph[i] == '0')
			color_graph[i-1] = 0;
		else
			color_graph[i-1] = 1;
		printf("%i", color_graph[i-1]);
	}
	printf("\n");
	return color_graph;
}

int* read_color_graph(const char *file_name) {
		std::string rNum;
		std::string graph;
		std::ifstream myfile(file_name);
		
		if (myfile.is_open()) {
			getline(myfile, rNum);
			getline(myfile, graph);
			myfile.close();
		} else {
			printf("Unable to open file.\n");
			return NULL;
		}

		int ramseyNum = std::stoi(rNum);
		int numColors = ramseyNum / 2;
		int *color_graph = new int[numColors];
		printf("RAMSEY NUMBER: %i.\n", ramseyNum);
		for (int i = 0; i < numColors; ++i) {
			if (graph[i] == '0')
				color_graph[i] = 0;
			else
				color_graph[i] = 1;
			printf("%i", color_graph[i]);
		}
		printf("\n");
		return color_graph;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("./cyclic ramsey_number tabu_size\n");
		return 0;
	}
	
	int rNum = atoi(argv[1]);
	int tabuSize = atoi(argv[2]);
	
	while (true) {
		RGraph *rGraph = new RGraph(rNum, tabuSize);
		rGraph->tabu_search();
	}


	/* For incrementing from known graph
	int *colors = read_adj_graph("329.txt");
	RGraph *rGraph = new RGraph(rNum, tabuSize, colors);
	while (true) {
		rGraph->incrementNumber(2);
		rGraph->tabu_search();
	}*/
	/*int numThreads = 4;
	std::vector<std::thread> thread_pool;
	thread_pool.reserve(numThreads);
	for (int i = 0; i < numThreads; ++i) {
		thread_pool.push_back(std::thread(tabu_loop_thread, rNum, tabuSize));
	}*/
	
	return 0;
}