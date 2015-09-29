#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include "osm_base.hpp"
using namespace std;

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
	vector <double> distances;
	punkt* start;
	punkt* stop;
};


struct Przelicznik
{
	virtual double getPrzelicznikWagiDrog(int id, map <string, string> akt_tags);
};

struct PrzelicznikBus : Przelicznik
{
	double getPrzelicznikWagiDrog(int id, map <string, string> akt_tags);
};

struct PrzelicznikTram : Przelicznik
{
	double getPrzelicznikWagiDrog(int id, map <string, string> akt_tags);
};

struct dijkstra
{
	map <long long, double> permisions;
	map <long long, punkt*> punkty;
	set <punkt*> odwiedzone;
	set <pair<double, punkt*> > do_odwiedzenia;
	vector <Przelicznik*> przeliczniki;
	dijkstra(vector <Przelicznik*> przelicznikiW);
	void usun_do_odwiedzenia(punkt* akt);
	void load_permisions();
	void nowa_krawedz_zaznaczona(punkt* akt, odcinek* from, double distance);
	void dijkstra_clear();
	void oblicz(long long start_id, long long stop_id, int przelicznikId);
	void dodaj_nowe(long long id1, long long id2, long long id3, vector<double> dist);
	void laduj_dijkstra_from_file(string sciezka);
	void laduj_dijkstra_from_base(osm_base* bazuka);
	void dij_podmien(long long old, long long nju, long long start, long long stop);
};
#endif
