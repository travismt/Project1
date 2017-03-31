#include "stdafx.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <map>
using namespace std;

struct nodeTableEntry
{
	int cost;
	int nextHop;
};

struct nodeNeighborEntry
{
	int neighbor;
	float delay;
};

struct event
{
	int src;
	int dest;
	int hopCount;
	int packetType;
	float time;
	map<int, nodeTableEntry> routeTable;
	event* next = NULL;
};

event* qHead;
float clock = 0;

map<int, map<int, nodeTableEntry> > nodeTables;
map<int, map<int, nodeNeighborEntry> > neighborTables;
//map<int, map<int, nodeTableEntry> >::iterator nodeTable_it;
//map<int, map<int, nodeNeighborEntry> >::iterator neighborTable_it;

//****** Function Prototypes
void sendUpdates(int);
void addEventToQue(event*);
void addTableEntries(int, int, int, float);
void PrintRoutingTables();
void PrintNeighborTables();
//****** Function Prototypes

int main(int argc, char* argv[])
{
	string myline;
	ifstream myReadFile;
	string t1 = argv[1];

	myReadFile.open(argv[1]);
	int src, dest, cost;
	float delay;

	// processing the topology file
	while (!myReadFile.eof())
	{
		getline(myReadFile, myline);
		sscanf_s(myline.c_str(), "%d %d %d %f", &src, &dest, &cost, &delay);
		addTableEntries(src, dest, cost, delay);
		addTableEntries(dest, src, cost, delay);
	}

	//nodes = nodeTables[node];
	//for (map<int, nodeTableEntry>::iterator temp_it = neighbors.begin(); temp_it != neighbors.end(); temp_it++)
	//{

	for (map<int, map<int, nodeTableEntry> >::iterator node_it = nodeTables.begin(); node_it != nodeTables.end(); node_it++) 
	{
		sendUpdates(node_it->first);
	}
	
	PrintRoutingTables();
	PrintNeighborTables();

	return 0;
}

void sendUpdates(int node)
{
	map<int, nodeNeighborEntry> neighbors;
	neighbors = neighborTables[node];
	int neighbor;
	float delay;
				
	for (map<int, nodeNeighborEntry>::iterator temp_it = neighbors.begin(); temp_it != neighbors.end(); temp_it++)
	{
		event* tempEvent = new event();
		nodeNeighborEntry temp_entry = temp_it->second;
		neighbor = temp_entry.neighbor;
		delay = temp_entry.delay;
		tempEvent->src = node;
		tempEvent->dest = neighbor;
		tempEvent->packetType = 0;
		tempEvent->routeTable = nodeTables[node];
		tempEvent->time = clock + delay;
		addEventToQue(tempEvent);
	}
}

void addEventToQue(event* e)
{
	//adding to empty queue
	if (qHead == NULL)
	{
		qHead = e;
		return;
	}
	//new event becomes head
	if (qHead->time > e->time)
	{
		e->next = qHead;
		qHead = e;
		return;
	}
	//adding to middle of queue
	else
	{
		event* temp = qHead;
		event* prev = NULL;

		do
		{
			if (temp->time > e->time)
			{
				prev->next = e;
				e->next = temp;
				return;
			}
			prev = temp;
			temp = temp->next;
		} while(temp != NULL);
		//adding to the end
		prev->next = e;
	}
}

void addTableEntries(int targetNode, int entryNode, int cost, float delay) {

	map<int, map<int, nodeTableEntry> >::iterator nodeTable_it;
	map<int, map<int, nodeNeighborEntry> >::iterator neighborTable_it;

	nodeTable_it = nodeTables.find(targetNode);
	neighborTable_it = neighborTables.find(targetNode);

	nodeTableEntry routeEntry;
	routeEntry.cost = cost;
	routeEntry.nextHop = entryNode;

	nodeNeighborEntry neighborEntry;
	neighborEntry.neighbor = entryNode;
	neighborEntry.delay = delay;
	
	if (nodeTable_it == nodeTables.end())
	{
		map <int, nodeTableEntry> temp_map;
		temp_map.insert(make_pair(entryNode, routeEntry));
		nodeTables.insert(make_pair(targetNode, temp_map));
	}
	else
	{
		nodeTables[targetNode].insert(make_pair(entryNode, routeEntry));
	}

	if (neighborTable_it == neighborTables.end())
	{
		map <int, nodeNeighborEntry> temp_map;
		temp_map.insert(make_pair(entryNode, neighborEntry));
		neighborTables.insert(make_pair(targetNode, temp_map));
	}
	else
	{
		neighborTables[targetNode].insert(make_pair(entryNode, neighborEntry));
	}

}

void PrintRoutingTables() {
	for (map<int, map<int, nodeTableEntry> >::iterator node_it = nodeTables.begin(); node_it != nodeTables.end(); node_it++) {
		map<int, nodeTableEntry> temp = node_it->second;
		cout << "Node: " << node_it->first << "\n";
		for (map<int, nodeTableEntry>::iterator temp_it = temp.begin(); temp_it != temp.end(); temp_it++) {
			nodeTableEntry temp_entry = temp_it->second;
			cout << temp_it->first << " " << temp_entry.cost << " " << temp_entry.nextHop << "\n";
		}
		cout << "\n";
	}
}

void PrintNeighborTables() {
	for (map<int, map<int, nodeNeighborEntry> >::iterator node_it = neighborTables.begin(); node_it != neighborTables.end(); node_it++) {
		map<int, nodeNeighborEntry> temp = node_it->second;
		cout << "Node: " << node_it->first << "\n";
		for (map<int, nodeNeighborEntry>::iterator temp_it = temp.begin(); temp_it != temp.end(); temp_it++) {
			nodeNeighborEntry temp_entry = temp_it->second;
			cout << temp_entry.neighbor << " " << temp_entry.delay << "\n";
		}
		cout << "\n";
	}
}


void triggeredUpdate()
{

}


