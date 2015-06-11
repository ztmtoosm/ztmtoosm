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
	set <string> oldRelationStops(string linia, osm_base* baza);
	set <string> newRelationStops(string linia, ztmread_for_html* baza);
	string infoLinie(string linia, osm_base* bazaOsm, ztmread_for_html* bazaZtm);
	void printRoznice(string linia, osm_base* bazaOsm, ztmread_for_html* bazaZtm, map<string, string>* infoHTML);
	set <string> doPrzerobienia;
	public:
	set <string> prawidlowe;
	set <string> nieprawidlowe;
	PrzegladanieCzyPrawidloweStareLinie(osm_base* bazaOsm, ztmread_for_html* bazaZtm, set <string> doPrzerobieniaW, map<string, string>* infoHTML);
};
#endif
