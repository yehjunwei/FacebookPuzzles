#include <stdio.h>
#include <vector>
#include <queue>

using namespace std;

#define WALL -1
#define START_POINT 10
#define END_POINT 11

#define WHITE 0
#define GRAY 1
#define BLACK 2

struct Node {
	// store type, -1 ~ 11
	char type;
	// store adjacent nodes index
	vector<int> adjList;
	int d;
	char color;

	Node() {
		type = 0;
		adjList.clear();
		d = 0;
		color = WHITE;
	}
};

// prototypes
void setupGraph(Node *nodes, char **matrix, vector<int> *portals, int numRows, int numCols);
char setType(Node *node, char c);
int calcIndex(int row, int col, int numCols);
void setAdjList(Node *node, char **matrix, int i, int j, int numRows, int numCols);
void testAndAddAdjNode(Node *node, char **matrix, int row, int col, int numCols);
void addPortalAdjacency(Node *nodes, int numNodes, vector<int> *portals);

int main(int argc, char *argv[]) {

	FILE *fp = fopen(argv[1], "r");
	if(fp == 0)
		return 0;

	int numTestCases;
	fscanf(fp, "%d", &numTestCases);
	for(int i = 0 ; i < numTestCases ; ++i) {
		Node *nodes;
		// Read input matrix data
		int numRows, numCols;
		fscanf(fp, "%d %d", &numRows, &numCols);
		nodes = new Node [numRows*numCols];
		char **matrix = 0;
		matrix = new char* [numRows];
		for(int j = 0 ; j < numRows ; ++j) {
			matrix[j] = new char [numCols+1];
		}
		for(int row = 0 ; row < numRows ; ++row) {
			fscanf(fp, "%s", matrix[row]);
		}
		
		vector<int> *portals;
		portals = new vector<int> [END_POINT+1];
		// initialize graph in adjacency list structure
		setupGraph(nodes, matrix, portals, numRows, numCols);
		addPortalAdjacency(nodes, numRows*numCols, portals);

		// BFS
		int start_index = portals[START_POINT][0];
		int end_index = portals[END_POINT][0];

		queue<int> processQueue;
		nodes[start_index].color = GRAY;
		nodes[start_index].d = 0;
		processQueue.push(start_index);

		while(processQueue.size() > 0) {
			int now = processQueue.front();
			processQueue.pop();
			
			int numAdjs = nodes[now].adjList.size();
			for(int i = 0 ; i < numAdjs ; ++i) {
				int adj_index = nodes[now].adjList[i];
				char adj_color = nodes[adj_index].color;
				if(adj_color == WHITE) {
					nodes[adj_index].d = nodes[now].d + 1;
					nodes[adj_index].color = GRAY;
					processQueue.push(adj_index);
				}
			}
		}
		printf("%d\n", nodes[end_index].d);

		// recycle portals
		delete [] portals;

		// recycle nodes
		delete [] nodes;

		// recycle matrix
		for(int j = 0 ; j < numRows ; ++j) {
			delete [] matrix[j];
		}
		delete [] matrix;
	}

	fclose(fp);
	return 0;
}

char setType(Node *node, char c) {
	char type;
	if(c == 'W')
		type = WALL;
	else if(c == 'S')
		type = START_POINT;
	else if(c == 'E')
		type = END_POINT;
	else {
		type = c - '0';
	}
	node->type = type;
	return type;
}

int calcIndex(int row, int col, int numCols) {
	return row * numCols + col;
}

void testAndAddAdjNode(Node *node, char **matrix, int row, int col, int numCols) {
	if(matrix[row][col] != 'W') {
		int index = calcIndex(row, col, numCols);
		node->adjList.push_back(index);
	}
}
void setAdjList(Node *node, char **matrix, int i, int j, int numRows, int numCols) {
	if(j > 0)	// is left node adjacent?
		testAndAddAdjNode(node, matrix, i, j-1, numCols);
	if(i < numRows-1)	// is down node adjacent?
		testAndAddAdjNode(node, matrix, i+1, j, numCols);
	if(j < numCols-1)	// is right node adjacent?
		testAndAddAdjNode(node, matrix, i, j+1, numCols);
	if(i > 0)	// is up node adjacent?
		testAndAddAdjNode(node, matrix, i-1, j, numCols);
}

void setupGraph(Node *nodes, char **matrix, vector<int> *portals, int numRows, int numCols) {
	int index = 0;
	for(int i = 0 ; i < numRows ; ++i) {
		for(int j = 0 ; j < numCols ; ++j) {
			index = calcIndex(i, j, numCols);
			char type = setType(&nodes[index], matrix[i][j]);
			// store portal index, including start and end point
			if(type > 0)
				portals[type].push_back(index);
			if(nodes[index].type != WALL)
				setAdjList(&nodes[index], matrix, i, j, numRows, numCols);
		}
	}
}

void addPortalAdjacency(Node *nodes, int numNodes, vector<int> *portals) {
	for(int i = 0 ; i < numNodes ; ++i) {
		char type = nodes[i].type;
		if(type > 0 && type < 10) {
			for(int j = 0 ; j < (int)portals[type].size() ; ++j) {
				// if index is self, do not add self into adjacency list
				if(portals[type][j] == i)
					continue;
				int index = portals[type][j];
				nodes[i].adjList.push_back(index);
			}
		}
	}
}