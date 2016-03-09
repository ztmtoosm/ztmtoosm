#ifndef PRZEGLADNIECZYPRAWIDLOWESTARELINIE_HPP
#define PRZEGLADNIECZYPRAWIDLOWESTARELINIE_HPP
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include "HTMLGenerator.hpp"
#include "RouteCohesion.hpp"
#include "osm_base.hpp"
#include "stringspecial.hpp"
#include "ztmosmpor.hpp"
#include "OsmStopData.hpp"
using namespace std;
pair <long long, vector <long long> > relacje_linia(osm_base* roo, long long root, string linia);

set <string> linieDoUsuniecia(set <string>& istniejace, osm_base* roo, long long root);

set <string> linieDoUsuniecia(ScheduleHandlerInternal* baza_ztm, osm_base* roo, long long root);

class PrzegladanieCzyPrawidloweStareLinie
{
	string ref_key;
	long long rootRel;
	set <string> oldRelationStops(string linia, osm_base* baza);
	set <string> newRelationStops(string linia, ScheduleHandlerInternal* baza);
	void zbadajLinie(string linia, osm_base* bazaOsm, ScheduleHandlerInternal* bazaZtm);
	public:
	set <string> prawidlowe;
	set <string> nieprawidlowe;
	set <long long> badRelations;
	set <string> badLines;
	map <string, set<string> > onlyOsmStop;
	map <string, set<string> > onlyZtmStop;
	map <string, pair <long long, vector <long long> > > relacjeDlaLinii;
	PrzegladanieCzyPrawidloweStareLinie(osm_base* bazaOsm, ScheduleHandlerInternal* bazaZtm, set <string> doPrzerobienia, long long rootRelW, string ref_keyW);
};

class PrzegladanieCzyPrawidloweNoweLinie
{
	set <string> prawidlowe;
	map <string, set<string> > nieprawidlowe;
	ScheduleHandlerInternal* bazaZtm;
	map <string, OsmStopData>* osmStops;
	set <string> sprawdzLinie(string linia, vector <vector <string> > drugi);
	public:
	set <string> getPrawidlowe();
	set <string> getNieprawidlowe();
	map <string, set<string> > getNieprawidloweMap();
	PrzegladanieCzyPrawidloweNoweLinie(map<string, OsmStopData>* osmStopsW, ScheduleHandlerInternal* bazaZtmW, set<string> doPrzerobienia);
};
#endif
