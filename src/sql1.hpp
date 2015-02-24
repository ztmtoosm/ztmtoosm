#include <stdlib.h>
#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

struct sql_routes
{
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
	sql_routes()
	{
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "");
		con->setSchema("osm");
		stmt = con->createStatement();
	}
	vector <pair <double, double> > get_route (string from, string to)
	{
		vector <pair <double, double> > wynik;
		try {
		res = stmt->executeQuery("SELECT * FROM relacje WHERE fromid=\""+from+"\" AND toid=\""+to+"\"");
		while (res->next()) 
		{
			wynik.push_back(pair<double,double>(res->getDouble(4), res->getDouble(5)));
		}
		}
		catch (sql::SQLException &e) {}
		return wynik;
	}
};

struct sql_polaczenia
{
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
	sql_polaczenia()
	{
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "");
		con->setSchema("osm");
		stmt = con->createStatement();
	}
	virtual string getNameP (string id)
	{
		return "";
	}
	pair<int, int> limity_polaczenia (string from, int time, string delim_type, bool sasiednie, int limit)
	{
		int maxW=0;
		int minW=2000000000;
		try 
		{
			stringstream polecenie;
			string from0=from.substr(0, 4);
			polecenie<<"SELECT * FROM polaczenia WHERE fromid LIKE \""<<from0<<"%\"";
			polecenie<<" AND fromtime"<<delim_type<<time<<" ";
			string xd="";
			if(delim_type=="<")
				xd="DESC";
			polecenie<<"ORDER BY fromtime "<<xd<<" LIMIT "<<limit;
			res = stmt->executeQuery(polecenie.str());
			while (res->next()) 
			{
				minW=min(minW, res->getInt("fromtime"));
				maxW=max(maxW, res->getInt("fromtime"));
			}
		}
		catch (sql::SQLException &e) {}
		return pair<int, int>(minW, maxW);
	}
	void wypisz_polaczenia (string from, int time, string delim_type, bool sasiednie, stringstream& out)
	{
		pair <int, int> limits;
		if(delim_type=="=")
		{
			limits.first=limity_polaczenia(from,time,"<",sasiednie,10).first;
			limits.second=limity_polaczenia(from,time,">",sasiednie,10).second;
		}
		else
		{
			limits=limity_polaczenia(from,time,delim_type,sasiednie,20);
		}
		out<<"[";
		bool ok=0;
		vector <pair <double, double> > wynik;
		try {
			stringstream polecenie;
			string from0=from.substr(0, 4);
			polecenie<<"SELECT * FROM polaczenia WHERE fromid LIKE \""<<from0<<"%\"";
			polecenie<<" AND fromtime>="<<limits.first<<" ";
			polecenie<<" AND fromtime<="<<limits.second<<" ";
			polecenie<<"ORDER BY fromtime";
		res = stmt->executeQuery(polecenie.str());
		while (res->next()) 
		{
			if(ok)
				out<<",";
			out<<"{\"time\": "<<res->getInt("fromtime");
			out<<",\"id\": \""<<res->getString("fromid")<<"\"";
			out<<",\"idname\": \""<<getNameP(res->getString("fromid"))<<"\", \"linia\": \"";
			out<<res->getString("linia")<<"\"}"<<endl;
			ok=1;
		}
		}
		catch (sql::SQLException &e) {}
		out<<"]";
	}
	virtual void wypiszKurs (string line, vector <pair <string, int> > pary, ostream& out)
	{
	}
	void wypiszKurs2 (string from, int time, string line, stringstream& out)
	{
		bool ok=0;
		vector <pair <double, double> > wynik;
		try {
			stringstream polecenie;
			polecenie<<"SELECT * FROM polaczenia WHERE fromid=\""<<from<<"\"";
			polecenie<<" AND fromtime="<<time<<" ";
			polecenie<<" AND linia=\""<<line<<"\"";
		res = stmt->executeQuery(polecenie.str());
		int nkol=-1;
		while (res->next()) 
		{
			nkol=res->getInt("nkol");
		}
		if(nkol!=-1)
		{
			stringstream polecenie2;
			polecenie2<<"SELECT * FROM polaczenia WHERE linia=\""<<line<<"\"";
			polecenie2<<" AND nkol="<<nkol<<" ";
			polecenie2<<" AND linia=\""<<line<<"\"";
			polecenie2<<"ORDER BY fromtime";
			res = stmt->executeQuery(polecenie2.str());
			bool ok=0;
			string ost_id="";
			int ost_time=-1;
			vector <pair<string, int> > dane;
			while (res->next()) 
			{
				ost_id=res->getString("toid");
				ost_time=res->getInt("totime");
				dane.push_back(make_pair(res->getString("fromid"), res->getInt("fromtime")));
			}
			dane.push_back(make_pair(ost_id, ost_time));
			wypiszKurs(line, dane, out);
		}
		}
		catch (sql::SQLException &e) {}
	}
};



class HafasBazaSQL : ztmread
{
	fstream plik;
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
	map <string, vector <string> > kalendarz;
	map <string, int> liczbaKursow;
	private:
	void newkalendar(string date, vector<string>caltypes)
	{
		kalendarz[date]=caltypes;
	}
	string datetostring(time_t data)
	{
		tm* tim = localtime(&data);
		stringstream foo;
		foo<<(tim->tm_year+1900);
		foo<<"-";
		if((tim->tm_mon)<9)
			foo<<"0";
		foo<<(tim->tm_mon+1)<<"-";
		if((tim->tm_mday)<10)
			foo<<"0";
		foo<<(tim->tm_mday);
		return foo.str();
	}
	time_t midnight(time_t data)
	{
		tm* tim = localtime(&data);
		tim->tm_sec=0;
		tim->tm_min=0;
		tim->tm_hour=0;
		return mktime(tim);
	}
	vector <int> diffs(string type)
	{
		vector <int> wynik;
		int today=midnight(time(NULL));
		for(int i=-1; i<=2; i++)
		{
			string data=datetostring(today+i*24*3600);
			bool ok=0;
			for(int j=0; j<kalendarz[data].size(); j++)
			{
				if(kalendarz[data][j]==type)
					ok=1;
			}
			if(ok)
			{
				wynik.push_back(today+i*24*3600);
			}
		}
		return wynik;
	}
	void nowy_kurs(kurs nowy)
	{
		vector <int> dify = diffs(nowy.dni);
		for(int g=0; g<dify.size(); g++)
		{
			liczbaKursow[nowy.linia]++;
			for(int i=0; i<nowy.postoje.size()-1; i++)
			{
				plik<<nowy.linia<<"	";
				plik<<nowy.postoje[i].stop_id<<"	";
				plik<<nowy.postoje[i+1].stop_id<<"	";
				plik<<nowy.postoje[i].time+dify[g]<<"	";
				plik<<nowy.postoje[i+1].time+dify[g]<<"	";
				plik<<liczbaKursow[nowy.linia]<<"	";
				plik<<endl;
			}
		}
	}
	void nowy_przystanek(przystanek nowy)
	{
		/*
		HafasStop* nowy2 = new HafasStop;
		nowy2->name = nowy.name;
		nowy2->id = nowy.id;
		nowy2->miejscowosc = nowy.miejscowosc;
		nowy2->wspol.lon = nowy.lon;
		nowy2->wspol.lat = nowy.lat;
		baza->przystanki[nowy.id] = nowy2;
		*/
	}
	void nowa_linia(string nazwa, vector <vector <string> > trasy)
	{
		/*
		baza->linie[nazwa]=new HafasLinia;
		baza->linie[nazwa]->id=nazwa;
		baza->linie[nazwa]->trasy=trasy;
		*/
	}
	public:
	HafasBazaSQL (string sciezka, string sciezka2) : ztmread(sciezka)
	{
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "");
		con->setSchema("osm");
		stmt = con->createStatement();
		plik.open(sciezka2.c_str(), ios::trunc | ios::out);
		run();
		plik.close();
	}
};
