#include <pqxx/pqxx>
#include "conf.h"
#include "stringspecial.hpp"
#include "fcgi_stdio.h"

void getNearestPoint(double x, double y, pqxx::work& txn, stringstream& wyp)
{
	double id1 = x;
	double id2 = y;
	long long idd1=id1*10000000;
	long long idd2=id2*10000000;
	stringstream polecenie;
	polecenie<<"SELECT lat, lon, id FROM planet_osm_nodes WHERE lat>"<<idd1-1000000<<" AND lat<"<<idd1+1000000;
	polecenie<<" AND lon>"<<idd2-1000000<<" AND lon<"<<idd2+1000000;
	polecenie<<" ORDER BY geom  <-> ST_GeometryFromText('POINT("<<id1<<" "<<id2<<")',4326) LIMIT 1";
	pqxx::result r = txn.exec(polecenie.str());
	for(int i=0; i<r.size(); i++)
	{
		double y = r[i][0].as<int>()/10000000.0;
		double x = r[i][1].as<int>()/10000000.0;
		wyp<<"{ \"y\":"<<y<<", \"x\":"<<x<<", \"id\": "<<r[i][2]<<"}"<<std::endl;
	}
}

void getId(long long id, pqxx::work& txn, stringstream& wyp)
{
	stringstream polecenie;
	polecenie<<"SELECT lat, lon, id FROM planet_osm_nodes WHERE id="<<id;
	polecenie<<" LIMIT 1";
	pqxx::result r = txn.exec(polecenie.str());
	for(int i=0; i<r.size(); i++)
	{
		double y = r[i][0].as<int>()/10000000.0;
		double x = r[i][1].as<int>()/10000000.0;
		wyp<<"{ \"y\":"<<y<<", \"x\":"<<x<<", \"id\": "<<r[i][2]<<"}"<<std::endl;
	}
}

int getDatabaseNodeId(long long foo, pqxx::work& txn)
{
	stringstream polecenie;
	polecenie<<"SELECT key_column FROM planet_osm_nodes WHERE id="<<foo<<";";
	pqxx::result r = txn.exec(polecenie.str());
	if(r.size()<=0)
		return -1;
	cout<<r[0][0].as<int>()<<endl;
	return r[0][0].as<int>();
}
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


void getTrack(long long id1, long long id2, pqxx::work& txn, stringstream& wyp)
{
	int idd1=getDatabaseNodeId(id1, txn);
	int idd2=getDatabaseNodeId(id2, txn);
	stringstream polecenie;
	polecenie<<"SELECT lat,lon, b.id FROM pgr_dijkstra(' \
	SELECT key_column AS id,\
	source::int,\
	target::int,\
	vals::double precision AS cost,\
	vals_rev::double precision AS reverse_cost\
	FROM ways2',"<<idd1<<", "<<idd2<<", true, true) a JOIN planet_osm_nodes b ON id1=key_column ORDER BY seq";
	pqxx::result r = txn.exec(polecenie.str());
	wyp<<"[";
	for(int i=0; i<r.size(); i++)
	{
		double y = r[i][0].as<int>()/10000000.0;
		double x = r[i][1].as<int>()/10000000.0;
		if(i>0)
			wyp<<",";
		wyp<<"{ \"y\":"<<y<<", \"x\":"<<x<<", \"id\": "<<r[i][2]<<"}"<<endl;
	}
	wyp<<"]";
}


int main(int argc, char** argv)
{
	pqxx::connection c("dbname=gis user=root password=foo");
	pqxx::work txn(c);

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
				if(env.find("val")!=env.end())
				{
					getId(fromstring<long long>(env["val"]), txn, wyp);
				}
				else
				{
					getNearestPoint(fromstring<double>(env["y"]), fromstring<double>(env["x"]), txn, wyp);
				}
				wypisz(wyp);
			}
			else
			{
				stringstream al1;
				string przel = env["przelicznik"];
				if(przel=="")
					przel="0";
				al1<<env["from"]<<" "<<env["to"]<<" "<<przel;
				long long xxx, yyy;
				al1>>xxx;
				al1>>yyy;
				int przeli;
				al1>>przeli;
				getTrack(xxx, yyy, txn, wyp);
				wypisz(wyp);
			}
		}
	}
}
