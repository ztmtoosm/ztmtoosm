#include "osm_base.hpp"
#include "starelinie.hpp"
int main(int argc, char** argv)
{
	string a1=argv[1];
	osm_base baza(a1);
	set <long long> ttt=wszystkie_route(&baza, 3651336);
	auto it1=baza.relations.begin();
	while(it1!=baza.relations.end())
	{
		map <string, string> tagi = (it1->second).tags;
		if(tagi["type"]=="route")
		{
			if(tagi["route"]=="bus" || tagi["route"]=="tram")
			{
				long long id=(it1->second).id;
				if(ttt.find(id)==ttt.end())
				{
					cout<<(it1->second).id<<endl;
					auto it2=tagi.begin();
					while(it2!=tagi.end())
					{
						cout<<it2->first<<" : "<<it2->second<<endl;
						it2++;
					}
					cout<<"----"<<endl;
				}
			}
		}
		it1++;
	}
	cout<<"XXX "<<ttt.size()<<endl;
}
