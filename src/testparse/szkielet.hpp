#include "../RareToThickRoutes/dijkstra.hpp"
#include "../osmbase/osm_base.hpp"

struct PunktSzkieletowy
{
	map<long long, long long> wychodzace;
	void podmien(long long destination, long long new_way);
	long long getWay(long long destination);
};

class Szkielet
{
	public:
	bool ok;
	map <long long, PunktSzkieletowy> punkty;
	osm_base* baza;
	Szkielet(osm_base* baza, Przelicznik* przel);
	map <long long, set <long long> > kolejneDrogi (vector <long long> posrednie);
	vector <long long> kolejneDrogi2 (vector <long long> posrednie);
	vector <long long> kolejneDrogiFull (vector <long long> posrednie);
};
