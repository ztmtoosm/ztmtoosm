#include "../../include/sciezka1.hpp"
#include "../../include/RareToThickRoutes/dij_data.hpp"
#include "../../include/RareToThickRoutes/dijkstra.hpp"
#include "fcgi_stdio.h"
void wypisz(stringstream& lol)
{
	printf("%s", lol.str().c_str());
}
string lel()
{
	string wynik;
	int alfa=FCGI_fgetc(FCGI_stdin);
	while(alfa>0)
	{
		wynik+=(char)(alfa);
		alfa=FCGI_fgetc(FCGI_stdin);
	}
	return wynik;
}
map <string, string> mapaenv()
{
	map<string,string>wynik;
	string podstawa=getenv("QUERY_STRING");
	int s1=podstawa.length();
	int i=0;
	while(i<s1)
	{
		string key, value;
		while(podstawa[i]!='=' && i<s1)
		{
			key+=podstawa[i];
			i++;
		}
		i++;
		while(podstawa[i]!='&' && i<s1)
		{
			value+=podstawa[i];
			i++;
		}
		i++;
		wynik[key]=value;
	}
	return wynik;
}
int main(int argc, char** argv)
{
	string osmBasePath = "/ztmtoosm/data/latest.osm";
	osm_base bazaOsm(osmBasePath);
	vector <Przelicznik*> przeliczniki;
	przeliczniki.push_back(new PrzelicznikBus());
	przeliczniki.push_back(new PrzelicznikTram());
	dijkstra dij(przeliczniki);
	dij.laduj_dijkstra_from_base(&bazaOsm);
	while(FCGI_Accept() >= 0)
	{
		stringstream wyp;
		wyp.precision(9);
		map<string, string>env=mapaenv();
		if(env.size()==0)
		{
			wyp<<"Content-type: application/txt\n\n";
			int t =time(NULL);
			stringstream xyz;
			xyz<<t;
			string sp="";
			string scie = SCIEZKA1+sp+"/www/"+xyz.str()+".json";
			fstream plik(scie, ios::out | ios::trunc);
			plik<<lel()<<endl;
			plik.close();
			string pol  = SCIEZKA1+sp+"/bin/testparse "+xyz.str();
			int wyn=system(pol.c_str());
			if(wyn!=-1)
				wyp<<t<<endl;
			else
				wyp<<-1<<endl;
			wypisz(wyp);
		}
		else
		{
		wyp<<"Content-type: application/json\n\n";
		if(env.find("y")!=env.end() || env.find("val")!=env.end())
		{
			double wynik=1000000;
			long long val = 0;
			if(env.find("val")!=env.end())
			{
				val = fromstring<long long>(env["val"]);
			}
			else
			{
				map <long long, node>::iterator it1=bazaOsm.nodes.begin();
				while(it1!=bazaOsm.nodes.end())
				{
					double kand = distance(it1->second.lat, it1->second.lon, fromstring<double>(env["y"]), fromstring<double>(env["x"]));
					if(wynik>kand)
					{
						val=it1->first;
						wynik=kand;
					}
					it1++;
				}
			}
			wyp<<"{\"id\":"<<val<<", \"y\":"<<bazaOsm.nodes[val].lat<<", \"x\":"<<bazaOsm.nodes[val].lon<<"}"<<endl;
			wypisz(wyp);
		}
		else
		{
			vector <long long> rareNodes;
			stringstream al1;
			string przel = env["przelicznik"];
			if(przel=="")
				przel="0";
			al1<<env["from"]<<" "<<env["to"]<<" "<<przel;
			long long xxx;
			al1>>xxx;
			rareNodes.push_back(xxx);
			al1>>xxx;
			rareNodes.push_back(xxx);
			int przeli;
			al1>>przeli;
			dij_data out(rareNodes, &dij, przeli);
			vector <long long> out2 = out.all_nodes;
			cout<<przeli<<endl;
			wyp<<"[";
			for(int i=0; i<out2.size(); i++)
			{
				wyp<<"{\"id\": "<<out2[i]<<", \"y\": "<<bazaOsm.nodes[out2[i]].lat<<", \"x\": "<<bazaOsm.nodes[out2[i]].lon<<"}";
				if(i<out2.size()-1)
					wyp<<",";
				wyp<<endl;
			}
			wyp<<"]";
			wypisz(wyp);
		}
		}
	}
}
