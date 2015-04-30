#include "../../include/RouteInfoGenerator/RouteCohesion.hpp"
void dfs(long long start, map<long long, set <long long> >& graph, set <long long>& odwiedzone)
{
	odwiedzone.insert(start);
	auto it1 = graph.find(start);
	if(it1!=graph.end())
	{
		for(long long it2 : it1->second)
		{
			if(odwiedzone.find(it2)==odwiedzone.end())
			{
				odwiedzone.insert(it2);
				dfs(it2, graph, odwiedzone);
			}
		}
	}
}

bool graphCohesion (map<long long, set <long long> >& graph)
{
	set <long long> odwiedzone;
	auto it1 = graph.begin();
	if(it1==graph.end())
		return false;

	dfs(it1->first, graph, odwiedzone);

	while(it1!=graph.end())
	{
		if(odwiedzone.find(it1->first)==odwiedzone.end())
			return false;
		it1++;
	}
	return true;
}

bool wayCohesion(set <long long> wayList, osm_base* base)
{
	if(wayList.size()==0)
		return true;
	map <long long, set <long long> > polaczenia;
	for(long long it : wayList)
	{
		vector <long long> nodes = base->ways[it].nodes;
		for(int i=0; i<nodes.size()-1; i++)
		{
			polaczenia[nodes[i]].insert(nodes[i+1]);
			polaczenia[nodes[i+1]].insert(nodes[i]);
		}
	}
	return graphCohesion(polaczenia);
}

bool relationCohesion(long long relationId, osm_base* base)
{
	if(base->relations.find(relationId)==base->relations.end())
		return false;
	vector <relation_member> members=base->relations[relationId].members;
	set <long long> wayList;
	for(auto it1 : members)
	{
		if(it1.member_type==WAY)
		{
			if(it1.role=="")
				wayList.insert(it1.member_id);
		}
	}
	return wayCohesion(wayList, base);
}
