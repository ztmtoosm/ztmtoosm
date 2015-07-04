#include <pqxx/pqxx>
#include <ctime>
#include <sys/time.h>
#include "conf.h"
#include "stringspecial.hpp"
#include "fcgi_stdio.h"
string debug;
string type;
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

double abs(double x, double y)
{
	double wyn=x-y;
	if(x-y<0)
		wyn*=-1;
	return wyn;
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

int getDatabaseNodeId(long long foo, pqxx::work& txn, double& lat, double& lon)
{
	stringstream polecenie;
	polecenie<<"SELECT key_column, lat, lon FROM planet_osm_nodes WHERE id="<<foo<<";";
	pqxx::result r = txn.exec(polecenie.str());
	if(r.size()<=0)
		return -1;
	lat=r[0][1].as<double>()/10000000.0;
	lon=r[0][2].as<double>()/10000000.0;
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
	struct timeval start, end;
	long mtime, seconds, useconds;    

	gettimeofday(&start, NULL);
	double lat1, lon1, lat2, lon2;
	int idd1=getDatabaseNodeId(id1, txn, lat1, lon1);
	int idd2=getDatabaseNodeId(id2, txn, lat2, lon2);
	double marginx=max(max(abs(lat1-lat2), abs(lon1, lon2)*0.6), 0.07);
	double marginy=max(max(abs(lat1-lat2)/0.6, abs(lon1, lon2)), 0.11);
	if(debug=="xxx")
	{
		marginx=0.01;
		marginy=0.01;
	}
	stringstream polecenie, polecenie2;
	polecenie<<"SELECT lat,lon, b.id FROM pgr_"<<type<<"(' \
	SELECT key_column AS id,\
	source::int,\
	target::int,\
	vals::double precision AS cost,\
	vals_rev::double precision AS reverse_cost, x1,y1,x2,y2\
	FROM ways2 ";
	if(debug!="fio")
		polecenie<<"WHERE x1>"<<min(lat1, lat2)-marginx<<" AND x1<"<<max(lat1,lat2)+marginx<<" AND y1>"<<min(lon1, lon2)-marginy<< " AND y1<"<<max(lon1,lon2)+marginy;
	polecenie<<"',"<<idd1<<", "<<idd2;
	polecenie<<", true, true) a JOIN planet_osm_nodes b ON id1=key_column ORDER BY seq";
	polecenie2<<"SELECT COUNT(*)\
	FROM ways2 WHERE x1>"<<min(lat1, lat2)-marginx<<" AND x1<"<<max(lat1,lat2)+marginx<<" AND y1>"<<min(lon1, lon2)-marginy<< " AND y1<"<<max(lon1,lon2)+marginy;
	
		pqxx::result g = txn.exec(polecenie2.str());
if(debug!="")
	{
		gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
		wyp<<mtime<<endl;


	}
	pqxx::result r = txn.exec(polecenie.str());
	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	wyp<<"[";
	for(int i=0; i<r.size(); i++)
	{
		double y = r[i][0].as<int>()/10000000.0;
		double x = r[i][1].as<int>()/10000000.0;
		if(i>0)
			wyp<<",";
		wyp<<"{ \"y\":"<<y<<", \"x\":"<<x<<", \"id\": "<<r[i][2];
		if(debug!="")
			wyp<<",\"ciapa\":\""<<mtime<< " ms\"";
		wyp<<" }"  <<endl;
	}
	wyp<<"]";
	if(debug!="")
	{
			wyp<<g[0][0].as<int>()<<" "<<lat1<<" "<<lon1<<" "<<lat2<<" "<<lon2<<" "<<polecenie.str()<<" "<<polecenie2.str()<<endl;
	}
}


int main(int argc, char** argv)
{
	type="dijkstra";
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
			debug=env["debug"];
			type=env["type"];
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
