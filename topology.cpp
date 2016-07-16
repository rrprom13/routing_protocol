#include <stdlib.h>

#include "topology.h"

void Topology::add_node (int x, int data)
{
	Topology::nr++;
	road* node = (road*) malloc (sizeof(road));
	node->id = x;
	node->cost = data;
	node->next = NULL;
	if (Topology::root == NULL)
		Topology::root = node;
	else
	{
		road *p = Topology::root;
		while (p->next != NULL)
			p = p->next;
		p->next = node;
	}
}

void Topology::remove_node (int x)
{
	road *p = Topology::root;
	if (p == NULL)
		return;
	if (p->id == x)
		Topology::root = Topology::root->next;
	else
	{
		while (p->next != NULL && p->next->id != x)
			p = p->next;
		if (p->next) {
			road *r = p->next->next;
			p->next = r;
		}
	}
	Topology::nr--;
}

int Topology::find (int x)
{
	road *p = Topology::root;
	while (p)
	{
		if (p->id == x)
			return p->cost;
		p = p->next;
	}
	return -1;
}
