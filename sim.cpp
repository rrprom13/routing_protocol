/******************************************************************************/
/* Tema 2 Protocoale de Comunicatie (Aprilie 2015)                            */
/******************************************************************************/

#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "api.h"
#include "sim.h"
#include "router.h"
#include "event.h"
#include "message.h"

Event *change;
Router *nodes;
Message *msg;
int nr;
int nr_events;
int total_time;
int nr_msg;
int version;

int mindist(bool *visited, double *dist)
{
	int position;
	double min = INT_MAX;
	for (int i = 0 ; i < nr ; i++)
		if (visited[i] == false && dist[i] <= min) {
			min = dist[i];
			position = i;
		}
	return position;
}

double distance(int x, int y, int src, int *ok)
{
	double val = 0;
	if (nodes[src].near[x].nr == 0)
		return val;
	road *p = nodes[src].near[x].root;
	while (p)
	{
		if (p->id == y)
		{
			val = p->cost;
			*ok = 1;
			break;
		}
		p = p->next;
	}
	return val;
}

void way(int current, int dest, int **path, int *prev)
{
	if (current == dest)
		return;
	for (int i = 1 ; i <= path[current][0] ; i++)
	{
		*prev = current;
		way(path[current][i],dest,path,prev);
	}
}

void dijkstra(int src)
{
	bool visited[nr];
	double dist[nr];
	int** path = (int**) malloc (nr * sizeof(int*));
	for (int i = 0 ; i < nr ; i++)
	{
		visited[i] = false;
		dist[i] = INT_MAX;
		path[i] = (int*) malloc (nr * sizeof(int));
		path[i][0] = 0;
	}
	dist[src] = 0;

	int min, ok;
	double x;
	for (int i = 0 ; i < nr ; i++)
	{
		min = mindist(visited,dist);
		visited[min] = true;
		for (int j = 0 ; j < nr ; j++)
			if (visited[j] == false)
			{
				ok = 0;
				x = distance(min,j,src,&ok);
				if (ok == 1 && dist[min] != INT_MAX)
				{
					if (dist[min] + x < dist[j])
					{
						dist[j] = dist[min] + x;
						path[j][0] = 1;
                    				path[j][1] = min;
					}
					else
						if (dist[min] + x == dist[j] && min < j)
						{
							path[j][0]++;
							path[j][path[j][0]] = min;
						}
				}
			}
	}
	for (int i = 0 ; i < nr ; i++)
	{
		if (i != src && dist[i] < INT_MAX)
		{
			int prev = src;
			way(i,src,path,&prev);
			nodes[src].route_table[i][0] = dist[i];
			nodes[src].route_table[i][1] = prev;
		}
	}
}

int get_digits (int x)
{
	if (x == 0)
		return 1;
	int res = 0;
	while (x)
	{
		res++;
		x /= 10;
	}
	return res;
}

char* get_string (int x, int id)
{
	if (nodes[x].near[id].nr == 0)
		return NULL;
	char *res = (char*) malloc (100 * sizeof(int));
	road *p = nodes[x].near[id].root;
	sprintf(res,"%d %d",p->id,p->cost);
	p = p->next;
	while (p)
	{
		sprintf(res,"%s %d %d",res,p->id,p->cost);
		p = p->next;
	}
	return res;
}

void read_topology (char *file)
{
	int n, n1, n2, cost;
	ifstream read;
	read.open(file);
 	read >> n;
	nr = n;
	nodes = new Router[n];

	for (int i = 0 ; i < n ; i++)
	{
		nodes[i].route_table = (int**) malloc (n * sizeof(int*));
		nodes[i].near = new Topology[n];
		nodes[i].version = (int*) malloc (nr * sizeof(int*));
		for (int j = 0 ; j < n ; j++)
		{
			nodes[i].route_table[j] = (int*) malloc (2 * sizeof(int));
			nodes[i].version[j] = -1;
			if (i == j)
			{
				nodes[i].route_table[j][0] = 0;
				nodes[i].route_table[j][1] = i;
			}
			else
			{
				nodes[i].route_table[j][0] = -1;
				nodes[i].route_table[j][1] = -1;
			}
		}
	}
	while (read >> n1 && read >> cost && read >> n2)
	{
		if (nodes[n1].near[n1].find(n2) != -1)
		{
			nodes[n1].near[n1].remove_node(n2);
			nodes[n2].near[n2].remove_node(n1);

			nodes[n1].near[n1].nr--;
			nodes[n2].near[n2].nr--;
		}

		nodes[n1].near[n1].add_node(n2,cost);
		nodes[n2].near[n2].add_node(n1,cost);

		nodes[n1].near[n1].nr++;
		nodes[n2].near[n2].nr++;

		nodes[n1].route_table[n2][0] = cost;
		nodes[n1].route_table[n2][1] = n2;

		nodes[n2].route_table[n1][0] = cost;
		nodes[n2].route_table[n1][1] = n1;
	}

	read.close();
}

int get_int (char *str)
{
	int pos = 0;
	while (str[pos] >= '0' && str[pos] <= '9')
		pos++;
	char res[pos + 1];
	strncpy(res,str,pos);
	res[pos] = '\0';
	return atoi(res);
}

void read_messages(char *file)
{
	int index = 0;
	char data[100];
	ifstream read(file);

	read.getline(data,100);
	nr_msg = atoi(data);
	msg = new Message[nr_msg];

	while (index < nr_msg)
	{
		int pos = 0;
		read.getline(data,100);

		msg[index].src = get_int(data);
		pos += get_digits(msg[index].src) + 1;

		msg[index].dest = get_int(data + pos);
		pos += get_digits(msg[index].dest) + 1;

		msg[index].time = get_int(data + pos);
		pos += get_digits(msg[index].time) + 1;

		msg[index].tag = get_int(data + pos);
		pos += get_digits(msg[index].tag);

		msg[index].data = (char*) malloc (100 * sizeof(char));

		sprintf(msg[index].data,"%s\n",data + pos);
		msg[index].data[strlen(msg[index].data)] = '\0';

		index++;
	}

	read.close();
}

void read_events(char *file)
{
	int index = 0, r1, r2, type, cost, time;
	ifstream read;
	read.open(file);
 	read >> total_time;
	read >> nr_events;
	change = new Event[nr_events];

	while (read >> r1 && read >> r2 && read >> type && read >> cost && read >> time)
	{
		change[index].r1 = r1;
		change[index].r2 = r2;
		change[index].type = type;
		change[index].cost = cost;
		change[index].time = time;
		index++;
	}

	read.close();
}

void init_sim(int argc, char **argv)
{
	read_topology(argv[1]);
	read_messages(argv[2]);
	read_events(argv[3]);
	version = 0;

	for (int i = 0 ; i < nr ; i++)
	{
		road *r = nodes[i].near[i].root;
		char send[100], tmp[100];
		sprintf(tmp,"%d",i);
		while (r)
		{
			sprintf(tmp,"%s %d %d",tmp,r->id,r->cost);
			r = r->next;
		}
		int len = strlen(tmp) + 2;
		sprintf(send,"%d %d %s",len,version,tmp);

		road *p = nodes[i].near[i].root;

		while (p)
		{
			endpoint[i].send_msg(&endpoint[p->id], send, strlen(send), NULL);
			p = p->next;
		}
	}
	version++;
}

void clean_sim() {}

void trigger_events()
{
	for (int i = 0 ; i < nr_events ; i++)
		if (change[i].time == get_time())
		{
			int r1 = change[i].r1;
			int r2 = change[i].r2;
			int cost = change[i].cost;

			if (change[i].type == 0)
			{
				if (nodes[r1].near[r1].find(r2) != -1)
				{
					nodes[r1].near[r1].remove_node(r2);
					nodes[r2].near[r2].remove_node(r1);

					nodes[r1].near[r1].nr--;
					nodes[r2].near[r2].nr--;
				}

				nodes[r1].near[r1].add_node(r2,cost);
				nodes[r2].near[r2].add_node(r1,cost);

				nodes[r1].near[r1].nr++;
				nodes[r2].near[r2].nr++;

				nodes[r1].route_table[r2][0] = cost;
				nodes[r1].route_table[r2][1] = r2;

				nodes[r2].route_table[r1][0] = cost;
				nodes[r2].route_table[r1][1] = r1;

				road *r = nodes[r1].near[r1].root;
				char send1[100], tmp1[100];
				sprintf(tmp1,"%d",r1);
				while (r)
				{
					sprintf(tmp1,"%s %d %d",tmp1,r->id,r->cost);
					r = r->next;
				}
				int len = strlen(tmp1) + get_digits(i) + 1;
				sprintf(send1,"%d %d %s",len,version,tmp1);

				r = nodes[r2].near[r2].root;
				char send2[100], tmp2[100];
				sprintf(tmp2,"%d",r2);
				while (r)
				{
					sprintf(tmp2,"%s %d %d",tmp2,r->id,r->cost);
					r = r->next;
				}
				len = strlen(tmp2) + get_digits(i) + 1;
				sprintf(send2,"%d %d %s",len,version,tmp2);

				road *p = nodes[r1].near[r1].root;
				while (p)
				{
					endpoint[r1].send_msg(&endpoint[p->id], send1, strlen(send1), NULL);
					p = p->next;
				}

				p = nodes[r2].near[r2].root;
				while (p)
				{
					endpoint[r2].send_msg(&endpoint[p->id], send2, strlen(send2), NULL);
					p = p->next;
				}
			}
			else
			{
				if (nodes[r1].near[r1].find(r2) != -1)
				{
					nodes[r1].near[r1].remove_node(r2);
					nodes[r2].near[r2].remove_node(r1);

					nodes[r1].near[r1].nr--;
					nodes[r2].near[r2].nr--;
				}

				nodes[r1].route_table[r2][0] = -1;
				nodes[r1].route_table[r2][1] = -1;

				nodes[r2].route_table[r1][0] = -1;
				nodes[r2].route_table[r1][1] = -1;

				road *r = nodes[r1].near[r1].root;
				char send1[100], tmp1[100];
				sprintf(tmp1,"%d",r1);
				while (r)
				{
					sprintf(tmp1,"%s %d %d",tmp1,r->id,r->cost);
					r = r->next;
				}
				int len = strlen(tmp1) + get_digits(i) + 1;
				sprintf(send1,"%d %d %s",len,version,tmp1);

				r = nodes[r2].near[r2].root;
				char send2[100], tmp2[100];
				sprintf(tmp2,"%d",r2);
				while (r)
				{
					sprintf(tmp2,"%s %d %d",tmp2,r->id,r->cost);
					r = r->next;
				}
				len = strlen(tmp2) + get_digits(i) + 1;
				sprintf(send2,"%d %d %s",len,version,tmp2);

				road *p = nodes[r1].near[r1].root;
				while (p)
				{
					endpoint[r1].send_msg(&endpoint[p->id], send1, strlen(send1), NULL);
					p = p->next;
				}

				p = nodes[r2].near[r2].root;
				while (p)
				{
					endpoint[r2].send_msg(&endpoint[p->id], send2, strlen(send2), NULL);
					p = p->next;
				}
			}
			version++;
		}
}

void process_messages()
{
	for (int i = 0 ; i < nr_msg ; i++)
		if (msg[i].time == get_time())
			endpoint[msg[i].src].route_message(&endpoint[nodes[msg[i].src].route_table[msg[i].dest][1]], msg[i].dest, msg[i].tag, msg[i].data, NULL);

	for (int i = 0 ; i < nr ; i++)
		while (1)
		{
			int src, dest, tag;
			char data[100];
			int valid = endpoint[i].recv_message(&src, &dest, &tag, data);
			if (valid == 0)
				break;
			if (i != dest)
				endpoint[i].route_message(&endpoint[nodes[i].route_table[dest][1]], dest, tag, data, NULL);
		}

	for (int i = 0 ; i < nr ; i++)
	{
		char receive[100];
		while (1)
		{
			int rez = endpoint[i].recv_protocol_message(receive);
			
			if (rez < 0)
				break;

			receive[rez] = '\0';
			int walk;
			int len = rez;
			for (walk = 0 ; walk < len ; walk++)
				if (receive[walk] < '0' || receive[walk] > '9')
					break;

			char *p = (char*) malloc (walk + 1);
			memset(p,0,walk+1);
			strncpy(p,receive,walk);
			p[strlen(p)] = '\0';
			int dim = atoi(p);

			int go = walk + 1;
			while (go < len && receive[go] >= '0' && receive[go] <= '9')
				go++;
			char *r = (char*) malloc (go - walk + 2);
			memset(r,0,go - walk + 2);
			strncpy(r,receive + walk + 1,go - walk - 1);
			r[strlen(r)] = '\0';
			int ev = atoi(r);
	
			int pass = go + 1;
			while (pass < len && receive[pass] >= '0' && receive[pass] <= '9')
				pass++;
			char *q = (char*) malloc (pass - go + 2);
			memset(q,0,pass - go + 2);
			strncpy(q,receive + go + 1,pass - go - 1);
			q[strlen(q)] = '\0';
			int id = atoi(q);

			char *aux = (char*) malloc (100 * sizeof(char));
			strncpy(aux,receive + pass + 1, dim - pass + walk);
			aux[dim - pass + walk] = '\0';
			dim = dim - pass + walk;

			if (ev > nodes[i].version[id])
			{
				nodes[i].near[id].nr = 0;
				nodes[i].near[id].root = NULL;
				nodes[i].version[id] = ev;
				int count = 0, node, cost;
				while (count < dim)
				{
					sscanf(aux + count,"%d %d",&node,&cost);
					nodes[i].near[id].add_node(node,cost);
					nodes[i].near[id].nr++;
					count += get_digits(node) + get_digits(cost) + 2;
				}
			
				road *p = nodes[i].near[i].root;
				while (p)
				{
					endpoint[i].send_msg(&endpoint[p->id], receive, strlen(receive), NULL);
					p = p->next;
				}

			}
		}
	}
}

void update_routing_table()
{
	for (int i = 0 ; i < nr ; i++)
		dijkstra(i);
}
