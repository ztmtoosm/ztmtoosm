#include "osm_base.hpp"

struct PunktSzkieletowy
{
	//pary way-id, next node-id
	vector <pair<long long, long long> > wychodzace;
	vector <pair<long long, long long> > wchodzace;
	void wypisz (ostream& out)
	{
		for(auto it1 : wychodzace)
		{
			out<<it1.first<<" "<<it1.second<<endl;
		}
		out<<endl;
		for(auto it1 : wchodzace)
		{
			out<<it1.first<<" "<<it1.second<<endl;
		}
	}
	void podmien(long long destination, long long new_way)
	{
		for(int i=0; i<wychodzace.size(); i++)
		{
			if(wychodzace[i].second==destination)
				wychodzace[i].first=new_way;
		}
		for(int i=0; i<wchodzace.size(); i++)
		{
			if(wchodzace[i].second==destination)
				wchodzace[i].first=new_way;
		}
	}
	long long getWay(long long destination)
	{
		for(auto it1 : wychodzace)
			if(it1.second==destination)
				return it1.first;
		for(auto it1 : wchodzace)
			if(it1.second==destination)
				return it1.first;
		return 0;
	}
};

class Szkielet
{
	public:
	map <long long, PunktSzkieletowy> punkty;
	Szkielet(osm_base* baza)
	{
		for(auto& it1 : baza->ways)
		{
			long long id = it1.first;
			vector <long long> nodes = it1.second.nodes;
			for(int i=0; i<nodes.size()-1; i++)
			{
				punkty[nodes[i]].wychodzace.push_back(make_pair(id, nodes[i+1]));
				punkty[nodes[i+1]].wchodzace.push_back(make_pair(id, nodes[i]));
			}
		}
	}
	map <long long, set <long long> > kolejneDrogi (vector <long long> posrednie)
	{
		map <long long, set <long long> > wynik;
		bool ok = 1;
		long long aktWay = 0;
		long long punktStart = 0;
		for(int i=0; i<posrednie.size()-1 && ok; i++)
		{
			long long nextWay = punkty[posrednie[i]].getWay(posrednie[i+1]);
			if(nextWay == 0)
				ok = 0;
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
	vector <long long> kolejneDrogi2 (vector <long long> posrednie)
	{
		bool ok = 1;
		vector <long long> wynik;
		long long aktWay = 0;
		for(int i=0; i<posrednie.size()-1 && ok; i++)
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
	void wypisz(ostream& out)
	{
		for(auto& it1 : punkty)
		{
			out<<it1.first<<"------"<<endl;
			it1.second.wypisz(out);
		}
	}
};
