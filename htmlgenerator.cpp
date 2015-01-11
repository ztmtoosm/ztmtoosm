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
#include "stringspecial.hpp"
#include "ztmosmpor.hpp"
#include "dijkstra.hpp"
#include "osm_base.hpp"
using namespace std;
using namespace rapidxml;

class csvfile
{
	fstream plik;
	string sciezka;
	public:
	csvfile(string sciezka)
	{
		plik.open(sciezka.c_str(), ios::out | ios::trunc);
		plik<<"longitude;latitude;highway;name;ref"<<endl;
	}
	void send(double lon, double lat, string name, string ref)
	{
		plik<<lon<<";"<<lat<<";bus_stop;"<<name<<";"<<ref<<endl;
	}
	
};
set <long long> merge(set<long long>& a, set <long long>& b)
{
	set <long long> wynik;
	wynik.insert(a.begin(), a.end());
	wynik.insert(b.begin(), b.end());
	return wynik;
}
set <long long> merge(set<long long>& a, set <long long>& b, set <long long>& c)
{
	set <long long> wynik;
	wynik.insert(a.begin(), a.end());
	wynik.insert(b.begin(), b.end());
	wynik.insert(c.begin(), c.end());
	return wynik;
}
int main(int argc, char** argv)
{
	fstream html(argv[3], ios::out | ios::trunc);
	html<<"<html>"<<endl;
	csvfile csv1((string)argv[4]);
	csvfile csv2((string)argv[5]);
	csvfile csv3((string)argv[6]);
	char data[20000];
	string arg1=argv[1];
	string arg2=argv[2];
	osm_base bazuka(arg2);
	dijkstra dij;
	dij.laduj_dijkstra_from_base(bazuka);
	DijkstraTram dijTra;
	dijTra.laduj_dijkstra_from_base(bazuka);
	DijkstraRail dijRail;
	dijRail.laduj_dijkstra_from_base(bazuka);
	ztmread_for_html ztm(arg2, arg1, merge(dijRail.eee3, dij.eee3, dijTra.eee3));
	html.precision(9);
	map <string, vector< vector<przystanek_big> > >::iterator it1=ztm.dane_linia.begin();
	set<string> nie_w_porzadku;
	while(it1!=ztm.dane_linia.end())
	{
		bool w_porzadku=1;
		string akt_linia=it1->first;
		pair<long long, vector <long long> > akt_rel=relacje_linia(bazuka, 3651336,akt_linia);
		html<<"<div>";
		html<<"<b>";
		html<<"<a href=\"http://ztm.waw.pl/rozklad_nowy.php?c=182&l=1&q="<<akt_linia<<"\">";
		html<<akt_linia<<"</a></b> ";
		set <string> extr;
		set <string> extr0;
		for(int i=0; i<it1->second.size(); i++)
		{
			int s1=it1->second[i].size();
			for(int j=0; j<s1; j++)
			{
				extr0.insert((it1->second)[i][j].id);
			}
		}
		vector <int> longs_wyniki;
		html<<"<a href=\"http://www.openstreetmap.org/relation/"<<akt_rel.first<<"\">"<<akt_rel.first<<"</a></br>";
		for(int i=0; i<akt_rel.second.size(); i++)
		{
			html<<"<a href=\"http://www.openstreetmap.org/relation/"<<akt_rel.second[i]<<"\">"<<akt_rel.second[i]<<"</a>";
			html<<" "<<bazuka.relations[akt_rel.second[i]].tags["name"]<<" "<<endl;
			set<string> extr1=extract_ref(bazuka, akt_rel.second[i]);
			set<string>::iterator it3=extr1.begin();
			map <long long, string> longs;
			int wynik_part=0;
			while(it3!=extr1.end())
			{
				if(ztm.dane0.find(*it3)!=ztm.dane0.end())
				{
					if(ztm.dane0[*it3].stop_position>0)
						longs[ztm.dane0[*it3].stop_position]=ztm.dane0[*it3].id;
				}
				else
					wynik_part++;
				it3++;
			}
			map <long long, string> szu=szukaj_na_drogach(bazuka, akt_rel.second[i], longs);
			map <long long, string>::iterator it4=szu.begin();
			while(it4!=szu.end())
			{
				html<<"<a href=\"http://www.openstreetmap.org/node/"<<it4->first<<"\">"<<it4->second<<" "<<ztm.dane0[it4->second].name<<"</a> ";
				it4++;
			}
			wynik_part+=szu.size();
			longs_wyniki.push_back(wynik_part);
			set<string>::iterator it2=extr1.begin();
			while(it2!=extr1.end())
			{
				extr.insert(*it2);
				it2++;
			}
			html<<"</br>"<<endl;
		}
		set<string>::iterator it3=extr.begin();
		while(it3!=extr.end())
		{
			if(extr0.find(*it3)==extr0.end())
			{
				html<<*it3;
				if(ztm.dane0.find(*it3)!=ztm.dane0.end())
					html<<"-"<<ztm.dane0[*it3].name;
				html<<" OSM ";
				w_porzadku=0;
			}
			it3++;
		}
		it3=extr0.begin();
		while(it3!=extr0.end())
		{
			if(extr.find(*it3)==extr.end())
			{
				html<<*it3<<"-"<<ztm.dane0[*it3].name<<" ZTM ";
				w_porzadku=0;
			}
			it3++;
		}
		if(!w_porzadku)
			nie_w_porzadku.insert(it1->first);
		html<<"</br>"<<endl;
		html<<"</div>"<<endl;
		it1++;
	}
	html<<"<table>"<<endl;
	int s9=ztm.dane.size();
	for(int j=0; j<s9; j++)
	{
		przystanek_big& teraz = ztm.dane[j];
		string name=teraz.name;
		string id=teraz.id;
		double akt_lon=teraz.lon;
		double akt_lat=teraz.lat;
		if(true)
		{
			html<<"<tr>";
			html<<"<td>"<<akt_lon<<"</td>";
			html<<"<td>"<<akt_lat<<"</td>";
			html<<"<td> <a href=\"http://www.openstreetmap.org/#map=19/"<<akt_lat<<"/"<<akt_lon<<"\"/>";
			html<<name<<" "<<id[4]<<id[5]<<"</a></td>";
			html<<"<td>";
			html<<"<a href=\"http://ztm.waw.pl/rozklad_nowy.php?c=183&l=1&a="<<id[0]<<id[1]<<id[2]<<id[3]<<"\">";
			html<<id<<"</a></td>";
			bool ok=0;
			html<<"<td>";
			if(teraz.bus_stop!=0)
			{
				html<<"bus/tram_stop: "<<teraz.to_bus_stop*1000<<" m; "<<"<a href=\"http://openstreetmap.org/node/"<<teraz.bus_stop<<"\" >"<<teraz.bus_stop<<"</a>";
				ok=1;
			}
			html<<"</td>";
			html<<"<td>";
			if(teraz.stop_position!=0)
			{
				if(teraz.pos_error)
				{
					csv3.send(akt_lon, akt_lat, name, id);
					html<<"STOP POSITION NIE LEŻY NA DRODZE!!! ";
				}
				html<<"stop_position: "<<teraz.to_stop_position*1000<<" m; "<<"<a href=\"http://openstreetmap.org/node/"<<teraz.stop_position<<"\" >"<<teraz.stop_position<<"</a>";
				ok=1;
			}
			else
			{
				if(ok)
					csv1.send(akt_lon, akt_lat, name, id);
			}
			html<<"</td>";
			html<<"<td>";
			if(!ok)
			{
				csv2.send(akt_lon, akt_lat, name, id);
					html<<"BRAK OTAGOWANIA";
			}
			html<<"</td>";
			html<<"</tr>"<<endl;
		}
	}
	html<<"</table></html>"<<endl;
	set<string>::iterator it8=nie_w_porzadku.begin();
	while(it8!=nie_w_porzadku.end())
	{
		if((*it8)[0]!='L' && (*it8)[0]!='K' && (*it8)[0]!='S' && *it8!="WKD")
			cout<<*it8<<" ";
		it8++;
	}
	cout<<endl;
}
