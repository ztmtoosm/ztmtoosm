#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include "osm_base.hpp"
using namespace std;
using namespace rapidxml;

struct odcinek;

struct punkt
{
	odcinek* from;
	double akt_distance;
	long long osm_id;
	vector <odcinek*> drogi_start;
	vector <odcinek*> drogi_stop;
	punkt()
	{
		akt_distance=1000000;
	}
};

struct odcinek
{
	long long osm_id;
	double distance;
	punkt* start;
	punkt* stop;
};

struct dijkstra
{
	map <long long, double> permisions;
	map <long long, punkt*> punkty;
	set <punkt*> odwiedzone;
	set <pair<double, punkt*> > do_odwiedzenia;
	void usun_do_odwiedzenia(punkt* akt)
	{
		set <pair<double, punkt*> >::iterator it1=do_odwiedzenia.find(pair<double, punkt*>(akt->akt_distance, akt));
		if(it1!=do_odwiedzenia.end())
		{
			do_odwiedzenia.erase(it1);
		}
	}
	void load_permisions()
	{
		fstream plik;
		plik.open(".permisions");
		long long id;
		double per;
		while(plik>>id)
		{
			plik>>per;
			permisions[id]=per;
		}
	}
	void nowa_krawedz_zaznaczona(punkt* akt, odcinek* from, double distance)
	{
		if(odwiedzone.find(akt)!=odwiedzone.end())
			return;
		usun_do_odwiedzenia(akt);
		if(akt->akt_distance>distance)
		{
			akt->akt_distance=distance;
			akt->from=from;
			do_odwiedzenia.insert(pair<double, punkt*>(distance, akt));
		}
		else
		{
			do_odwiedzenia.insert(pair<double, punkt*>(akt->akt_distance, akt));
		}
	}
	void dijkstra_clear()
	{
		odwiedzone.clear();
		do_odwiedzenia.clear();
		map<long long, punkt*>::iterator it1=punkty.begin();
		while(it1!=punkty.end())
		{
			if(it1->second!=NULL)
			{
				it1->second->akt_distance=1000000;
				it1->second->from=NULL;
			}
			it1++;
		}
	}
	void oblicz(long long start_id, long long stop_id)
	{
		dijkstra_clear();
		punkt* destination=punkty[stop_id];
		nowa_krawedz_zaznaczona(punkty[start_id], NULL, 0);
		while(!do_odwiedzenia.empty())
		{
			set <pair<double, punkt*> >::iterator it1=do_odwiedzenia.begin();
			punkt* akt=it1->second;
			if(akt==destination)
				return;
			odwiedzone.insert(akt);
			usun_do_odwiedzenia(akt);
			int s1=akt->drogi_start.size();
			for(int i=0; i<s1; i++)
			{
				odcinek* teraz = akt->drogi_start[i];
				nowa_krawedz_zaznaczona(teraz->stop, teraz, akt->akt_distance+teraz->distance);
			}
		}
	}
	set<long long> eee3;
	void dodaj_nowe(long long id1, long long id2, long long id3, double dist)
	{
		eee3.insert(id1);
		eee3.insert(id2);
		if(punkty.find(id1)==punkty.end())
		{
			punkt* nowy= new punkt();
			nowy->osm_id=id1;
			punkty[id1]=nowy;
		}
		if(punkty.find(id2)==punkty.end())
		{
			punkt* nowy= new punkt();
			nowy->osm_id=id2;
			punkty[id2]=nowy;	
		}
		odcinek* nowa = new odcinek;
		nowa->start=punkty[id1];
		nowa->stop=punkty[id2];
		punkty[id1]->drogi_start.push_back(nowa);
		punkty[id2]->drogi_stop.push_back(nowa);
		nowa->osm_id=id3;
		nowa->distance=dist;
	}

	void laduj_dijkstra_from_file(string sciezka)
	{
		fstream plik;
		plik.open(sciezka.c_str());
		char data[5000];
		while(plik.getline(data, 5000))
		{
			stringstream dtt;
			dtt<<data;
			long long id1, id2, id3;
			double distance;
			dtt>>id1>>id2>>id3>>distance;
			dodaj_nowe(id1,id2,id3,distance);
		}
		plik.close();
	}

	virtual double getPrzelicznikWagiDrog(int id, map <string, string> akt_tags)
	{
		double przelicznik =-1;
		if(akt_tags.find("highway")!=akt_tags.end())
		{
			//przelicznik=4;
			if(akt_tags["highway"]=="motorway")
				przelicznik=0.8;
			if(akt_tags["highway"]=="motorway_link")
				przelicznik=1;
			if(akt_tags["highway"]=="trunk")
				przelicznik=0.8;
			if(akt_tags["highway"]=="trunk_link")
				przelicznik=1;
			if(akt_tags["highway"]=="primary")
				przelicznik=0.98;
			if(akt_tags["highway"]=="primary_link")
				przelicznik=0.98;
			if(akt_tags["highway"]=="secondary")
				przelicznik=0.98;
			if(akt_tags["highway"]=="secondary_link")
				przelicznik=0.98;
			if(akt_tags["highway"]=="tertiary")
				przelicznik=1;
			if(akt_tags["highway"]=="tertiary_link")
				przelicznik=1;
			if(akt_tags["highway"]=="residential")
				przelicznik=1.3;
			if(akt_tags["highway"]=="unclassified")
				przelicznik=1.3;
			if(akt_tags["highway"]=="service")
				przelicznik=1.3;
			if(akt_tags["highway"]=="living_street")
				przelicznik=1.3;
			if(akt_tags["routing:ztm"]=="yes")
				przelicznik=0.8;
			if(akt_tags["access"]=="private")
				przelicznik=0;
		}
		if(permisions.find(id)!=permisions.end())
			przelicznik=permisions[id];
		return przelicznik;
	}

	void laduj_dijkstra_from_base(osm_base* bazuka)
	{
		map<long long, way>::iterator it1=bazuka->ways.begin();
		while(it1!=bazuka->ways.end())
		{
			map <string, string> akt_tags=(it1->second).tags;
			bool way1=1;
			bool way2=1;
			if(akt_tags["oneway"]=="yes")
				way2=0;
			if(akt_tags["oneway"]=="-1")
				way1=0;
			if(akt_tags["highway"]=="motorway")
				way2=0;
			if(akt_tags["junction"]=="roundabout")
				way2=0;
			if(akt_tags["highway"]=="motorway_link")
				way2=0;
			if(akt_tags["oneway"]=="no")
				way2=1;
			double przelicznik=getPrzelicznikWagiDrog(it1->first, akt_tags);
			if(przelicznik>0)
			{
				vector <long long> akt_nodes=(it1->second).nodes;
				int s1=akt_nodes.size();
				for(int i=0; i<s1-1; i++)
				{
					double distance_abs=distance(bazuka->nodes[akt_nodes[i]].lon, bazuka->nodes[akt_nodes[i]].lat, bazuka->nodes[akt_nodes[i+1]].lon, bazuka->nodes[akt_nodes[i+1]].lat)*przelicznik;
					if(way1)
						dodaj_nowe(akt_nodes[i], akt_nodes[i+1], it1->first, distance_abs);
					if(way2)
						dodaj_nowe(akt_nodes[i+1], akt_nodes[i], it1->first, distance_abs);
				}
			}
			it1++;
		}
	}
	void dij_podmien(long long old, long long nju, long long start, long long stop)
	{
		punkt* st=punkty[start];
		if(st==NULL)
			return;
		int s1=st->drogi_start.size();
		for(int i=0; i<s1; i++)
		{
			long long f1=st->drogi_start[i]->start->osm_id;
			long long s1=st->drogi_start[i]->stop->osm_id;
			long long i1=st->drogi_start[i]->osm_id;
			if(f1==start && s1==stop && i1==old)
			{
				st->drogi_start[i]->osm_id=nju;
			}
		}
	}
};

struct DijkstraTram : dijkstra
{
	double getPrzelicznikWagiDrog(int id, map <string, string> akt_tags)
	{
		double przelicznik =-1;
		if(akt_tags.find("railway")!=akt_tags.end())
		{
			if(akt_tags["railway"]=="tram")
				przelicznik=1;
		}
		return przelicznik;
	}
};
struct DijkstraRail : dijkstra
{
	double getPrzelicznikWagiDrog(int id, map <string, string> akt_tags)
	{
		double przelicznik =-1;
		if(akt_tags.find("railway")!=akt_tags.end())
		{
			if(akt_tags["railway"]=="rail")
				przelicznik=1;
			if(akt_tags["railway"]=="subway")
				przelicznik=1;
			if(akt_tags["railway"]=="light_rail")
				przelicznik=1;
		}
		return przelicznik;
	}
};
#endif
