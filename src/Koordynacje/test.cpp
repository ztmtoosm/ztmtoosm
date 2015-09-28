#include "conf.h"
#include "../ScheduleReader/ScheduleReader.hpp"
#include "md5.h"
#include <map>
#include <vector>
#include <fstream>
using namespace std;

class CoordinationHandler : public ScheduleHandler
{
	public:
	map <pair<string, string>, set <string> > mapa;
	vector <kurs*> kursy;
	map <string, set <pair<int, kurs*> > > mapa2;
	map <string, vector <vector <string> > > linie;
	map <string, string> przystanki;
	map <string, map <string, set <string> > > mapa7;
	map <string, string> hashOut;
	map <string, set<pair<string, int> > > lineInfo;
	map <string, double> hashLevels;
	double calculateLevel (vector <string> info)
	{
		int value = 100.0;
		int value2 = 100.0;
		if(info.size()<2)
			return value;
		for(int i=2; i<info.size(); i++)
		{
			string akt = info[i];
			for(auto& it1 : linie[akt])
			{
				for(int j=0; j<it1.size(); j++)
				{
					if(it1[j]==info[0])
						value = min(value, j);
				}
			}
		}
		for(int i=2; i<info.size(); i++)
		{
			string akt = info[i];
			for(auto& it1 : linie[akt])
			{
				for(int j=0; j<it1.size(); j++)
				{
					if(it1[j]==info[1])
						value2 = min(value2, (int)it1.size()-j);
				}
			}
		}
		return (double)value+(double)value2/100.0;
	}
	int calculateStopKolejnosc (string stop, string line)
	{
		for(int i=0; i<linie[line].size(); i++)
		{
			for(int j=0; j<linie[line][i].size(); j++)
			{
				if(linie[line][i][j]==stop)
				{
					return i*200+j;
				}
			}
		}
		return 20000;
	}

	void analizujRelacje(string about, set <pair<int, kurs*> > scz)
	{
		stringstream hash;
		stringstream about2;
		about2<<about;
		vector <string> foo;
		char data[10000];
		while(about2.getline(data, 10000, '#'))
		{
			string data2 = data;
			foo.push_back(data2);
		}
		for(int i=2; i<foo.size(); i++)
		{
			hash<<foo[i];
		}
		set<string> mhs;



		for(auto& it2 : scz)
		{
			stringstream tmp5;
			tmp5<<it2.second;
			mhs.insert(tmp5.str());
		}
		for(auto& it2 : mhs)
			hash<<it2;

		string real_hash = md5(hash.str());

		vector <pair <int, string> > kursy2;
		for(auto& it2 : scz)
		{
			kursy2.push_back(make_pair(it2.second->postoje[it2.first].time, it2.second->linia));
			//cout<<godz(it2.second->postoje[it2.first].time)<<"/"<<it2.second->linia<<"/"<<it2.second->dni<<" ";
		}
		sort(kursy2.begin(), kursy2.end());
		vector <int> kursy3;
		for(int i=1; i<kursy2.size(); i++)
		{
			kursy3.push_back((kursy2[i].first-kursy2[i-1].first)/60);
		}
		vector <int> bledix;
		for(int i=2; i<((int)kursy3.size()-3); i++)
		{
			//cout<<"@"<<i<<" "<<kursy3.size()<<" "<<(i<(kursy3.size()-3))<<endl;
			double wyn = 0;
			for(int j=(i-2); j<(i+3); j++)
			{
				wyn+=kursy3[j];
			}
			//cout<<"*"<<i<<endl;
			wyn/=5.0;
			double wyn2 = kursy3[i];
			if(wyn2>(wyn*1.5) && wyn2>=6.0)
			{
				bledix.push_back(i);
			}
		}
		bool zuo = false;
		bool tab[kursy3.size()];
		for(int i=0; i<kursy3.size(); i++)
		{
			tab[i]=false;
		}
		for(int i=1; i<bledix.size(); i++)
		{
			if(bledix[i]-bledix[i-1]<=5)
			{
				for(int j=max(0, bledix[i-1]-2); j<min((int)kursy3.size(), bledix[i]+3); j++)
				{
					tab[j] = true;
				}
				zuo = true;
			}
		}
		if(foo.size()<4)
			zuo = false;
		if(true)
		{
			stringstream frt;
			frt<<foo[0]<<"	"<<foo[1]<<"	";
			for(int i=2; i<foo.size(); i++)
			{
				if(i>2)
					frt<<", ";
				frt<<foo[i];
				lineInfo[foo[i]].insert(make_pair<>(real_hash, calculateStopKolejnosc(foo[0], foo[i])));
			}
			frt<<"	";
			//frt<<md5(hash.str())<<"</td><td>";
			for(int i=0; i<kursy3.size(); i++)
			{
				if(i==0)
				{
					frt<<"<span title=\""<<godz(kursy2[i].first)<<" / "<<kursy2[i].second<<"\">-</span>";
				}
				if(tab[i])
					frt<<"<b>";
				frt<<kursy3[i];
				if(tab[i])
					frt<<"</b>";
				frt<<"<span title=\""<<godz(kursy2[i+1].first)<<" / "<<kursy2[i+1].second<<"\">-</span>";
			}
			double akt_level = calculateLevel(foo);
			if(hashLevels.find(real_hash)==hashLevels.end() || hashLevels[real_hash]>akt_level)
			{
				hashLevels[real_hash]=akt_level;
				hashOut[real_hash]=frt.str();
			}
		}
	}
	bool eq(set<string> a, set<string> b)
	{
		if(a.size()!=b.size())
			return false;
		for(auto& it1 : a)
		{
			if(b.find(it1)==b.end())
			{
				return false;
			}
		}
		return true;
	}

	void przegladajKurs (kurs* k)
	{
		if(k->dni!="DP")
			return;
		for(int i=0; i<k->postoje.size()-1; i++)
		{
			if(mapa7.find(k->postoje[i].stop_id)!=mapa7.end())
			{
				auto& it1 = mapa7[k->postoje[i].stop_id];

				for(int g=i+1; g<k->postoje.size(); g++)
				{
					if(it1.find(k->postoje[g].stop_id)!=it1.end())
					{
						auto& it2 = it1[k->postoje[g].stop_id];
						if(it2.find(k->linia)!=it2.end())
						{
							string txt = k->postoje[i].stop_id;
							txt += "#"+k->postoje[g].stop_id+"#";
							for(auto& it3 : it2)
								txt+=it3+"#";
							mapa2[txt].insert(make_pair(i, k));
						}
					}
				}
			}
			/*if(mapa.find(p)!=mapa.end() && mapa[p].find(k->linia)!=mapa[p].end())
			{
				string txt = k->postoje[i].stop_id+"#"+k->postoje[i+1].stop_id+"#";
				for(auto& it1 : mapa[p])
				{
					txt+=it1+"#";
				}
				if(mapa[p].size()>1)
				{
					bool ok = true;
					if(i>0)
					{
						pair<string, string> pBis = make_pair(k->postoje[i-1].stop_id, k->postoje[i].stop_id);
						if(mapa.find(pBis)!=mapa.end())
						{
							if(eq(mapa[pBis], mapa[p]))
								ok=false;
						}
					}
					if(ok)
						mapa2[txt].insert(make_pair(i, k));
				}
			}*/
		}
	}

	virtual void nowa_linia(string nazwa, vector <vector <string> > trasy)
	{
		//if(nazwa.size()<=2 && nazwa[0]>='0' && nazwa[0]<='9')
		//{
			linie[nazwa] = trasy;
			for(int j=0; j<trasy.size(); j++)
			{
				auto& it1 = trasy[j];
				for(int i=0; i<it1.size()-1; i++)
				{
					stringstream nazwa2;
					nazwa2<<nazwa<<"$"<<j;
					mapa[make_pair(it1[i], it1[i+1])].insert(nazwa2.str());
				}
			}
		//}
	}
	virtual void nowy_przystanek(przystanek nowy)
	{
		przystanki[nowy.id]=nowy.name;
	}

	virtual void nowy_kurs(kurs nowy)
	{
		kurs* nowy2 = new kurs();
		nowy2->dni = nowy.dni;
		nowy2->id = nowy.id;
		nowy2->linia = nowy.linia;
		nowy2->postoje = nowy.postoje;
		kursy.push_back(nowy2);
	}

	CoordinationHandler(string sciezka)
	{
		ScheduleReaderWarszawa reader(sciezka, this);
		reader.run();
	}
	void run()
	{
		for(auto& it1 : linie)
		{
			string linia = it1.first;
			for(auto& it2 : it1.second)
			{
				vector <string> pdd;
				if(it2.size()>0)
				{
					pdd.push_back(it2[0]);

				}
				pair<string, string> g;
				for(int i=0; i<(it2.size()-1); i++)
				{
					pair<string, string> p = make_pair(it2[i], it2[i+1]);
					if(i>0)
					{
						//if(!eq(mapa[g], mapa[p]))
						//{
							pdd.push_back(it2[i]);
						//}
					}
					g=p;
				}
				if(it2.size()>0)
				{
					pdd.push_back(it2[it2.size()-1]);
				}
				for(int i=0; i<pdd.size(); i++)
				{
					for(int j=i+1; j<pdd.size(); j++)
					{
						//cout<<linia<<" "<<przystanki[pdd[i]]<<" "<<przystanki[pdd[j]]<<endl;
						mapa7[pdd[i]][pdd[j]].insert(linia);
					}
				}
			}
		}

		for(int i=0; i<kursy.size(); i++)
		{
			przegladajKurs(kursy[i]);
		}

	}

	~CoordinationHandler()
	{
		for(int i=0; i<kursy.size(); i++)
		{
			delete kursy[i];
		}
	}
};

int main(int argc, char** argv)
{
	string str = argv[1];
	fstream plik1(argv[2], ios::out | ios::trunc);
	fstream plik2(argv[3], ios::out | ios::trunc);
	fstream plik3(argv[4], ios::out | ios::trunc);
	CoordinationHandler hand(str);
	hand.run();
	for(auto& it1 : hand.mapa2)
	{
		hand.analizujRelacje(it1.first, it1.second);
	}
	for(auto& it1 :hand.hashOut)
	{
		plik1<<it1.first<<"	"<<it1.second<<endl;
	}
	for(auto& it1 : hand.lineInfo)
	{
		map <string, int> linePrim;
		for(auto& it2 : it1.second)
		{
			if(linePrim.find(it2.first)==linePrim.end() || linePrim[it2.first]>it2.second)
			{
				linePrim[it2.first]=it2.second;
			}

		}
		for(auto& it2 : linePrim)
		{
			plik2<<it1.first<<"	"<<it2.first<<"	"<<it2.second<<endl;
		}
	}
	for(auto& it1 : hand.przystanki)
	{
		plik3<<it1.first<<"	"<<it1.second<<endl;
	}
}
