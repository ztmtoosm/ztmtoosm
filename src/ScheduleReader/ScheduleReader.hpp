#ifndef SCHEDULEREADER
#define SCHEDULEREADER
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include <algorithm>
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
	bool ok_stop(string komorka)
	{
		cout<<komorka<<endl;
		if(komorka.find(",H")!=string::npos)
		{
			return false;
		}
		return true;
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
	bool isSubRoute(vector <string>& testowany, vector <string>& tester)
	{
		if(testowany.size()==0)
			return false;
		for(int i=0; i<tester.size(); i++)
		{
			bool ok_tmp = true;
			if(tester[i]==testowany[0])
			{
				for(int j=0; j<testowany.size() && ok_tmp; j++)
				{
					if((i+j)>=tester.size())
						ok_tmp = false;
					else if(tester[i+j]!=testowany[j])
						ok_tmp = false;
				}
				if(ok_tmp)
					return true;
			}
		}
		return false;
	}
	bool isSubRoute(vector <vector <string> >& in, int pos)
	{
		bool ok = 0;
		for(int i=0; i<in.size(); i++)
		{
			if(i!=pos)
			{
				ok |= isSubRoute(in[pos], in[i]);
			}
		}
		return ok;
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
			bool ok = true;
			for(int j=1; j<tabela.size(); j++)
			{
				if(!ok_stop(tabela[j][1]) && notempty(tabela[j][i]))
					ok=false;	
				if(notempty(tabela[j][i]))
					posrednie.push_back(ref(tabela[j][1]));
			}
			if(!cmp(linieTmp[idLinii], posrednie) && ok && posrednie.size()>0)
				linieTmp[idLinii].push_back(posrednie);
		}
	}
	vector <string> szerokaLista(string path)
	{
		vector <string> wynik;
		string pol="ls "+path+"/*/*warianty*.csv";
		FILE* file = popen(pol.c_str(), "r");
		char buffer[256];
		set <string> buffers;
		map <string, vector <string> > buffers2;
		while(fscanf(file, "%100s", buffer) !=EOF)
		{
			//buffers.insert((string)(buffer));
			stringstream foo;
			foo<<buffer;
			string lin, date;
			getline(foo, lin, '_');
			getline(foo, date, '_');
			buffers2[lin].push_back((string)(buffer));
		}
		for(auto& it1 : buffers2)
		{
			vector <pair<int, string> > t;
			for(string buffer : it1.second)
			{
				stringstream foo;
				foo<<buffer;
				string lin, date;
				getline(foo, lin, '_');
				getline(foo, date, '_');
				stringstream foo2;
				foo2<<date.substr(0, 4)<<" "<<date.substr(4,2)<<" "<<date.substr(6,2);
				int year, month, day;
				foo2>>year>>month>>day;
				cout<<year<<" "<<month<<" "<<day<<" "<<lin<<" "<<buffer<<endl;
				time_t rawtime;
				struct tm * timeinfo;
				time ( &rawtime );
				timeinfo = localtime ( &rawtime );
				timeinfo->tm_year = year - 1900;
				timeinfo->tm_mon = month - 1;
				timeinfo->tm_mday = day;
				int wyn=mktime(timeinfo);
				t.push_back(make_pair(wyn, buffer));
			}
			sort(t.begin(), t.end());
			if(t.size()>0)
			{
				buffers.insert(t[t.size()-1].second);
			}
		}
		for(string buffer : buffers)
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
			vector <vector <string> > tmp = it1.second;
			vector <vector <string> > tmp2;
			for(int i=0; i<tmp.size(); i++)
			{
				if(!isSubRoute(tmp, i))
					tmp2.push_back(tmp[i]);
			}
			hand->nowa_linia(it1.first, tmp2);
		}
	}
};

class ScheduleReaderSzczecin : public ScheduleReader
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
	
	bool notempty(string foo)
	{
		for(int i=0; i<foo.length(); i++)
			if(isalnum(foo[i]))
				return true;
		return false;
	}

	vector <vector <string> > czytajPlik(string sciezka2)
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
			while(getline(foo2, foo3, ','))
			{
				string foo4;
				for(int j=0; j<foo3.length(); j++)
				{
					if(foo3[j]!='"')
						foo4+=foo3[j];
				}
				row.push_back(foo4);
			}
			tabela.push_back(row);
		}
		plik.close();
		return tabela;
	}
	void czytajPrzystanki(string sciezka2)
	{
		vector <vector <string> > tabela=czytajPlik(sciezka2);
		for(int i=1; i<tabela.size(); i++)
		{
			przystanek przyst;
			przyst.id=tabela[i][1];
			przyst.name=tabela[i][2];
			hand->nowy_przystanek(przyst);
		}
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
	void czytajTrasy(string routes, string trips, string stopTimes)
	{
		vector <vector <string> > routesTab=czytajPlik(routes);
		vector <vector <string> > tripsTab=czytajPlik(trips);
		map <string, string> tripsToRoutes;
		map <string, string> longtoShort;
		for(int i=1; i<routesTab.size(); i++)
		{
			longtoShort[routesTab[i][0]]=routesTab[i][1];
		}
		for(int i=1; i<tripsTab.size(); i++)
		{
			cerr<<longtoShort[tripsTab[i][0]]<<" "<<tripsTab[i][2]<<endl;
			tripsToRoutes[tripsTab[i][2]]=longtoShort[tripsTab[i][0]];
		}
		routesTab.clear();
		tripsTab.clear();
		map <string, vector <string> > posrednie;
		vector <vector <string> > stopTimesTab=czytajPlik(stopTimes);
		map <string, vector <vector <string> > > wynikiLinie;
		for(int i=1; i<stopTimesTab.size(); i++)
		{
			cerr<<stopTimesTab[i][0]<<" "<<stopTimesTab[i][3]<<endl;
			if(stopTimesTab[i][0][stopTimesTab[i][0].length()-1]=='0')
				posrednie[stopTimesTab[i][0]].push_back(stopTimesTab[i][3]);
			else
				cerr<<stopTimesTab[i][0]<<endl;
		}
		for(auto& it1 : posrednie)
		{
			string linia = tripsToRoutes[it1.first];
			if(linia.length()!=0 && !cmp(wynikiLinie[linia], it1.second))
				wynikiLinie[linia].push_back(it1.second);
		}
		for(auto& it1 : wynikiLinie)
		{
			hand->nowa_linia(it1.first, it1.second);
		}
	}
	public:
	ScheduleReaderSzczecin (string sciezka, ScheduleHandler* handler) : ScheduleReader(sciezka, handler) {}
	void run()
	{
		cout<<"start..."<<endl;
		czytajPrzystanki(sciez+"/stops.txt");
		for(auto& it1 : przystankiTmp)
		{
			przystanek przyst;
			przyst.id=it1.first;
			przyst.name=it1.second;
			hand->nowy_przystanek(przyst);
		}
		czytajTrasy(sciez+"/routes.txt", sciez+"/trips.txt", sciez+"/stop_times.txt");
	}
};

class ScheduleReaderLodz : public ScheduleReader
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
	bool ok_stop(string komorka)
	{
		cout<<komorka<<endl;
		if(komorka.find(",H")!=string::npos)
		{
			return false;
		}
		return true;
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
	bool isSubRoute(vector <string>& testowany, vector <string>& tester)
	{
		if(testowany.size()==0)
			return false;
		for(int i=0; i<tester.size(); i++)
		{
			bool ok_tmp = true;
			if(tester[i]==testowany[0])
			{
				for(int j=0; j<testowany.size() && ok_tmp; j++)
				{
					if((i+j)>=tester.size())
						ok_tmp = false;
					else if(tester[i+j]!=testowany[j])
						ok_tmp = false;
				}
				if(ok_tmp)
					return true;
			}
		}
		return false;
	}
	bool isSubRoute(vector <vector <string> >& in, int pos)
	{
		bool ok = 0;
		for(int i=0; i<in.size(); i++)
		{
			if(i!=pos)
			{
				ok |= isSubRoute(in[pos], in[i]);
			}
		}
		return ok;
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
			while(getline(foo2, foo3, ','))
			{
				row.push_back(foo3);
				cout<<foo3<<"# ";
			}
			tabela.push_back(row);
			cout<<endl;
		}
		plik.close();
		map <string, vector <string> > trasy;
		for(int i=1; i<tabela.size(); i++)
		{
			przystankiTmp[tabela[i][1]]=tabela[i][3];
			trasy[tabela[i][4]+"x"+tabela[i][5]].push_back(tabela[i][1]);
		}
		vector <vector <string> > ost;
		for(auto& it1 : trasy)
			ost.push_back(it1.second);
		linieTmp[tabela[1][0]]=ost;
	}
	vector <string> szerokaLista(string path)
	{
		vector <string> wynik;
		string pol="ls "+path+"/foo/*.inf2";
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
	ScheduleReaderLodz (string sciezka, ScheduleHandler* handler) : ScheduleReader(sciezka, handler) {}
	void run()
	{
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
			vector <vector <string> > tmp = it1.second;
			vector <vector <string> > tmp2;
			for(int i=0; i<tmp.size(); i++)
			{
				if(!isSubRoute(tmp, i))
					tmp2.push_back(tmp[i]);
			}
			hand->nowa_linia(it1.first, tmp2);
		}
	}
};


#endif
