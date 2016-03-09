#include "osmbase/osm_base.hpp"
struct OsmStopData
{
	string name;
	long long stop_position;
	long long bus_stop;
	long long platform;
	char platform_type;
	OsmStopData();
};

map<string, OsmStopData> loadOsmStopData(osm_base* baza, string ref_key="ref");
