#include "CliqueCountOG.c"
#include <stdlib.h>
#include <fstream>

using namespace std;
int main(int argc, char **argv) {
	if (argc != 2) {
		printf("./checker ramsey_number\n");
		return 0;
	}

	int r_num = atoi(argv[1]);
	int size = r_num * r_num;
	//for (int i = 0; i < r_num; ++i)
	//	size += i;
	int *graph = new int[size];
	std::ifstream readFile("ramsey_graph.txt", fstream::in);
	char ch;
	int counter = 0;
	while (readFile >> noskipws >> ch) {
		int color;
		if (ch == '0')
			color = 0;
		else
			color = 1;
		graph[counter] = color;
		counter += 1;
	}
	printf("Doing checker with r_num = %i.\n", r_num);
	int cliques = CliqueCountOG(graph, r_num);
	printf("Checker shows that CliqueCount=%i.\n", cliques);
	//for (int i = 0; i < size; ++i)
	//	printf("%i", graph[i]);
	//printf("\n");	
	delete[] graph;
}