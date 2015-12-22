#ifndef SCHEDULEREADERGDANSK
#define SCHEDULEREADERGDANSK
#include "ScheduleReader.hpp"
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
				time_t rawtime;
				struct tm * timeinfo;
				time ( &rawtime );
				timeinfo = gmtime ( &rawtime );
				timeinfo->tm_year = year - 1900;
				timeinfo->tm_mon = month - 1;
				timeinfo->tm_mday = day;
				timeinfo->tm_hour=0;
				timeinfo->tm_min=0;
				int wyn=mktime(timeinfo);
				if(wyn<(int)(time(NULL)))
					t.push_back(make_pair(wyn, buffer));
			}
			sort(t.begin(), t.end());
			if(t.size()>0)
			{
				int j=t.size()-1;
				while(j>=0 && t[t.size()-1].first==t[j].first)
				{
					buffers.insert(t[j].second);
					j--;
				}
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
#endif
