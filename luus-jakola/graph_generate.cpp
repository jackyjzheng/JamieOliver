#include <unordered_map>
#include <queue>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include "CliqueCount.c"

/*
* Since I'm literally too lazy to write a two-line makefile:
*	compile with g++ -std=c++11 -o jakola jakola.cpp
*/
class RGraphGenerator {
public:
	/* Initialization:
	*	r_num=current ramsey number
	*	graph=int* array encoding upper right hand triangle of adj_matrix, these are the edges we flip
	*	g_size=number of elements in graph, is equal to sum(1->r_num-1)
	*	adj_graph=int* array encoding entire adjacency matrix for CliqueCount
	*	local_min_intensity=variable counting how long we've stayed at the same number. Used to try and climb out of local minimum
	*/
	void initialize(int r_num) {
		printf("===================================\n");
		printf("Initialize with Ramsey Number: %i.\n", r_num);
		printf("===================================\n");
		
		this->r_num = r_num;
		this->adj_graph = new int[r_num*r_num];

		int g_size = 0;
		for (int i = 0; i < r_num; ++i)
			g_size += i;
		this->g_size = g_size;
		this->graph = new int[this->g_size];
		
		for (int i = 0; i < r_num*r_num; ++i) {
			(this->adj_graph)[i] = rand() % 2;
		}
	
		for (int i = 0; i < g_size; ++i) {
			int color = rand() % 2;
			(this->graph)[i] = color;
			(this->adj_graph)[index_from_triangle(i)] = color;
		}
	}

	void reinitialize(int r_num) {
		delete[] graph;
		delete[] adj_graph;
		initialize(r_num);
	}

	/*
	* Converts index from *graph to index of *adj_graph
	* --> *graph only encodes upper right triangle which are the only edges we want to flip,
	*		but CliqueCount needs to take in the whole matrix.
	*		So everytime we flip (this->graph)[i] we also flip (this->adj_graph)[index_from_triangle(i)]
	*/
	int index_from_triangle(int triangle_index) {
		int row = 1;
		
		int counter = 0;
		int n_elems_in_row = (this->r_num) - row;
		while (counter + n_elems_in_row <= triangle_index)
		{	
			row += 1;
			counter += n_elems_in_row;
			n_elems_in_row -= 1;
		}

		int distance = 0;
		for (int row_counter = 1; row_counter <= row; ++row_counter)
			distance += row_counter;
		return (triangle_index + distance);
	}

	RGraph(int r_num) {
			initialize(r_num);
	}

	~RGraph() {
		printf("Destructor called.\n");
		delete[] graph;
		delete[] adj_graph;
	}

	/*
	*	Calculates number of edges that are of different color than (this->graph)[index1] from index1 to index2
	*/
	int distance(int index1, int index2) {
		int counter = 0;
		int initial_color = (this->graph)[index1];
		for (int i = index1+1; i <= index2; ++i)
		{
			if ((this->graph)[i] != initial_color)
				counter += 1;
		}
		return counter;
	}

	void increment_and_run(int increment_value) {
		this->next_number(increment_value);
		this->luus_jaakola(0, this->r_num-1);
	}

	/*
	*	http://ijmcs.future-in-tech.net/10.1/R-MangeDunn.pdf
	*	Edge flipping algorithm: search_space_a and search_space_b are indices where we will flip edge.
	*	--> Paper says important parameters are d,q where for R(5,5) q~=.9975 optimal and d~=3 optimal
	*			From what I'm seeing these values are kinda useless, going to write test program to attempt to find optimal values.
	*		d is a double where round(d) is how many edges we are going to flip
	*		q is the shrinking parameter. If CliqueCount does not go down d = d*q. If round(d) == 0, we pick new random d
	*	Right now we use color distance to calculate d, no real reason just seems to give a good value I think.
	*	
	*	std::queue<int> edges_flipped so store edges we flip in case we need to flip them back
	*	std::unordered_map<int, bool> already_flipped to remember if we've already flipped this turn. Need to fix after some bs. I did.
	*/
	int luus_jaakola(int search_space_start=0, int search_space_end=-1, int cycle_limit=-1, std::string file_name_test="") {
		int cycles = 0;

		if (search_space_end == -1 || search_space_end > this->g_size-1) {
			search_space_end = this->g_size-1;
		}
		printf("Initializing Luus-Jaakola with search space between indices %i and %i.\n", search_space_start, search_space_start+search_space_size-1);

		int x = rand() % this->g_size;
		
		// Look into these parameters
		int initial_d = distance(0, this->g_size) % (search_space_end+1);
		double d = initial_d;
		double q = .925;
		printf("Distance calculated is %i.\n", initial_d);
		
		printf("Beginning initial CliqueCount...\n");
		int cliqueCount = CliqueCount(this->adj_graph, this->r_num);
		
		printf("Initial CliqueCount = %i\n", cliqueCount);
		while (cliqueCount != 0) {
			int a = (rand()) % (((int) round(d)) + 1);
			//int y = a + x;

			// Temporarily not caring if we re-flip the edge
			std::queue<int> edges_flipped;
			bool flip_back = false;
			for (int i = 0; i < a; ++i) {
				int flip =  (rand() + search_space_start) % search_space_end;
				edges_flipped.push(flip);
				
				(this->graph)[flip] = (this->graph[flip] + 1) % 2;
				int adj_index = index_from_triangle(flip);
				(this->adj_graph)[adj_index] = ((this->adj_graph)[adj_index] + 1) % 2;
			}

			int newCliqueCount = CliqueCountLimit(this->adj_graph, this->r_num, cliqueCount);
			if (newCliqueCount <= cliqueCount) {
				if (cliqueCount == newCliqueCount) {
					printf("CliqueCount stayed the same, but edges flipped. Current Ramsey Number: %i. Current Cycle: %i\n", this->r_num, cycles);
				} else {
					cliqueCount = newCliqueCount;
					printf("CliqueCount improved, now %i. Current Ramsey Number: %i. Current Cycle: %i\n", cliqueCount, this->r_num, cycles);
				}
			} else {
				flip_back = true;
				d = q * d;
				while (round(d) == 0) {
					initial_d = distance(0, this->g_size) % (search_space_b-1);
					d = initial_d;
					search_space_b = rand() % (this->g_size-1);
					//x = rand() % this->g_size;
				}	
				printf("CliqueCount did not improve, still %i, d is now %f. Current Ramsey Number: %i. Local min. counter: %i. Current Cycle: %i\n", cliqueCount, d, this->r_num, local_min_count, cycles);
			}

			while (!edges_flipped.empty()) {
				int top = edges_flipped.front();
				if (flip_back)
				{	
					(this->graph)[top] = ((this->graph)[top] + 1) % 2;
				}
				edges_flipped.pop();
			}
			cycles += 1;
			if (cycle_limit != -1 && cycles >= cycle_limit)
				return -1;
		}
		
		printf("CliqueCount reached 0, outputing to file.\n");
		std::string file_name = std::to_string(this->r_num) + ".txt";
		write_graph(file_name.c_str());
		printf("\n");
		return cycles;
	}

	/*
	*	Function to move on to next ramsey number. Can currently jump either 1 or 2.
	*	Most of this is basically copying old_adj_matrix and old_graph to new_adj_matrix and new_graph	
	* 	For example: (moving up by 1)
	*	xxx	  					                    	
	*	xab = new_adj_graph		ab = old_adj_graph   xxxabd = new_graph   abd = old_graph 
	*	xcd						cd
	*	--> Once new graphs are set, Luus-Jaakola algorithm is called again to find 0 clique of new graph.
	*/
	void next_number(int jump_number) {
		while (jump_number != 0) {
			this->r_num += 1;
			this->local_min_intensity = 0;

			printf("===================================\n");
			printf("Moving on to Ramsey Number: %i.\n", this->r_num);
			printf("===================================\n");

			int *new_adj_graph = new int[this->r_num*this->r_num];
			int old_g_size = this->g_size;
			this->g_size  += (this->r_num)-1;
			int *new_graph = new int[this->g_size];

			// Fill in new adjacency matrix
			int old_adj_counter = 0;
			for (int i = 0; i < (this->r_num)*(this->r_num); ++i) {
				int color = rand() % 2;
				if (i < this->r_num || (i % this->r_num) == 0) {
					new_adj_graph[i] = color;
					if (i < this->r_num && i != 0) 
						new_graph[i-1] = color;
				} else {
					new_adj_graph[i] = (this->adj_graph)[old_adj_counter];
					old_adj_counter += 1;
				}
			}

			for (int i = 0; i < old_g_size; ++i) 
				new_graph[i+((this->r_num)-1)] = (this->graph)[i]; 
			
			delete[] this->graph;
			delete[] this->adj_graph;
			this->graph = new_graph;
			this->adj_graph = new_adj_graph;
			
			jump_number -= 1;
		}
	}

	void print_graph() {
		for (int i = 0; i < r_num * r_num; ++i) {
			printf("%i", this->adj_graph[i]);
		}
		printf("\n");
	}

	/*
	*	Write out adjacency matrix into file
	*/
	void write_graph(const char *file_name, bool adj=1) {
		std::ofstream outFile(file_name);
		outFile << "RAMSEY NUMBER: " << this->r_num << std::endl;
		if (adj) {
			int adj_size = (this->r_num) * (this->r_num);
			for (int i = 0; i < adj_size; ++i)
				outFile << (this->adj_graph)[i]; 
		} else {
			for (int i = 0; i < this->g_size; ++i)
				outFile << (this->graph)[i];
		}
	}
	
	int rate_graph_freshness(int start_r_num, int iterations) {
		this->reinitialize(start_r_num);
		int *graph_seed = new int[this->g_size];
		for (int i = 0; i < this->g_size; ++i)
			graph_seed[i] = (this->graph)[i];

		int counter = 1;
		bool stop = false;
		
		int cycle_return = this->luus_jaakola(0, this->g_size-1, 50000);
		if (cycle_return == -1)
			stop = true;
	
		while (!stop) {
			this->next_number(2);
			cycle_return = this->luus_jaakola(0, this->g_size-1, 50000);
			counter += 2;
			if (cycle_return == -1)
				stop = true;
		}
		
		printf("===========================================\n");
		printf("GRAPH_RATING IS %i.\n", counter);
		printf("===========================================\n");

		//this->write_graph("", 0);
		return counter;
	}

private:
	int *adj_graph;
	int *graph;
	int g_size;
	int r_num;
};

int main(int argc, char **argv) {
	srand(time(NULL));
	if (argc != 2) {
		printf("./jakola ramsey_number\n");
		return 0;
	}
	int r_num = atoi(argv[1]);
	RGraph test = RGraph(r_num);
	//test.rate_parameters(r_num, 10000);
	//while (true) {
	//	int graph_rating = test.rate_graph_freshness(r_num);
	//}
	//test.luus_jaakola();
	while (true) {
		test.increment_and_run(3);
	}
}