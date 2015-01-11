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
	string dni;
};
struct przystanek
{
	string name;
	string id;
	string miejscowosc;
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
			{
				readzp(roftl, plik);
			}
			else
			{
				if(roftl=="LL")
					readll(roftl, plik);
				else
					readobszar(roftl, plik);
			}
		}
		plik.close();
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
	bool porpolskie(string tekst, int i, string znak)
	{
		if(i>tekst.length()-1)
			return false;
		string tmp;
		tmp+=tekst[i];
		tmp+=tekst[i+1];
		if(tmp==znak)
			return true;
		return false;
	}
	bool polskiznak(string tekst, int i)
	{
		if(porpolskie(tekst, i, "Ą")) return true;
		if(porpolskie(tekst, i, "Ć")) return true;
		if(porpolskie(tekst, i, "Ę")) return true;
		if(porpolskie(tekst, i, "Ł")) return true;
		if(porpolskie(tekst, i, "Ń")) return true;
		if(porpolskie(tekst, i, "Ó")) return true;
		if(porpolskie(tekst, i, "Ś")) return true;
		if(porpolskie(tekst, i, "Ź")) return true;
		if(porpolskie(tekst, i, "Ż")) return true;
		return false;
	}
	string znakmale(string tekst, int i)
	{
		if(porpolskie(tekst, i, "Ą")) return "ą";
		if(porpolskie(tekst, i, "Ć")) return "ć";
		if(porpolskie(tekst, i, "Ę")) return "ę";
		if(porpolskie(tekst, i, "Ł")) return "ł";
		if(porpolskie(tekst, i, "Ń")) return "ń";
		if(porpolskie(tekst, i, "Ó")) return "ó";
		if(porpolskie(tekst, i, "Ś")) return "ś";
		if(porpolskie(tekst, i, "Ź")) return "ź";
		if(porpolskie(tekst, i, "Ż")) return "ż";
		return "";
	}
	string duzemale(string skrot)
	{
		string wynik;
		if(skrot.size()==0)
			return "";
		wynik+=skrot[0];
		for(int i=1; i<skrot.length(); i++)
		{
			if(polskiznak(skrot, i))
			{
				wynik+=znakmale(skrot, i);
				i++;
			}
			else
			{
				wynik+=tolower(skrot[i]);
			}
		}
		return wynik;
	}
	string zaprzecinek(stringstream& ttt)
	{
		string skrot;
		ttt>>skrot;
		string wynik;
		while(ttt>>skrot)
		{
			wynik+=duzemale(skrot)+" ";
		}
		return wynik.substr(0, max(0, (int)wynik.length()-1));
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
						nowy.dni=type;
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
				if(stop!="" && stop.length()==6)
				{
					if(stop[4]!='7')
						lista_przystankow.push_back(stop);
				}
			}
		}
		return lista_przystankow;
	}
	void readpr(string nazwa, fstream& plik, string akt, string aktmiasto)
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
				cout<<aktmiasto<<"#"<<endl;
				foo.miejscowosc=aktmiasto;
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
		string aktmiasto;
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
					readpr("PR", plik, aktnazwa, aktmiasto);
				}
				else
				{
					aktnazwa=doprzecinka(plt);
					aktmiasto=zaprzecinek(plt);
				}
			}
		}
	}
};
#endif
