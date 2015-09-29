#include "szkielet.hpp"



void PunktSzkieletowy::podmien(long long destination, long long new_way)
{
	wychodzace[destination]=new_way;
}
long long PunktSzkieletowy::getWay(long long destination)
{
	if(wychodzace.find(destination)!=wychodzace.end())
	{
		return wychodzace[destination];
	}
	return 0;
}


Szkielet::Szkielet(osm_base* baza, Przelicznik* przel)
{
	ok=1;
	this->baza = baza;
	for(auto& it1 : baza->ways)
	{
		long long id = it1.first;
		auto tags = it1.second.getTags();
		double wartosc = przel->getPrzelicznikWagiDrog(0, tags);
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
map <long long, set <long long> > Szkielet::znajdzWierzcholkiDoSplitowania (vector <long long> posrednie)
{
	map <long long, set <long long> > wynik;
	long long aktWay = 0;
	long long punktStart = 0;
	for(int i=0; i<(signed int)(posrednie.size()-1) && ok; i++)
	{
		long long nextWay = punkty[posrednie[i]].getWay(posrednie[i+1]);
		if(nextWay == 0)
		{
			ok = 0;
		}
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
	if(aktWay != 0 )
	{
		wynik[aktWay].insert(punktStart);
		wynik[aktWay].insert(posrednie[posrednie.size()-1]);
	}
	return wynik;
}
vector <long long> Szkielet::kolejneDrogi2 (vector <long long> posrednie)
{
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
vector <long long> Szkielet::kolejneDrogiFull (vector <long long> posrednie)
{
	auto posr1 = znajdzWierzcholkiDoSplitowania(posrednie);
	for(auto& it1 : posr1)
	{
		auto posr2 = baza->rozdziel_way(it1.first, it1.second);
		for(int i=0; i<posr2.size(); i++)
		{
			long long id = posr2[i].first;
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
