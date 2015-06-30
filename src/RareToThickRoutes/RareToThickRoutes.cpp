#include <pqxx/pqxx>
#include "conf.h"
#include "dij_data.hpp"
#include "dijkstra.hpp"
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

void getTrack(long long id1, long long id2, stringstream& wyp)
{
	pqxx::connection c("dbname=gis user=root");
	pqxx::work txn(c);
	int idd1=getDatabaseNodeId(id1, txn);
	int idd2=getDatabaseNodeId(id2, txn);
	stringstream polecenie;
	polecenie<<"SELECT lat,lon, b.id FROM pgr_dijkstra(' \
	SELECT key_column AS id,\
	source::int,\
	target::int,\
	vals::double precision AS cost\
	FROM ways2',"<<idd1<<", "<<idd2<<", false, false) a JOIN planet_osm_nodes b ON id1=key_column ORDER BY seq";
	pqxx::result r = txn.exec(polecenie.str());
	wyp<<"[";
	for(int i=0; i<r.size(); i++)
	{
		double y = r[i][0].as<int>()/10000000.0;
		double x = r[i][1].as<int>()/10000000.0;
		if(i>0)
			std::cout<<",";
		wyp<<"{ \"y\":"<<y<<", \"x\":"<<x<<", \"id\": "<<r[i][2]<<endl;
	}
	wyp<<"]";
}


int main(int argc, char** argv)
{
	string osmBasePath = "/ztmtoosm/data/warszawa.osm";
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
			string scie = MYPROJECT_DIR+sp+"/www/"+xyz.str()+".json";
			fstream plik(scie, ios::out | ios::trunc);
			plik<<lel()<<endl;
			plik.close();
			string pol  = MYPROJECT_DIR+sp+"/bin/testparse "+xyz.str();
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
			long long xxx, yyy;
			al1>>xxx;
			rareNodes.push_back(xxx);
			al1>>yyy;
			rareNodes.push_back(yyy);
			int przeli;
			al1>>przeli;
			if(przeli!=9)
			{
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
			}
			else
			{
				getTrack(xxx, yyy, wyp);
			}
			wypisz(wyp);
		}
		}
	}
}
