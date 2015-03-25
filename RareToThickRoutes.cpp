#include "src/dij_data.hpp"
#include "src/dijkstra.hpp"
#include "fcgi_stdio.h"
void wypisz(stringstream& lol)
{
	printf("%s", lol.str().c_str());
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
	dijkstra dij;
	dij.laduj_dijkstra_from_base(&bazaOsm);
	while(FCGI_Accept() >= 0)
	{
		cout<<"elo"<<endl;
		stringstream wyp;
		wyp.precision(9);
		wyp<<"Content-type: application/json\n\n";
		map<string, string>env=mapaenv();
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
			al1<<env["from"]<<" "<<env["to"];
			long long xxx;
			al1>>xxx;
			rareNodes.push_back(xxx);
			al1>>xxx;
			rareNodes.push_back(xxx);
			dij_data out(rareNodes, &dij);
			vector <long long> out2 = out.all_nodes;
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
