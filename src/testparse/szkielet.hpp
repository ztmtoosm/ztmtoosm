#include "dijkstra.hpp"
#include "osm_base.hpp"

struct PunktSzkieletowy
{
	map<long long, long long> wychodzace;
	void podmien(long long destination, long long new_way);
	long long getWay(long long destination);
};


class Szkielet
{
	private:
	map <long long, set <long long> > znajdzWierzcholkiDoSplitowania (vector <long long> posrednie);
	vector <long long> kolejneDrogi2 (vector <long long> posrednie);
	map <long long, PunktSzkieletowy> punkty;
	osm_base* baza;
	public:
	bool ok;
	Szkielet(osm_base* baza, Przelicznik* przel);
	vector <long long> kolejneDrogiFull (vector <long long> posrednie);
};
