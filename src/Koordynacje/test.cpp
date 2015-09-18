#include "conf.h"
#include "../ScheduleReader/ScheduleReader.hpp"
#include <map>
#include <vector>
using namespace std;

class CoordinationHandler : public ScheduleHandler
{
	public:
	map <pair<string, string>, set <string> > mapa;

	virtual void nowa_linia(string nazwa, vector <vector <string> > trasy)
	{
		for(auto& it1 : trasy)
		{
			for(int i=0; i<it1.size()-1; i++)
			{
				mapa[make_pair(it1[i], it1[i+1])].insert(nazwa);
			}
		}
	}
	CoordinationHandler(string sciezka)
	{
		ScheduleReaderWarszawa reader(sciezka, this);
		reader.run();
	}
};

int main(int argc, char** argv)
{
	string str = argv[1];
	CoordinationHandler hand(str);
	for(auto& it1 : hand.mapa)
	{
		cout<<it1.first.first<<" <-> "<<it1.first.second<<": ";
		for(auto& it2 : it1.second)
		{
			cout<<it2<<" ";
		}
		cout<<endl;
	}
}
