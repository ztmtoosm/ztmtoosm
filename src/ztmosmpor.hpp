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
struct data1
{
bool position;
bool bus_stop;
bool platform;
string ref;
string name;
long long id;
double lat;
double lon;
data1()
{
id=0;
position=0;
bus_stop=0;
platform=0;
}
};

struct przystanek_big : public przystanek
{
public:
string name_osm;
long long stop_position;
long long bus_stop;
long long platform;
double to_stop_position;
bool pos_error;
double to_bus_stop;
przystanek_big()
{
pos_error=0;
stop_position=0;
bus_stop=0;
}
przystanek_big(przystanek foo)
{
pos_error=0;
name=foo.name;
id=foo.id;
lon=foo.lon;
lat=foo.lat;
stop_position=0;
bus_stop=0;
platform=0;
miejscowosc=foo.miejscowosc;
}
};



class ztmread_for_html : public ztmread
{
	map<string, data1*> bus_stop_lista;
	map<string, data1*> position_lista;
	map<string, data1*> platform_lista;
	void laduj_listke(osm_base* baza)
	{
		cout<<"ŁADOWANIE..."<<endl;
		map<long long, node>::iterator it1=baza->nodes.begin();
		while(it1!=baza->nodes.end())
		{
			map <string, string> tags=it1->second.getTags();
			if(tags["highway"]=="bus_stop" || tags["railway"]=="tram_stop" || tags["public_transport"]=="stop_position")
			{
				data1* foo = new data1;
				foo->id=it1->second.id;
				foo->lat=it1->second.lat;
				foo->lon=it1->second.lon;
				if(tags.find("name")!=tags.end())
				foo->name=tags["name"];
				if(tags["highway"]=="bus_stop")
				foo->bus_stop=1;
				if(tags["railway"]=="tram_stop")
				foo->bus_stop=1;
				if(tags["public_transport"]=="stop_position")
				foo->position=1;
				if(tags["ref"]!="")
				{
				foo->ref=tags["ref"];
				if(foo->position)
				position_lista[foo->ref]=foo;
				if(foo->bus_stop)
				bus_stop_lista[foo->ref]=foo;
				}
				if(foo->position)
				position_lista[foo->ref]=foo;
				if(foo->bus_stop)
				bus_stop_lista[foo->ref]=foo;
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
				data1* foo = new data1;
				foo->id=it2->second.id;
				if(tags["ref"]!="")
				{
					foo->ref=tags["ref"];
					foo->platform=1;
					platform_lista[foo->ref]=foo;
				}
			}
			it2++;
		}
	}
	set<long long> eee;

	public:
	vector <przystanek_big> dane;
	map <string, vector< vector<przystanek_big> > > dane_linia;
	map <string, przystanek_big> dane0;
	ztmread_for_html (osm_base* sciez0, string sciez, set<long long> eee3) : ztmread(sciez)
	{
	eee=eee3;
	laduj_listke(sciez0);
	run();
	}
	void nowy_przystanek(przystanek nowy)
	{
	przystanek_big nowy_big(nowy);
	if(bus_stop_lista.find(nowy.id)!=bus_stop_lista.end())
	{
	long long idt=bus_stop_lista[nowy.id]->id;
	double tlon=bus_stop_lista[nowy.id]->lon;
	double tlat=bus_stop_lista[nowy.id]->lat;
	double dist=distance(tlon, tlat, nowy.lon, nowy.lat);
	nowy_big.bus_stop=idt;
	nowy_big.to_bus_stop=dist;
	}
	if(position_lista.find(nowy.id)!=position_lista.end())
	{
	long long idt=position_lista[nowy.id]->id;
	if(eee.find(idt)==eee.end())
	{
	nowy_big.pos_error=1;
	}
	double tlon=position_lista[nowy.id]->lon;
	nowy_big.name_osm=position_lista[nowy.id]->name;
	double tlat=position_lista[nowy.id]->lat;
	double dist=distance(tlon, tlat, nowy.lon, nowy.lat);
	nowy_big.stop_position=idt;
	nowy_big.to_stop_position=dist;
	}
	if(platform_lista.find(nowy.id)!=platform_lista.end())
	{
	nowy_big.platform=platform_lista[nowy.id]->id;
	}
	dane0[nowy.id]=nowy_big;
	dane.push_back(nowy_big);
	}
	void nowa_linia(string nazwa, vector <vector <string> > trasy)
	{
	int s1=trasy.size();
	vector <vector <przystanek_big> > dane_tmp_big;
	for(int i=0; i<s1; i++)
	{
	vector <przystanek_big> dane_tmp;
	for(int j=0; j<trasy[i].size(); j++)
	{
	if(dane0.find(trasy[i][j])!=dane0.end())
	dane_tmp.push_back(dane0[trasy[i][j]]);
	}
	dane_tmp_big.push_back(dane_tmp);
	}
	dane_linia[nazwa]=dane_tmp_big;
	}
};

bool digits(string nastepne)
{
	int s1=nastepne.length();
	for(int i=0; i<s1; i++)
	{
		if(nastepne[i]<'0' || nastepne[i]>'9')
		return false;
	}
	return true;
}
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
