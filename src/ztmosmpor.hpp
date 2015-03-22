#ifndef ZTMOSMPOR
#define ZTMOSMPOR
#include "stringspecial.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <fstream>
#include "osm_base.hpp"
#include "readztm.hpp"
using namespace std;
struct OsmStopData
{
	string name;
	long long stop_position;
	long long bus_stop;
	long long platform;
	char platform_type;
	OsmStopData()
	{
		stop_position = 0;
		bus_stop = 0;
		platform = 0;
		platform_type = 0;
	}
};

map<string, OsmStopData> loadOsmStopData(osm_base* baza)
{
	map <string, OsmStopData> wynik;
	cout<<"ŁADOWANIE..."<<endl;
	map<long long, node>::iterator it1=baza->nodes.begin();
	while(it1!=baza->nodes.end())
	{
		map <string, string> tags=it1->second.getTags();
		if(tags["highway"]=="bus_stop" || tags["railway"]=="tram_stop" || tags["public_transport"]=="stop_position")
		{
			if(tags["ref"]!="")
			{
				string ref = tags["ref"];
				if(tags.find("name")!=tags.end())
					wynik[ref].name=tags["name"];
				if(tags["highway"]=="bus_stop")
					wynik[ref].bus_stop=it1->first;
				if(tags["railway"]=="tram_stop")
					wynik[ref].bus_stop=it1->first;
				if(tags["public_transport"]=="stop_position")
					wynik[ref].stop_position=it1->first;
			}
		}
		it1++;
	}
	cout<<"ŁADOWANIE2..."<<endl;
	map<long long, way>::iterator it2=baza->ways.begin();
	while(it2!=baza->ways.end())
	{
		map <string, string> tags=it2->second.getTags();
		if(tags["public_transport"]=="platform" || tags["highway"]=="platform" || tags["railway"]=="platform")
		{
			if(tags["ref"]!="")
			{
				string ref = tags["ref"];
				wynik[ref].platform=it2->first;
				wynik[ref].platform_type='W';
			}
		}
		it2++;
	}
	map<long long, relation>::iterator it3=baza->relations.begin();
	while(it3!=baza->relations.end())
	{
		map <string, string> tags=it3->second.getTags();
		if(tags["public_transport"]=="platform" || tags["highway"]=="platform" || tags["railway"]=="platform")
		{
			if(tags["ref"]!="")
			{
				string ref = tags["ref"];
				wynik[ref].platform=it3->first;
				wynik[ref].platform_type='R';
			}
		}
		it3++;
	}
	return wynik;
}

class ztmread_for_html : public ScheduleHandler
{
	public:
	map<string, przystanek> przystanki;
	map <string, vector< vector<string> > > dane_linia;
	ztmread_for_html (string sciez)
	{
		ScheduleReaderZtm nowo(sciez, this);
		nowo.run();
	}
	void nowy_przystanek(przystanek nowy)
	{
		przystanki[nowy.id]=nowy;
	}
	void nowa_linia(string nazwa, vector <vector <string> > trasy)
	{
		dane_linia[nazwa]=trasy;
	}
};


bool todigits(int nasze, string nastepne)
{
	stringstream zuo;
	zuo<<nastepne;
	int dati;
	zuo>>dati;
	if(nasze%100==dati)
		return true;
	return false;
}

#endif
