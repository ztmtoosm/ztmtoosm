#include <set>
#include <vector>
#include <algorithm>
#include "osm_base.hpp"
class WypisywanieWspolrzednychTras
{
	set <pair<string, string> > dodane;
	vector <long long> bfs(long long start, long long stop, map <long long, vector <long long> >& prepareBfs);
	void easyNodes(long long rel, osm_base* base, fstream& plikOut);

	public:
	WypisywanieWspolrzednychTras(vector <long long>& rels, osm_base* bazaOsm, string sciezka_out)
	{
		fstream plik_out(sciezka_out.c_str(), ios::trunc | ios::out);
		plik_out.precision(9);
		for(auto& it1 : rels)
		{
			easyNodes(it1, bazaOsm, plik_out);
		}
		plik_out.close();
	}
};
