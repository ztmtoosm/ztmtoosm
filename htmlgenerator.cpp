#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <fstream>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "readztm.hpp"
using namespace std;
using namespace rapidxml;

double akt_lat;
double akt_lon;

struct data1
{
	bool position;
	bool bus_stop;
	string ref;
	long long id;
	double lat;
	double lon;
	data1()
	{
		position=0;
		bus_stop=0;
	}
};

vector <data1*> listka;

double distance(double long1, double lat1, double long2, double lat2)
{
	double d2r = (M_PI / 180.0);
	double dlong = (long2 - long1) * d2r;
	double dlat = (lat2 - lat1) * d2r;
	double a = pow(sin(dlat/2.0), 2) + cos(lat1*d2r) * cos(lat2*d2r) * pow(sin(dlong/2.0), 2);
	double c = 2 * atan2(sqrt(a), sqrt(1-a));
	double d = 6367 * c;
	return d;
}
bool srt(data1* alfa, data1* beta)
{
	if(distance(alfa->lon, alfa->lat, akt_lon, akt_lat)<distance(beta->lon, beta->lat, akt_lon, akt_lat))
		return true;
	return false;
}

template <typename T1> T1 fromstring(string alfa)
{
	stringstream foo;
	foo<<alfa;
	T1 foo2;
	foo>>foo2;
	return foo2;
}

map<string, data1*> bus_stop_lista;
map<string, data1*> position_lista;
void laduj_listke(string plik)
{
	file <> xmlFile(plik.c_str());
	xml_document<> doc;
	doc.parse<0>(xmlFile.data());
	xml_node<>* root=doc.first_node("osm");
	for(xml_node <> *tag=root->first_node("node"); tag; tag=tag->next_sibling("node"))
	{
		map <string, string> tags;
		for(xml_node <> *tap=tag->first_node("tag"); tap; tap=tap->next_sibling("tag"))
		{
			xml_attribute<>*k=tap->first_attribute("k");
			xml_attribute<>*v=tap->first_attribute("v");
			tags[k->value()]=v->value();
		}
		if(tags["highway"]=="bus_stop" || tags["railway"]=="tram_stop" || tags["public_transport"]=="stop_position")
		{
			xml_attribute<>*id=tag->first_attribute("id");
			xml_attribute<>*lat=tag->first_attribute("lat");
			xml_attribute<>*lon=tag->first_attribute("lon");
			data1* foo = new data1;
			foo->id=fromstring<long long>(id->value());
			foo->lat=fromstring<double>(lat->value());
			foo->lon=fromstring<double>(lon->value());
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
			listka.push_back(foo);
			if(foo->position)
				position_lista[foo->ref]=foo;
			if(foo->bus_stop)
				bus_stop_lista[foo->ref]=foo;
		}
	}
}
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
int main(int argc, char** argv)
{
	if(argc<3)
	{
		cout<<"za mało argumentów..."<<endl;
		return 0;
	}
	cout<<"Ładowanie OSM..."<<endl;
	laduj_listke(argv[1]);
	fstream html(argv[3], ios::out | ios::trunc);
	html<<"<html><table>"<<endl;
	cout<<"Ładowanie ZTM..."<<endl;
	fstream wyj(argv[4], ios::out | ios::trunc);
	if(argc>3)
	{
		fstream wyj(argv[4], ios::out | ios::trunc);
		wyj<<"longitude;latitude;highway;name;ref"<<endl;
	}
	char data[20000];
	string arg2=argv[2];
	ztmread ztm(arg2);
	int s9=ztm.dane.size();
	cout<<"Obróbka danych..."<<endl;
	html.precision(9);
	for(int j=0; j<s9; j++)
	{
		string name=ztm.dane[j].name;
		string id=ztm.dane[j].id;
		akt_lon=ztm.dane[j].lon;
		akt_lat=ztm.dane[j].lat;
		if(id[4]<'5')
		{
			html<<"<tr>";
			html<<"<td>"<<akt_lon<<"</td>";
			html<<"<td>"<<akt_lat<<"</td>";
			html<<"<td> <a href=\"http://www.openstreetmap.org/#map=19/"<<akt_lat<<"/"<<akt_lon<<"\"/>";
			html<<name<<" "<<id[4]<<id[5]<<"</a></td>";
			html<<"<td>"<<id<<"</td>";
			bool ok=0;
			html<<"<td>";
			if(bus_stop_lista.find(id)!=bus_stop_lista.end())
			{
				long long idt=bus_stop_lista[id]->id;
				double tlon=bus_stop_lista[id]->lon;
				double tlat=bus_stop_lista[id]->lat;
				double dist=distance(tlon, tlat, akt_lon, akt_lat);
				html<<"otagowany tym refem bus/tram_stop: "<<dist*1000<<" m; "<<"<a href=\"http://openstreetmap.org/node/"<<idt<<"\" >"<<idt<<"</a>";
				ok=1;
			}
			html<<"</td>";
			html<<"<td>";
			if(position_lista.find(id)!=position_lista.end())
			{
				long long idt=position_lista[id]->id;
				double tlon=position_lista[id]->lon;
				double tlat=position_lista[id]->lat;
				double dist=distance(tlon, tlat, akt_lon, akt_lat);
				html<<"otagowany tym refem stop_position: "<<dist*1000<<" m; "<<"<a href=\"http://openstreetmap.org/node/"<<idt<<"\" >"<<idt<<"</a>";
				ok=1;
			}
			html<<"</td>";
			html<<"<td>";


			if(!ok)
			{
				wyj<<akt_lon<<";"<<akt_lat<<";bus_stop;"<<name<<" "<<id[4]<<id[5]<<";"<<id<<endl;
				sort(listka.begin(), listka.end(), srt);
				for(int i=0; i<1; i++)
				{
					double dist=distance(listka[i]->lon, listka[i]->lat, akt_lon, akt_lat);
					html<<"BRAK OTAGOWANIA, najbl. przystanek w bazie OSM: "<<dist*1000<<" m "<<"<a href=\"http://www.openstreetmap.org/node/"<<listka[i]->id<<"\">"<<listka[i]->id<<"</a>";
				}
			}
			html<<"</td>";
			html<<"</tr>"<<endl;
		}
	}
	html<<"</table></html>"<<endl;
}
