#ifndef READZTM_HPP
#define READZTM_HPP
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include "stringspecial.hpp"
using namespace std;
enum typ_postoju
{
	ZWYKLY, ZJAZDOWY, KONCOWY
};
enum dni_kursowania
{

};
struct postoj
{
	string stop_id;
	int time;
	typ_postoju typ;
};
struct kurs
{
	string linia;
	vector <postoj> postoje;
	dni_kursowania dni;
};
struct przystanek
{
	string name;
	string id;
	double lon;
	double lat;
};


class ztmread
{
	string sciez;
	public:
	virtual void nowy_kurs(kurs nowy)
	{
	}
	virtual void nowy_przystanek(przystanek nowy)
	{
	}
	virtual void nowa_linia(string nazwa, vector <vector <string> > trasy)
	{
	}
	ztmread (string sciezka) : sciez(sciezka)
	{
	}
	void run()
	{
		fstream plik;
		plik.open(sciez.c_str());
		char data[100000];
		while(plik.getline(data,100000))
		{
			stringstream plt;
			plt<<data;
			string lll;
			plt>>lll;
			int s9=lll.size();
			string roftl;
			for(int i=1; i<s9; i++)
			{
				roftl+=lll[i];
			}
			if(roftl=="ZP")
				readzp(roftl, plik);
			else
			{
				if(roftl=="LL")
					readll(roftl, plik);
				else
					readobszar(roftl, plik);
			}
		}
	}
	private:
	string doprzecinka(stringstream& ttt)
	{
		bool ok=1;
		string lol;
		string wynik;
		int licznik=0;
		while(ok && ttt>>lol)
		{
			if(licznik>0)
				wynik+="_";
			wynik+=lol;
			licznik++;
			if(lol.length()>0)
				if(lol[lol.length()-1]==',')
					ok=0;
		}
		string wynik2;
		for(int i=0; i<(wynik.length()-1); i++)
			wynik2+=wynik[i];
		return wynik2;
	}
	void readobszar(string nazwa, fstream& plik)
	{
		nazwa="#"+nazwa;
		char data[100000];
		bool ok=1;
		while(ok && plik.getline(data, 100000))
		{
			stringstream plt;
			plt<<data;
			string lll;
			plt>>lll;
			if(lll==nazwa)
				ok=0;
		}
	}
	void readwk(string nazwa, fstream& plik, string nazwa2)
	{
		vector <postoj> postoje;
		nazwa="#"+nazwa;
		char data[100000];
		bool ok=1;
		while(ok && plik.getline(data, 100000))
		{
			stringstream plt;
			plt<<data;
			string lll;
			plt>>lll;
			if(lll==nazwa)
				ok=0;
			else
			{
				string id;
				plt>>id;
				string type;
				plt>>type;
				string time;
				plt>>time;
				postoj foo;
				int czas=get_times(time)*60;
				if(czas<(3*60*60))
					czas+=(1440*60);
				foo.time=czas;
				foo.stop_id=id;
				postoje.push_back(foo);
				foo.typ=ZWYKLY;
				if(!plt.eof())
				{
					string zik;
					plt>>zik;
					if(zik=="B")
					{
						foo.typ=ZJAZDOWY;
					}
					if(zik=="P")
					{
						foo.typ=KONCOWY;
						kurs nowy;
						nowy.postoje=postoje;
						nowy.linia=nazwa2;
						nowy_kurs(nowy);
						postoje.clear();
					}
				}
			}
		}
	}

	void pominlinie(int liczba, fstream& plik)
	{
		char data[100000];
		for(int i=0; i<liczba; i++)
		{
			plik.getline(data, 100000);
		}
	}
	string podkr(char* data)
	{
		stringstream zonk;
		zonk<<data;
		int licznik=0;
		string tmp;
		string wynik;
		while(zonk>>tmp)
		{
			if(licznik>0)
				wynik+="_";
			wynik+=tmp;
			licznik++;
		}
		return wynik;
	}
	void readll(string nazwa, fstream& plik)
	{
		vector <vector <string> > trasy;
		nazwa="#"+nazwa;
		char data[100000];
		bool ok=1;
		string nazwa1;
		while(ok && plik.getline(data, 100000))
		{
			stringstream plt;
			plt<<data;
			string lll;
			plt>>lll;
			if(lll==nazwa)
				ok=0;
			if(lll=="*WK")
			{
				nowa_linia(nazwa1, trasy);
				readwk("WK", plik, nazwa1);
			}
			if(lll=="*LW")
			{
				trasy.push_back(readlw("LW", plik));
			}
			if(lll=="Linia:")
			{
				trasy.clear();
				string pup;
				plt>>pup;
				nazwa1=pup;
			}
		}
	}
	string searchdigits(string data2)
	{
		for(int i=0; i<data2.length()-6; i++)
		{
			bool ok=1;
			for(int j=0; j<6; j++)
			{
				if(data2[i+j]<'0' || data2[i+j]>'9')
					ok=0;
			}
			string wynik;
			if(ok)
			{
				for(int j=0; j<6; j++)
					wynik+=data2[i+j];
				return wynik;
			}
		}
		return "";
	}
	vector <string> readlw(string nazwa, fstream& plik)
	{
		vector <string> lista_przystankow;
		nazwa="#"+nazwa;
		char data[100000];
		bool ok=1;
		string aktnazwa;
		while(ok && plik.getline(data, 100000))
		{
			stringstream plt;
			plt<<data;
			string lll;
			plt>>lll;
			if(lll==nazwa)
				ok=0;
			else
			{
				string data2=data;
				string stop=searchdigits(data2);
				if(stop!="")
				{
					lista_przystankow.push_back(stop);
				}
			}
		}
		return lista_przystankow;
	}
	void readpr(string nazwa, fstream& plik, string akt)
	{
		nazwa="#"+nazwa;
		char data[100000];
		bool ok=1;
		double y1=0;
		double x1=0;
		while(ok && plik.getline(data, 100000))
		{
			stringstream plt;
			plt<<data;
			string lll;
			plt>>lll;
			if(lll==nazwa)
				ok=0;
			else
			{
				int licz;
				plt>>licz;
				string gowno;
				if(!plt.eof())
					plt>>gowno;
				string alfa=doprzecinka(plt);
				string beta;
				if(!plt.eof())
					plt>>gowno;
				if(!plt.eof())
					beta=doprzecinka(plt);
				if(!plt.eof())
				{
					plt>>gowno;
					bool okbis=1;
					if(gowno.length()>=3)
					{
						if(gowno[2]=='y')
							okbis=0;
					}
					if(okbis)
					{
						plt>>y1;
						plt>>gowno;
						plt>>x1;
					}
				}
				przystanek foo;
				foo.name=akt;
				foo.id=lll;
				foo.lon=x1;
				foo.lat=y1;
				nowy_przystanek(foo);
				pominlinie(licz, plik);
			}
		}
	}

	void readzp(string nazwa, fstream& plik)
	{
		nazwa="#"+nazwa;
		char data[100000];
		bool ok=1;
		string aktnazwa;
		while(ok && plik.getline(data, 100000))
		{
			stringstream plt;
			plt<<data;
			string lll;
			plt>>lll;
			if(lll==nazwa)
				ok=0;
			else
			{
				if(lll=="*PR")
				{
					readpr("PR", plik, aktnazwa);
				}
				else
				{
					aktnazwa=doprzecinka(plt);
				}
			}
		}
	}
};
#endif
