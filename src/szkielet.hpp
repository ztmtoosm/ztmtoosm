#include "osm_base.hpp"
#include "dijkstra.hpp"

struct PunktSzkieletowy
{
	map<long long, long long> wychodzace;
	void podmien(long long destination, long long new_way)
	{
		wychodzace[destination]=new_way;
	}
	long long getWay(long long destination)
	{
		if(wychodzace.find(destination)!=wychodzace.end())
		{
			return wychodzace[destination];
		}
		return 0;
	}
};

class Szkielet
{
	public:
	map <long long, PunktSzkieletowy> punkty;
	osm_base* baza;
	dijkstra dij;
	Szkielet(osm_base* baza)
	{
		this->baza = baza;
		for(auto& it1 : baza->ways)
		{
			long long id = it1.first;
			auto tags = it1.second.getTags();
			double wartosc = dij.getPrzelicznikWagiDrog(0, tags);
			vector <long long> nodes = it1.second.nodes;
			if(wartosc>0)
			{
				for(int i=0; i<(nodes.size()-1); i++)
				{
					punkty[nodes[i]].wychodzace[nodes[i+1]]=id;
					punkty[nodes[i+1]].wychodzace[nodes[i]]=id;
				}
			}
		}
	}
	map <long long, set <long long> > kolejneDrogi (vector <long long> posrednie)
	{
		cout<<"kolejneDrogi - start"<<endl;
		map <long long, set <long long> > wynik;
		bool ok = 1;
		long long aktWay = 0;
		long long punktStart = 0;
		for(int i=0; i<(signed int)(posrednie.size()-1) && ok; i++)
		{
			cout<<"kolejneDrogi - nextWay start"<<endl;
			long long nextWay = punkty[posrednie[i]].getWay(posrednie[i+1]);
			if(nextWay == 0)
				ok = 0;
			cout<<"kolejneDrogi - nextWay stop"<<endl;
			if(nextWay != aktWay)
			{
				if(aktWay != 0 )
				{
					wynik[aktWay].insert(punktStart);
					wynik[aktWay].insert(posrednie[i]);
				}
				punktStart = posrednie[i];
				aktWay = nextWay;
			}
		}
		cout<<"kolejneDrogi - pętla stop"<<endl;
		if(aktWay != 0 )
		{
			wynik[aktWay].insert(punktStart);
			wynik[aktWay].insert(posrednie[posrednie.size()-1]);
		}
		cout<<"kolejneDrogi - THE END"<<endl;
		return wynik;
	}
	vector <long long> kolejneDrogi2 (vector <long long> posrednie)
	{
		bool ok = 1;
		vector <long long> wynik;
		long long aktWay = 0;
		for(int i=0; i<(signed int)(posrednie.size()-1) && ok; i++)
		{
			long long nextWay = punkty[posrednie[i]].getWay(posrednie[i+1]);
			if(nextWay == 0)
				ok = 0;
			if(nextWay != aktWay)
			{
				if(aktWay != 0)
				{
					wynik.push_back(aktWay);
				}
				aktWay = nextWay;
			}
		}
		if(aktWay != 0 )
		{
			wynik.push_back(aktWay);
		}
		return wynik;
	}
	vector <long long> kolejneDrogiFull (vector <long long> posrednie)
	{
		auto posr1 = kolejneDrogi(posrednie);
		for(auto& it1 : posr1)
		{
			for(auto& it2 : it1.second)
			{
				cout<<it2<<" xD ";
			}
			cout<<endl;
			auto posr2 = baza->rozdziel_way(it1.first, it1.second);
			for(int i=0; i<posr2.size(); i++)
			{
				long long id = posr2[i].first;
				cout<<"ROZDZIEL ID "<<id<<endl;
				for(int j=0; j<(signed int)(posr2[i].second.size()-1); j++)
				{
					long long f1 = posr2[i].second[j];
					long long f2 = posr2[i].second[j+1];
					punkty[f1].podmien(f2, id);
					punkty[f2].podmien(f1, id);
				}
			}
			cout<<"$$$$"<<endl;
		}
		return kolejneDrogi2(posrednie);
	}
};
