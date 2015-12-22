#ifndef SCHEDULEREADERLODZ
#define SCHEDULEREADERLODZ
#include "ScheduleReader.hpp"
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
