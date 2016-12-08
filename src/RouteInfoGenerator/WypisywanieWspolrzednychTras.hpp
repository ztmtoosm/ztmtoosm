#include <set>
#include <vector>
#include <algorithm>
#include "osm_base.hpp"

struct WspolrzedneTrasData {
    double lon;
    double lat;
    string refStart;
    string refEnd;
    int ordinalId;
    long long id; 
};

class WypisywanieWspolrzednychTrasHandler
{
public:
    virtual void onNewData(WspolrzedneTrasData data) {
    }
};

class WypisywanieWspolrzednychTras
{
	set <pair<string, string> > dodane;
	vector <long long> bfs(long long start, long long stop, map <long long, vector <long long> >& prepareBfs);
	void easyNodes(long long rel, osm_base* base, WypisywanieWspolrzednychTrasHandler* handler);

	public:
	WypisywanieWspolrzednychTras(vector <long long>& rels, osm_base* bazaOsm, WypisywanieWspolrzednychTrasHandler* handler)
	{
		for(auto& it1 : rels)
		{
			easyNodes(it1, bazaOsm, handler);
		}
	}
};
