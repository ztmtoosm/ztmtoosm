#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include "stringspecial.hpp"
#include "ztmosmpor.hpp"
#include "osm_base.hpp"
#include "dijkstra.hpp"
using namespace std;
using namespace rapidxml;
struct way_part_data
{
	long long way_id;
	long long from;
	long long to;
};

struct dij_data
{
	vector <long long> all_nodes;
	vector <long long> all_ways;
	map<long long, set<long long> > split_data;
	dij_data(vector <long long> stop_nodes, dijkstra& dij)
	{
		vector <way_part_data> dtt;
		int s5=stop_nodes.size();
		for(int i=0; i<s5-1; i++)
		{
			dij.oblicz(stop_nodes[i], stop_nodes[i+1]);
			punkt* from = dij.punkty[stop_nodes[i]];
			punkt* destination = dij.punkty[stop_nodes[i+1]];
			if(destination==NULL)
			{
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
};
string nazwa_skrocona(string nazwa)
{
	if(nazwa=="")
		return nazwa;
	string wynik;
	stringstream plt;
	plt<<nazwa;
	string tmp1;
	vector <string> tmp;
	while(plt>>tmp1)
		tmp.push_back(tmp1);
	for(int i=0; i<tmp.size()-1; i++)
	{
		wynik+=tmp[i];
		if(i<tmp.size()-2)
			wynik+=" ";
	}
	return wynik;
}

int main(int argc, char** argv)
{
	set<long long> wyb_w;
	set<long long> wyb_n;
	set<long long> wyb_r;
	set<string> linki;
	char data[10000];
	cin.getline(data, 10000);
	stringstream linki1;
	linki1<<data;
	string linki2;
	while(linki1>>linki2)
		linki.insert(linki2);
	cin.getline(data, 10000);
	string sciezka=data;
	string sciezka2=sciezka+".gpx";
	fstream plik5(sciezka2.c_str(), ios::out | ios::trunc);
	string arg2=argv[1];
	string arg5=argv[2];
	osm_base bazuka(arg5);
	dijkstra dij;
	dij.laduj_dijkstra_from_base(bazuka);
	ztmread_for_html ztm(arg5, arg2, dij.eee3);
	int s9=ztm.dane.size();
	map <string, vector< vector<przystanek_big> > >::iterator it1=ztm.dane_linia.begin();
	bool przerabiaj=1;
	int linia_licznik=0;
	map<string, vector <vector <long long> > > ids;
	map<string, vector <vector <long long> > > high_stops;
	while(it1!=ztm.dane_linia.end())
	{
		string akt_linia=it1->first;
	//	if(relacje_linia(bazuka, 3651336,akt_linia).first==0)
	//		if(relacje_linia(bazuka, 3651336,akt_linia).second.size()==0)
	//		{
	//			linki.insert(akt_linia);
	//		}
		przerabiaj=1;
		if(linki.find(akt_linia)==linki.end())
			przerabiaj=0;
		if(akt_linia.length()>2 && przerabiaj)
		{
		cout<<"#"<<akt_linia<<endl;
		bool ok=1;
		bool ok2=1;
		int s9=it1->second.size();
		for(int i=0; i<s9; i++)
		{
			int s8=(it1->second)[i].size();
			vector <long long> ids1;
			vector <long long> hig1;
			for(int j=0; j<s8; j++)
			{
				przystanek_big& data=(it1->second)[i][j];
				if(data.stop_position==0)
				{
					cout<<"error: "<<data.name<<"-"<<data.id<<": "<<"BRAK STOP_POSITION"<<endl;
					ok=0;
				}
				else
				{
					if(data.pos_error)
					{
						cout<<"error: "<<data.name<<"-"<<data.id<<": "<<"STOP_POSITION nie leży na drodze"<<endl;
						ok=0;
					}
				}
				ids1.push_back(data.stop_position);
				if(data.bus_stop!=0)
				{
					hig1.push_back(data.bus_stop);
				}
				else
				{
					hig1.push_back(data.stop_position);
				}
			}
			if(ok)
			{
				ids[akt_linia].push_back(ids1);
				high_stops[akt_linia].push_back(hig1);
			}
		}
		cout<<"ok1"<<endl;
		if(ok && ok2)
		{
			for(int g=0; g<ids[akt_linia].size(); g++)
			{
				dij_data d0(ids[akt_linia][g], dij);
				map <long long, set<long long> >::iterator it1=d0.split_data.begin();
				while(it1!=d0.split_data.end())
				{
					vector <pair<long long, vector <long long> > > rozd=bazuka.rozdziel_way(it1->first, it1->second);
					for(int i=0; i<rozd.size(); i++)
					{
						for(int j=0; j<rozd[i].second.size()-1; j++)
						{
							dij.dij_podmien(it1->first, rozd[i].first, rozd[i].second[j], rozd[i].second[j+1]);
							dij.dij_podmien(it1->first, rozd[i].first, rozd[i].second[j+1], rozd[i].second[j]);
						}
					}
					it1++;
				}
			}
		}
		else
		{
			if(ids.find(akt_linia)!=ids.end())
				ids.erase(ids.find(akt_linia));
			if(high_stops.find(akt_linia)!=high_stops.end())
				high_stops.erase(high_stops.find(akt_linia));
	
		}
		}
		it1++;
	}
	cout<<"PART1 DONE"<<endl;
	map<string, vector <vector <long long> > >::iterator it7=ids.begin();
	plik5<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?><gpx>"<<endl;
	while(it7!=ids.end())
	{
		string akt_linia=it7->first;
		string arglin=akt_linia+".gpx";
		plik5.precision(9);
		linia_licznik++;
		vector <long long> relation_ids;
		vector <long long> prop_id=relacje_linia(bazuka, 3651336,akt_linia).second;
		it1=ztm.dane_linia.find(akt_linia);
		cout<<it1->second.size()<<" "<<ids[akt_linia].size()<<endl;
		for(int g=0; g<ids[akt_linia].size(); g++)
		{
			long long aktoo=-1;
			long long aktoo2=-1;
			relation rel;
			rel.modify=1;
			rel.tags["network"]="ZTM Warszawa";
			rel.tags["type"]="route";
			rel.tags["route"]="bus";
			rel.tags["ref"]=akt_linia;
			rel.tags["name"]="Bus "+akt_linia+": "+nazwa_skrocona((it1->second)[g][0].name_osm)+" => "+nazwa_skrocona((it1->second)[g][(it1->second)[g].size()-1].name_osm);
			rel.tags["from"]=nazwa_skrocona((it1->second)[g][0].name_osm);
			rel.tags["to"]=nazwa_skrocona((it1->second)[g][(it1->second)[g].size()-1].name_osm);
			long long kand_id=(g*-1)-linia_licznik*100;
			if(prop_id.size()>0)
			{
				cout<<"WYBIERZ RELACJĘ DLA "<<rel.tags["name"]<<endl;
				for(int i=0; i<prop_id.size(); i++)
				{
					cout<<i+1<<" "<<bazuka.relations[prop_id[i]].tags["name"]<<" "<<prop_id[i]<<endl;
				}
				int kand2;
				cin>>kand2;
				if(kand2>0)
				{
					kand_id=prop_id[kand2-1];
				}
			}
			relation_ids.push_back(kand_id);
			rel.id=relation_ids[relation_ids.size()-1];
			plik5<<"<trk>"<<endl;
			plik5<<"<name>"<<rel.tags["name"]<<"</name>"<<endl;
			plik5<<"<trkseg>"<<endl;
			dij_data d1(ids[akt_linia][g], dij);
			vector <long long> way_id2=d1.all_ways;
			vector <long long> terids=d1.all_nodes;
			for(int i=0; i<terids.size(); i++)
			{
				plik5<<"<trkpt lat=\""<<bazuka.nodes[terids[i]].lat<<"\" lon=\""<<bazuka.nodes[terids[i]].lon<<"\"></trkpt>"<<endl;
			}
			for(int i=0; i<way_id2.size(); i++)
			{
				cout<<way_id2[i]<<" ";
				wyb_w.insert(way_id2[i]);
				relation_member foo;
				foo.member_type=WAY;
				foo.member_id=way_id2[i];
				foo.role="";
				rel.members.push_back(foo);
			}
			cout<<endl;
			for(int i=0; i<high_stops[akt_linia][g].size(); i++)
			{
				string role="stop";
				if(i==0)
					role="stop_entry_only";
				if(i==high_stops[akt_linia][g].size()-1)
					role="stop_exit_only";
				wyb_n.insert(high_stops[akt_linia][g][i]);
				relation_member foo;
				foo.member_type=NODE;
				foo.member_id=high_stops[akt_linia][g][i];
				foo.role=role;
				rel.members.push_back(foo);
			}
			if(rel.id>0)
				rel.version=bazuka.relations[rel.id].version;
			bazuka.relations[rel.id]=rel;
			plik5<<"</trkseg></trk>"<<endl;
		}
		relation rel;
		rel.modify=1;
		rel.id=(-50)-linia_licznik*100;
		long long prop_ids=relacje_linia(bazuka, 3651336,akt_linia).first;
		if(prop_ids!=0)
			rel.id=prop_ids;
		rel.tags["network"]="ZTM Warszawa";
		rel.tags["type"]="route_master";
		rel.tags["route_master"]="bus";
		rel.tags["ref"]=akt_linia;
		rel.tags["name"]="Bus "+akt_linia;
		for(int g=0; g<relation_ids.size(); g++)
		{
				relation_member foo;
				foo.member_type=RELATION;
				foo.member_id=relation_ids[g];
				foo.role="";
				rel.members.push_back(foo);
		}
		if(rel.id>0)
			rel.version=bazuka.relations[rel.id].version;
		bazuka.relations[rel.id]=rel;
		it7++;
	}
	plik5<<"</gpx>"<<endl;
	plik5.close();
	osm_base bazuka3=bazuka.wybrane(wyb_n, wyb_w, wyb_r);
	osm_base bazuka4=bazuka.modified();
	osm_base bazuka2=osm_base(bazuka4, bazuka3);
	bazuka2.wypisz(sciezka);
	cout<<"WYPISANO NASTĘPUJĄCE LINIE/RELACJE: ";
	it7=ids.begin();
	while(it7!=ids.end())
	{
		cout<<it7->first<<endl;
		it7++;
	}
}
