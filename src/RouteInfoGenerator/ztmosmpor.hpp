#ifndef ZTMOSMPOR
#define ZTMOSMPOR
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <fstream>
#include "../ScheduleReader/ScheduleReader.hpp"
#include "../ScheduleReader/ScheduleReader.hpp"
#include "../ScheduleReader/ScheduleReaderGdansk.hpp"
#include "../ScheduleReader/ScheduleReaderSzczecin.hpp"
#include "../ScheduleReader/ScheduleReaderLodz.hpp"
using namespace std;

class ScheduleHandlerInternal : public ScheduleHandler
{
	public:
	map<string, przystanek> przystanki;
	map <string, vector< vector<string> > > dane_linia;
	ScheduleHandlerInternal (string sciez, string type);
	void nowy_przystanek(przystanek nowy);
	void nowa_linia(string nazwa, vector <vector <string> > trasy);
};


bool todigits(int nasze, string nastepne);

#endif
