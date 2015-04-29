#ifndef ZTMOSMPOR
#define ZTMOSMPOR
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <fstream>
#include "../osmbase/osm_base.hpp"
#include "../ScheduleReader/ScheduleReader.hpp"
using namespace std;
struct OsmStopData
{
	string name;
	long long stop_position;
	long long bus_stop;
	long long platform;
	char platform_type;
	OsmStopData();
};

map<string, OsmStopData> loadOsmStopData(osm_base* baza);

class ztmread_for_html : public ScheduleHandler
{
	public:
	map<string, przystanek> przystanki;
	map <string, vector< vector<string> > > dane_linia;
	ztmread_for_html (string sciez);
	void nowy_przystanek(przystanek nowy);
	void nowa_linia(string nazwa, vector <vector <string> > trasy);
};


bool todigits(int nasze, string nastepne);

#endif
