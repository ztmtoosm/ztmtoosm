#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
using namespace std;
#ifndef READZTM_HPP
#define READZTM_HPP
struct plik2
{
	string name;
	string id;
	double lon;
	double lat;
};
class ztmread
{
	public:
	vector <plik2> dane;
	ztmread (string sciez)
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
				readobszar(roftl, plik);
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
				if(!plt.eof())
				{
					string zik;
					plt>>zik;
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
				readwk("WK", plik, nazwa1);
			if(lll=="Linia:")
			{
				string pup;
				plt>>pup;
				nazwa1=pup;
			}
		}
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
				if(x1>10 && y1>10)
				{
					plik2 foo;
					foo.name=akt;
					foo.id=lll;
					foo.lon=x1;
					foo.lat=y1;
					dane.push_back(foo);
				}
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
