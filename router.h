#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "topology.h"

class Router {
public:
	Topology *near;
	int **route_table;
	int *version;
};

#endif
