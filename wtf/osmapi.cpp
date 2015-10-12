#include <iostream>
#include "src/osm_base.hpp"
#include "src/szkielet.hpp"
using namespace std;
int main()
{
	vector <long long> test2;
	long long test;
	while(cin>>test)
	{
		test2.push_back(test);
	}
	osm_base baza(test2, "api.openstreetmap.org/api/");
	Szkielet szk(&baza);
	szk.wypisz(cout);
	baza.wypisz("gowno.txt");
}
