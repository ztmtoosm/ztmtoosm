#ifndef DIJDATA_HPP
#define DIJDATA_HPP
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include "dijkstra.hpp"
using namespace std;
/*
struct way_part_data
{
	long long way_id;
	long long from;
	long long to;
};*/
struct dij_data
{
	bool ok;
	vector <long long> all_nodes;
	vector <long long> all_ways;
	map<long long, set<long long> > split_data;
	dij_data(vector <long long> stop_nodes, dijkstra* dij, int przelicznikId);
};
#endif
