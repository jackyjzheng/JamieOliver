#include <unordered_map>
#include <queue>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CliqueCount.c"

class RGraph {
public:
	/* Initialization:
	*	r_num=
	*	graph=
	*	g_size=
	*	adj_graph=
	*	local_min_intensity=
	*/
	void initialize(int r_num) {
		printf("===================================\n");
		printf("Initialize with Ramsey Number: %i.\n", r_num);
		printf("===================================\n");
		
		this->r_num = r_num;
		this->adj_graph = new int[r_num*r_num];
		this->local_min_intensity = 0;

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
	void luus_jaakola(int search_space_a=0, int search_space_b=-1) {
		if (search_space_b == -1) {
			search_space_b = this->g_size-1;
		}
		printf("Initializing Luus-Jaakola with search space between indices %i and %i.\n", search_space_a, search_space_b);

		int x = rand() % this->g_size;
		int initial_d = distance(0, this->g_size) % (search_space_b+1);
		printf("Distance calculated is %i.\n", initial_d);
		double d = initial_d;
		//double d = 500;
		//.75 is pretty good double q = .75;
		double q = .985;
		
		printf("Beginning initial CliqueCount...\n");
		int cliqueCount = CliqueCount(this->adj_graph, this->r_num);
		
		printf("Initial CliqueCount = %i\n", cliqueCount);
		int local_min_count = 0;
		while (cliqueCount != 0) {
			int a = (rand() % (int) (round(d))) + 1;
			std::queue<int> edges_flipped;
			std::unordered_map<int, bool> already_flipped;
			
			bool flip_back = false;
			for (int i = 0; i < a; ++i) {
				int flip = (rand() % (search_space_b+1)) + search_space_a;
				//while (already_flipped[flip] == true) { <-- Need to fix
				//	flip = (rand() % (search_space_b+1)) + search_space_a;
				//}
				
				already_flipped[flip] = true;
				edges_flipped.push(flip);
				
				(this->graph)[flip] = (this->graph[flip] + 1) % 2;
				int adj_index = index_from_triangle(flip);
				(this->adj_graph)[adj_index] = ((this->adj_graph)[adj_index] + 1) % 2;
			}

			int newCliqueCount = CliqueCountLimit(this->adj_graph, this->r_num, cliqueCount);
			if (newCliqueCount <= cliqueCount) {
				if (cliqueCount == newCliqueCount) {
					printf("CliqueCount stayed the same, but edges flipped. Current Ramsey Number: %i\n", this->r_num);
				} else {
					cliqueCount = newCliqueCount;
					local_min_count = 0;
					printf("CliqueCount improved, now %i. Current Ramsey Number: %i\n", cliqueCount, this->r_num);
				}
			} else {
				flip_back = true;
				d = q * d;
				while (round(d) == 0) {
					initial_d = distance(0, this->g_size) % (search_space_b-1);
					d = initial_d;
					search_space_b += search_space_b; //<--maybe iteratively try new search space
					// sometimes try uphill climb to get out of local min
					local_min_count += 1;
					if (search_space_b > (this->g_size-1))
						search_space_b = rand() % (this->g_size-1);
					if (local_min_count > 15) {
						printf("Attempting to leave local min. Local min intensity: %i\n", this->local_min_intensity);
						cliqueCount = leave_local_minima(cliqueCount);
						flip_back = false;	
					}
				}
				printf("CliqueCount did not improve, still %i, d is now %f. Current Ramsey Number: %i. Local min. counter: %i\n", cliqueCount, d, this->r_num, local_min_count);
			}

			while (!edges_flipped.empty()) {
				int top = edges_flipped.front();
				already_flipped[top] = false;
				if (flip_back)
				{	
					(this->graph)[top] = ((this->graph)[top] + 1) % 2;
				}
				edges_flipped.pop();
			}
		}
		printf("CliqueCount reached 0, outputing to file.\n");
		std::string file_name = std::to_string(this->r_num) + ".txt";
		write_graph(file_name.c_str());
		printf("\n");
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
		
		// only working for jump 2
		jump_number -= 1;
		if (jump_number != 0)
			(this->next_number)(1);
		else
			return;
		
		this->luus_jaakola(0, this->r_num-1);
	}

	/*
	*	Function to try and leave local minimum. Not the best at the moment but w/e.
	*	Luus-Jaakola algorithm will only flip edges if the CliqueCount goes down. If CliqueCount is very small and we are at a local min, edges will never flip.
	*	Therefore once condition is met, this function is called and some number of edges are flipped regardless of CliqueCount going up or down.
	*	If we are stuck at the same ramsey_number for a long time --> local_min_intensity goes up and more edges flipped.
	*/

	int leave_local_minima(int currentCliqueCount) {
		local_min_intensity += 1;
		bool found_close_graph = false;

		printf("Randomly modifying edges.\n");
		int edges_to_change = 2*local_min_intensity;
		if (edges_to_change > this->g_size)
		{
			edges_to_change = (this->g_size)/2;
			local_min_intensity = 0;
		}
		for (int i = 0; i < edges_to_change; ++i) {
			int edge = rand() % this->g_size;
			(this->graph)[edge] = ((this->graph)[edge] + 1) % 2;
			(this->adj_graph)[index_from_triangle(edge)] = ((this->adj_graph)[index_from_triangle(edge)] + 1) % 2;
		}
		printf("Finished modifying edges.\n");
		return (CliqueCount(this->adj_graph, this->r_num));
	}


	void print_graph() {
		//printf("PRINTING GRAPH...");
		for (int i = 0; i < r_num * r_num; ++i) {
			printf("%i", this->adj_graph[i]);
		}
		printf("\n");
	}

	/*
	* WIP
	*
	*/
	void encode_graph() {
		this->encoded_graph = new short[this->g_size];
		int counter = 0;	
		for (int i = 0; i < this->g_size; ++i) {
			if ((this->graph)[i]) {
				(this->encoded_graph)[counter] = (short) i;
				counter += 1;
			}
		}
		if (counter != this->g_size)
				(this->encoded_graph)[counter] = -1;
	}

	void manipulate_new_edges() {
		return;
	}

	/*
	*	Write out adjacency matrix into file
	*/
	void write_graph(const char *file_name) {
		std::ofstream outFile(file_name);
		int adj_size = this->r_num * this->r_num;
		outFile << "RAMSEY NUMBER: " << this->r_num << std::endl;
		for (int i = 0; i < adj_size; ++i)
		{	
			//printf("%i", (this->adj_graph)[i]);
			outFile << (this->adj_graph)[i]; 
		}
		//printf("\n");
	}

	/*int write_encoded_graph(const char *file_name) {
		std::ofstream outputFile;
		outputFile.open(file_name);
		for (int i = 0; i < this->eg_size; ++i)
			outputFile.write((char *));
		outputFile.close();
	}*/

private:
	int *adj_graph;
	int *graph;
	int g_size;
	short *encoded_graph;
	int local_min_intensity;
	int eg_size;	
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
	test.luus_jaakola();
	while (true) {
		test.next_number(2);
	}
	//test.encode_graph();
	//int output = test.write_encoded_graph("r.dat");
	//test.next_number();
}