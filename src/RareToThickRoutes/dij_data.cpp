#include "../../include/RareToThickRoutes/dij_data.hpp"
dij_data::dij_data(vector <long long> stop_nodes, dijkstra* dij, int przelicznikId)
{
	ok=1;
	vector <way_part_data> dtt;
	int s5=stop_nodes.size();
	for(int i=0; i<s5-1; i++)
	{
		if(ok)
		{
		dij->oblicz(stop_nodes[i], stop_nodes[i+1], przelicznikId);
		cout<<"####"<<endl;
		punkt* from = dij->punkty[stop_nodes[i]];
		punkt* destination = dij->punkty[stop_nodes[i+1]];
		if(from==NULL || destination==NULL)
		{
			ok=0;
		}
		else
		{
			vector <long long> partial_nodes;
			vector <long long> partial_ways;
			vector <way_part_data> dtt2;
			long long destination_id=destination->osm_id;
			partial_nodes.push_back(destination_id);
			while(destination!=from && destination!=NULL && destination->from!=NULL)
			{
				way_part_data foo;
				foo.to=destination->osm_id;
				foo.way_id=destination->from->osm_id;
				partial_ways.push_back(destination->from->osm_id);
				destination=destination->from->start;
				foo.from=destination->osm_id;
				destination_id=destination->osm_id;
				partial_nodes.push_back(destination_id);
				dtt2.push_back(foo);
			}
			for(int j=partial_nodes.size()-1; j>=0; j--)
			{
				if(all_nodes.size()==0 || all_nodes[all_nodes.size()-1]!=partial_nodes[j])
				{
					all_nodes.push_back(partial_nodes[j]);
				}
			}
			for(int j=partial_ways.size()-1; j>=0; j--)
			{
				if(all_ways.size()==0 || all_ways[all_ways.size()-1]!=partial_ways[j])
				{
					all_ways.push_back(partial_ways[j]);
				}
			}
			for(int j=dtt2.size()-1; j>=0; j--)
			{
				dtt.push_back(dtt2[j]);
			}
		}
		}
	}
	if(ok)
	{
		long long akt_way=-1;
		long long akt_from=-1;
		long long akt_to=-1;
		for(int i=0; i<dtt.size(); i++)
		{
			if(akt_way!=-1 && dtt[i].way_id!=akt_way)
			{
				split_data[akt_way].insert(akt_from);
				split_data[akt_way].insert(akt_to);
			}
			if(dtt[i].way_id!=akt_way)
			{
				akt_way=dtt[i].way_id;
				akt_from=dtt[i].from;
			}
			akt_to=dtt[i].to;
		}
		if(akt_way!=-1)
		{
			split_data[akt_way].insert(akt_from);
			split_data[akt_way].insert(akt_to);
		}
	}
}
