#ifndef _TOPOLOGY_H_
#define _TOPOLOGY_H_

typedef struct road {
	int id;
	int cost;
	struct road* next;
} road;

class Topology {
public:
	int nr;
	road *root;

	Topology() : nr(0), root(NULL) {}
	
	void add_node(int x, int data);
	void remove_node(int x);
	int find(int x);
};

#endif
