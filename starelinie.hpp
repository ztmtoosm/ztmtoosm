#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include "stringspecial.hpp"
#include "ztmosmpor.hpp"
#include "osm_base.hpp"
#include "dijkstra.hpp"
#include "dij_data.hpp"
#include "htmlgen.hpp"
using namespace std;
using namespace rapidxml;
pair <long long, vector <long long> > relacje_linia(osm_base* roo, long long root, string linia)
{
	pair<long long, vector <long long> > wynik;
	wynik.first=0;
	relation akt=roo->relations[root];
	if(akt.tags["ref"]==linia)
	{
		if(akt.tags["type"]=="route")
		{
			wynik.second.push_back(akt.id);
		}
		if(akt.tags["type"]=="route_master")
		{
			wynik.first=akt.id;
		}
	}
	int s1=akt.members.size();
	for(int i=0; i<s1; i++)
	{
		if(akt.members[i].member_type==RELATION)
		{
			long long teraz_id=akt.members[i].member_id;
			if(roo->relations.find(teraz_id)!=roo->relations.end())
			{
				relation teraz=roo->relations[teraz_id];
				pair<long long, vector <long long> > tmp=relacje_linia(roo, teraz_id, linia);
				if(tmp.first!=0)
				{
					wynik.first=tmp.first;
				}
				for(int j=0; j<tmp.second.size(); j++)
				{
					wynik.second.push_back(tmp.second[j]);
				}
			}
		}
	}
	return wynik;
}

set <string> extract_ref(osm_base* baza, long long rel)
{
	set<string> wynik;
	relation akt=baza->relations[rel];
	int s1=akt.members.size();
	for(int i=0; i<s1; i++)
	{
		if(akt.members[i].member_type==NODE)
		{
			long long teraz_id=akt.members[i].member_id;
			if(baza->nodes.find(teraz_id)!=baza->nodes.end())
			{
				node teraz=baza->nodes[teraz_id];
				if(teraz.tags["highway"]=="bus_stop" || teraz.tags["railway"]=="tram_stop" || teraz.tags["public_transport"]=="stop_position")
				{
					if(teraz.tags.find("ref")!=teraz.tags.end())
					{
						wynik.insert(teraz.tags["ref"]);
					}
				}
			}
		}
		if(akt.members[i].member_type==WAY)
		{
			long long teraz_id=akt.members[i].member_id;
			if(baza->ways.find(teraz_id)!=baza->ways.end())
			{
				way teraz=baza->ways[teraz_id];
				if(teraz.tags["highway"]=="platform" || teraz.tags["railway"]=="platform")
				{
					if(teraz.tags.find("ref")!=teraz.tags.end())
					{
						wynik.insert(teraz.tags["ref"]);
					}
				}
			}

		}
	}
	return wynik;
}

map <long long, string> szukaj_na_drogach(osm_base& baza, long long rel, map<long long, string> nodes_to_be)
{
	relation akt=baza.relations[rel];
	int s1=akt.members.size();
	for(int i=0; i<s1; i++)
	{
		if(akt.members[i].member_type==WAY)
		{
			long long teraz_id=akt.members[i].member_id;
			if(baza.ways.find(teraz_id)!=baza.ways.end())
			{
				way teraz=baza.ways[teraz_id];
				int s2=teraz.nodes.size();
				for(int j=0; j<s2; j++)
				{
					if(nodes_to_be.find(teraz.nodes[j])!=nodes_to_be.end())
					{
						nodes_to_be.erase(nodes_to_be.find(teraz.nodes[j]));
					}
				}
			}
		}
	}
	return nodes_to_be;
}

class PrzegladanieCzyPrawidloweStareLinie
{
	set <string> oldRelationStops(string linia, osm_base* baza)
	{
		cout<<"okgamma"<<endl;
		set <string> wynik0;
		vector <long long> rels=relacje_linia(baza, 3651336, linia).second;
		for(int i=0; i<rels.size(); i++)
		{
			set <string> tmp1=extract_ref(baza, rels[i]);
			wynik0.insert(tmp1.begin(), tmp1.end());
		}
		cout<<"okdela"<<endl;
		return wynik0;
	}

	set <string> newRelationStops(string linia, ztmread_for_html* baza)
	{
		cout<<"okalfa"<<endl;
		set <string> wynik;
		for(int i=0; i<(baza->dane_linia[linia]).size(); i++)
		{
			vector <przystanek_big> przystanki=baza->dane_linia[linia][i];
			for(int j=0; j<przystanki.size(); j++)
			{
				wynik.insert(przystanki[j].id);
			}
		}
		cout<<"okbeta"<<endl;
		return wynik;
	}
	void printRoznice(string linia, osm_base* bazaOsm, ztmread_for_html* bazaZtm)
	{
		set <string> osm_list=oldRelationStops(linia, bazaOsm);
		set <string> ztm_list=newRelationStops(linia, bazaZtm);
		set <string>::iterator it1=osm_list.begin();
		set <string>::iterator it2=ztm_list.begin();
		bool prawi=1;
		string wynik;
		while(it1!=osm_list.end())
		{
			if(ztm_list.find(*it1)==ztm_list.end())
			{
				wynik+=htmlgen::div("osm_problem", "", *it1+" Jest w bazie OSM, nie ma w bazie ZTM");
				prawi=0;
			}
			it1++;
		}
		while(it2!=ztm_list.end())
		{
			if(osm_list.find(*it2)==osm_list.end())
			{
				wynik+=htmlgen::div("ztm_problem", "", *it2+" Jest w bazie ZTM, nie ma w bazie OSM");
				prawi=0;
			}
			it2++;
		}
		if(prawi)
		{
			prawidlowe.insert(linia);
		}
		else
		{
			wynik=htmlgen::div("bledy1", "", "Wykryto następujące błędy")+wynik;
			nieprawidlowe.insert(linia);
		}
		cout<<"LINIA="<<linia<<" "<<wynik<<endl;
		bledy[linia]=htmlgen::div("roznice", "", wynik);
	}
	set <string> doPrzerobienia;
	public:
	set <string> prawidlowe;
	set <string> nieprawidlowe;
	map <string, string> bledy;
	PrzegladanieCzyPrawidloweStareLinie(osm_base* bazaOsm, ztmread_for_html* bazaZtm, set <string> doPrzerobieniaW)
	{
		doPrzerobienia=doPrzerobieniaW;
		set <string>::iterator it1=doPrzerobienia.begin();
		while(it1!=doPrzerobienia.end())
		{
			printRoznice(*it1, bazaOsm, bazaZtm);
			it1++;
		}
	}
};

