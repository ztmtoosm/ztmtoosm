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
using namespace std;
using namespace rapidxml;

class csvfile
{
	fstream plik;
	string sciezka;
	public:
	csvfile(string sciezka)
	{
		cout<<"ok"<<endl;
		plik.open(sciezka.c_str(), ios::out | ios::trunc);
		plik<<"longitude;latitude;highway;name;ref"<<endl;
	}
	void send(double lon, double lat, string name, string ref)
	{
		cout<<"ok+"<<endl;
		plik<<lon<<";"<<lat<<";bus_stop;"<<name<<";"<<ref<<endl;
	}
	
};

int main(int argc, char** argv)
{
	cout<<argc<<endl;
	cout<<argv[7]<<endl;
	if(argc<3)
	{
		cout<<"za mało argumentów..."<<endl;
		return 0;
	}
	cout<<"Ładowanie OSM..."<<endl;
	fstream html(argv[4], ios::out | ios::trunc);
	html<<"<html><table>"<<endl;
	csvfile csv1((string)argv[5]);
	csvfile csv2((string)argv[6]);
	csvfile csv3((string)argv[7]);
	char data[20000];
	string arg1=argv[1];
	string arg2=argv[2];
	string arg3=argv[3];
	cout<<"Ładowanie ZTM..."<<endl;
	ztmread_for_html ztm(arg1, arg2, arg3);
	cout<<"Obróbka danych..."<<endl;
	html.precision(9);
	int s9=ztm.dane.size();
	for(int j=0; j<s9; j++)
	{
		przystanek_big& teraz = ztm.dane[j];
		string name=teraz.name;
		string id=teraz.id;
		double akt_lon=teraz.lon;
		double akt_lat=teraz.lat;
		if(id[1]!='9' && id[4]<'5')
		{
			html<<"<tr>";
			html<<"<td>"<<akt_lon<<"</td>";
			html<<"<td>"<<akt_lat<<"</td>";
			html<<"<td> <a href=\"http://www.openstreetmap.org/#map=19/"<<akt_lat<<"/"<<akt_lon<<"\"/>";
			html<<name<<" "<<id[4]<<id[5]<<"</a></td>";
			html<<"<td>"<<id<<"</td>";
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
}
