#ifndef SCHEDULEREADER
#define SCHEDULEREADER
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
	string id;
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
	string stopinfo;
};

class ScheduleHandler
{
	public:
	virtual void nowy_kurs(kurs nowy) {}
	virtual void nowy_przystanek(przystanek nowy) {}
	virtual void nowa_linia(string nazwa, vector <vector <string> > trasy) {}
	virtual void newkalendar(string date, vector<string>caltypes) {}
};

class ScheduleReader
{
	protected:
	string sciez;
	ScheduleHandler* hand;
	public:
	virtual void run() {}
	ScheduleReader (string sciezka, ScheduleHandler* handler) : hand(handler), sciez(sciezka) {}
};

class ScheduleReaderWarszawa : public ScheduleReader
{
	public:
	ScheduleReaderWarszawa (string sciezka, ScheduleHandler* handler) : ScheduleReader(sciezka, handler) {}
	void run();
	private:
	string doprzecinka(stringstream& ttt);
	bool porpolskie(string tekst, int i, string znak);
	bool polskiznak(string tekst, int i);	string znakmale(string tekst, int i);
	string duzemale(string skrot);
	string zaprzecinek(stringstream& ttt);
	void readobszar(string nazwa, fstream& plik);
	void readka(string nazwa, fstream& plik);
	void readwk(string nazwa, fstream& plik, string nazwa2);

	void pominlinie(int liczba, fstream& plik);
	string podkr(char* data);
	void readll(string nazwa, fstream& plik);
	string searchdigits(string data2);
	vector <string> readlw(string nazwa, fstream& plik);
	void readpr(string nazwa, fstream& plik, string akt, string aktmiasto);
	void readzp(string nazwa, fstream& plik);
};


class ScheduleReaderMetro : public ScheduleReader
{
	public:
	ScheduleReaderMetro(string sciezka, ScheduleHandler* handler) : ScheduleReader (sciezka, handler)
	{
	}
	string readForDelimeter(fstream& alpha, string& out);
	void run();
};

class ScheduleReaderGdansk : public ScheduleReader
{
	map <string, string> przystankiTmp;
	map <string, vector <vector <string> > > linieTmp;
	string ref(string komorka)
	{
		int i=0;
		bool ok=0;
		while(!ok && i<komorka.length()-1)
		{
			if(komorka[i]=='P')
				if(komorka[i+1]=='(')
					ok=1;
			i++;
		}
		i++;
		string wyn;
		while(komorka[i]!=')' && i<komorka.length())
		{
			wyn+=komorka[i];
			i++;
		}
		return wyn;
	}
	bool cmp(vector <vector <string> >& in, vector <string>& in2)
	{
		for(auto& it1 : in)
		{
			bool ok=1;
			if(it1.size()==in2.size())
			{
				for(int j=0; j<in2.size(); j++)
				{
					if(in2[j]!=it1[j])
						ok=0;
				}
			}
			else
			{
				ok=0;
			}
			if(ok)
				return true;
		}
		return false;
	}
	bool notempty(string foo)
	{
		for(int i=0; i<foo.length(); i++)
			if(isalnum(foo[i]))
				return true;
		return false;
	}

	void uzupelnij(string sciezka2)
	{
		vector <vector <string> > tabela;
		fstream plik;
		plik.open(sciezka2.c_str());
		string foo;
		while(getline(plik, foo))
		{
			vector <string> row;
			stringstream foo2;
			foo2<<foo;
			string foo3;
			while(getline(foo2, foo3, ';'))
				row.push_back(foo3);
			tabela.push_back(row);
		}
		plik.close();
		for(int i=1; i<tabela.size(); i++)
		{
			przystankiTmp[ref(tabela[i][1])]=tabela[i][3];
		}
		string idLinii=tabela[0][0];
		for(int i=4; i<tabela[0].size(); i++)
		{
			vector <string> posrednie;
			for(int j=1; j<tabela.size(); j++)
			{
				if(notempty(tabela[j][i]))
					posrednie.push_back(ref(tabela[j][1]));
			}
			if(!cmp(linieTmp[idLinii], posrednie))
				linieTmp[idLinii].push_back(posrednie);
		}
	}
	vector <string> szerokaLista(string path)
	{
		vector <string> wynik;
		string pol="ls "+path+"/*/*warianty*.csv";
		FILE* file = popen(pol.c_str(), "r");
		char buffer[256];
		while(fscanf(file, "%100s", buffer) !=EOF)
		{
			string pol2 = "iconv -f windows-1250 -t utf-8 < "+(string)(buffer)+" > "+(string)(buffer)+"2";
			system(pol2.c_str());
			wynik.push_back((string)(buffer)+"2");
		}
		pclose(file);
		return wynik;
	}
	public:
	ScheduleReaderGdansk (string sciezka, ScheduleHandler* handler) : ScheduleReader(sciezka, handler) {}
	void run()
	{
		cout<<"start..."<<endl;
		vector <string> lplik = szerokaLista(sciez);
		for(int i=0; i<lplik.size(); i++)
		{
			uzupelnij(lplik[i]);
		}
		for(auto& it1 : przystankiTmp)
		{
			przystanek przyst;
			przyst.id=it1.first;
			przyst.name=it1.second;
			hand->nowy_przystanek(przyst);
		}
		for(auto& it1 : linieTmp)
		{
			hand->nowa_linia(it1.first, it1.second);
		}
	}
};

#endif
