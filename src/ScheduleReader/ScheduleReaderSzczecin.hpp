#ifndef SCHEDULEREADERSZCZECIN
#define SCHEDULEREADERSZCZECIN
#include "ScheduleReader.hpp"
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
#endif
