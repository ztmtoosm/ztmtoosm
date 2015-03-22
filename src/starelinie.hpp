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
pair <long long, vector <long long> > relacje_linia(osm_base* roo, long long root, string linia)
{
	pair<long long, vector <long long> > wynik;
	wynik.first=0;
	relation akt=roo->relations[root];
	map <string, string> tags = akt.getTags();
	if(tags["ref"]==linia)
	{
		if(tags["type"]=="route")
		{
			wynik.second.push_back(akt.id);
		}
		if(tags["type"]=="route_master")
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

set <long long> wszystkie_route (osm_base* roo, long long root)
{
	set <long long> wynik;
	relation akt=roo->relations[root];
	auto tags = akt.getTags();
	int s1=akt.members.size();
	if(tags["type"]=="route")
	{
		wynik.insert(akt.id);
	}
	for(int i=0; i<s1; i++)
	{
		if(akt.members[i].member_type==RELATION)
		{
			long long teraz_id=akt.members[i].member_id;
			if(roo->relations.find(teraz_id)!=roo->relations.end())
			{
				relation teraz=roo->relations[teraz_id];
				set <long long> tmp=wszystkie_route(roo, teraz_id);
				wynik.insert(tmp.begin(), tmp.end());
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
				auto tags=teraz.getTags();
				if(tags["highway"]=="bus_stop" || tags["railway"]=="tram_stop" || tags["public_transport"]=="stop_position")
				{
					if(tags.find("ref")!=tags.end())
					{
						wynik.insert(tags["ref"]);
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
				auto tags = teraz.getTags();
				if(tags["highway"]=="platform" || tags["railway"]=="platform")
				{
					if(tags.find("ref")!=tags.end())
					{
						wynik.insert(tags["ref"]);
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
		set <string> wynik0;
		vector <long long> rels=relacje_linia(baza, 3651336, linia).second;
		for(int i=0; i<rels.size(); i++)
		{
			set <string> tmp1=extract_ref(baza, rels[i]);
			wynik0.insert(tmp1.begin(), tmp1.end());
		}
		return wynik0;
	}

	set <string> newRelationStops(string linia, ztmread_for_html* baza)
	{
		set <string> wynik;
		for(int i=0; i<(baza->dane_linia[linia]).size(); i++)
		{
			vector <string> przystanki=baza->dane_linia[linia][i];
			for(int j=0; j<przystanki.size(); j++)
			{
				wynik.insert(przystanki[j]);
			}
		}
		return wynik;
	}
	string infoLinie(string linia, osm_base* bazaOsm, ztmread_for_html* bazaZtm)
	{
		vector <long long> rels=relacje_linia(bazaOsm, 3651336, linia).second;
		long long rel_head=relacje_linia(bazaOsm, 3651336, linia).first;
		string link_href="http://openstreetmap.org/relation/"+tostring(rel_head);
		string tmp1;
		tmp1+=htmlgen::div("linia_head", "poczatek"+linia, htmlgen::div("humer_linii", "", linia)+htmlgen::link("http://www.ztm.waw.pl/rozklad_nowy.php?c=182&l=1&q="+linia, "pokaż rozkład"));
		tmp1+=htmlgen::div("relglowna", "", "route_master: "+htmlgen::link(link_href, tostring(rel_head)));
		for(int i=0; i<rels.size(); i++)
		{
			link_href="http://openstreetmap.org/relation/"+tostring(rels[i]);
			string nazwa=bazaOsm->relations[rels[i]].getTags()["name"];
			tmp1+=htmlgen::div("relboczna", "", "route: "+htmlgen::link(link_href, tostring(rels[i])+" "+nazwa));
		}
		return htmlgen::div("infolinie", "", tmp1);
	}
	void printRoznice(string linia, osm_base* bazaOsm, ztmread_for_html* bazaZtm, map<string, string>* infoHTML)
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
		(*infoHTML)[linia]=infoLinie(linia, bazaOsm, bazaZtm);
		if(prawi)
		{
			prawidlowe.insert(linia);
		}
		else
		{
		
			(*infoHTML)[linia]+=htmlgen::div("bledy1", "bledy1_"+linia, "Pokaż różnice pomiędzy bazami >>>");
			(*infoHTML)[linia]+=htmlgen::div("roznice", "roznice_"+linia, wynik);
			nieprawidlowe.insert(linia);
		}
	}
	set <string> doPrzerobienia;
	public:
	set <string> prawidlowe;
	set <string> nieprawidlowe;
	PrzegladanieCzyPrawidloweStareLinie(osm_base* bazaOsm, ztmread_for_html* bazaZtm, set <string> doPrzerobieniaW, map<string, string>* infoHTML)
	{
		doPrzerobienia=doPrzerobieniaW;
		set <string>::iterator it1=doPrzerobienia.begin();
		while(it1!=doPrzerobienia.end())
		{
			printRoznice(*it1, bazaOsm, bazaZtm, infoHTML);
			it1++;
		}
	}
};

