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
using namespace std;
pair <long long, vector <long long> > relacje_linia(osm_base* roo, long long root, string linia);
class PrzegladanieCzyPrawidloweStareLinie
{
	string ref_key;
	long long rootRel;
	set <string> oldRelationStops(string linia, osm_base* baza);
	set <string> newRelationStops(string linia, ztmread_for_html* baza);
	//string infoLinie(string linia, osm_base* bazaOsm, ztmread_for_html* bazaZtm);
	void zbadajLinie(string linia, osm_base* bazaOsm, ztmread_for_html* bazaZtm);
	public:
	set <string> prawidlowe;
	set <string> nieprawidlowe;
	set <long long> badRelations;
	set <string> badLines;
	map <string, set<string> > onlyOsmStop;
	map <string, set<string> > onlyZtmStop;
	map <string, pair <long long, vector <long long> > > relacjeDlaLinii;
	PrzegladanieCzyPrawidloweStareLinie(osm_base* bazaOsm, ztmread_for_html* bazaZtm, set <string> doPrzerobienia, long long rootRelW, string ref_keyW);
};
#endif
