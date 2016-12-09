#include "WypisywanieWspolrzednychTras.hpp"
vector <long long> WypisywanieWspolrzednychTras::bfs(long long start, long long stop, map <long long, vector <long long> >& prepareBfs)
{
	vector <long long> wynik;
	vector <long long> kolejka;
	int poz=0;
	kolejka.push_back(start);
	set <long long> odwiedzone;
	odwiedzone.insert(start);
	map <long long, long long> ojc;
	bool ok=1;
	while(poz!=kolejka.size() && ok)
	{
		vector <long long> nachte=prepareBfs[kolejka[poz]];
		for(int i=0; i<nachte.size(); i++)
		{
			if(odwiedzone.find(nachte[i])==odwiedzone.end())
			{
				kolejka.push_back(nachte[i]);
				ojc[nachte[i]]=kolejka[poz];
				odwiedzone.insert(nachte[i]);
				if(nachte[i]==stop)
					ok=0;
			}
		}
		poz++;
	}
	long long akt=stop;
	while(ojc.find(akt)!=ojc.end())
	{
		wynik.push_back(akt);
		akt=ojc[akt];
	}
	wynik.push_back(start);
	reverse(wynik.begin(), wynik.end());
	return wynik;
}

void WypisywanieWspolrzednychTras::easyNodes(long long rel, osm_base* base, WypisywanieWspolrzednychTrasHandler* handler)
{
	relation akt_rel = base->relations[rel];
	map <long long, vector <long long> > prepareBfs;
	map <string, long long> refs;
	for(int i=0; i<akt_rel.members.size(); i++)
	{
		if(akt_rel.members[i].member_type==WAY)
		{
			long long way_id=akt_rel.members[i].member_id;
			if(akt_rel.members[i].role!="platform")
			if(akt_rel.members[i].role!="platform_entry_only")
			if(akt_rel.members[i].role!="platform_exit_only")
			{
				vector <long long> nods=base->ways[way_id].nodes;
				for(int j=0; j<nods.size()-1; j++)
				{
					prepareBfs[nods[j]].push_back(nods[j+1]);
					prepareBfs[nods[j+1]].push_back(nods[j]);
				}
				for(int j=0; j<nods.size(); j++)
				{
					if(base->nodes[nods[j]].getTags()["public_transport"]=="stop_position")
					{
						refs[base->nodes[nods[j]].getTags()["ref"]]=nods[j];
					}
				}
			}
		}
	}
	vector <string> ref1;
	for(int i=0; i<akt_rel.members.size(); i++)
	{
		if(akt_rel.members[i].member_type==NODE)
		{
			bool ok=0;
			long long node_id=akt_rel.members[i].member_id;
			if(akt_rel.members[i].role=="stop")
				ok=1;
			if(akt_rel.members[i].role=="stop_entry_only")
				ok=1;
			if(akt_rel.members[i].role=="stop_exit_only")
				ok=1;
			if(ok && base->nodes[node_id].getTags()["ref"]!="")
			{
				string ref=base->nodes[node_id].getTags()["ref"];
				ref1.push_back(ref);
			}
		}
	}
	for(int i=0; i<ref1.size()-1; i++)
	{
		vector <long long> bfstmp=bfs(refs[ref1[i]], refs[ref1[i+1]], prepareBfs);
		if(dodane.find(pair<string, string> (ref1[i], ref1[i+1]))==dodane.end())
		{
			dodane.insert(pair<string,string>(ref1[i], ref1[i+1]));
			for(int j=0; j<bfstmp.size(); j++)
			{
                WspolrzedneTrasData data;
				data.lat=base->nodes[bfstmp[j]].lat;
				data.lon=base->nodes[bfstmp[j]].lon;
                data.ordinalId = j+1;
                data.id = bfstmp[j];
                data.refStart = ref1[i];
                data.refEnd = ref1[i+1];
                handler->onNewData(data);
			}
		}
	}
}
