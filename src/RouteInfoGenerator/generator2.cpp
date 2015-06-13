#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include "ztmosmpor.hpp"
#include "osm_base.hpp"
#include "HTMLHeadersRouteinfo.hpp"
#include "PrzegladanieCzyPrawidloweStareLinie.hpp"
using namespace std;
string nazwa_mala(string lineName)
{
	if(lineName.length()<3)
		return "tram";
	return "bus";
}
string nazwa_duza(string lineName)
{
	if(lineName.length()<3)
		return "Tram";
	return "Bus";
}
vector <string> miejscowosci(vector <przystanek> xyz)
{
	vector <string> wynik;
	string akt;
	for(int i=0; i<xyz.size(); i++)
	{
		if(xyz[i].miejscowosc!=akt)
		{
			wynik.push_back(xyz[i].miejscowosc);
			akt=xyz[i].miejscowosc;
		}
	}
	return wynik;
}

string znacznikLink(double lon, double lat)
{
	stringstream foo, foo2;
	foo<<"http://www.openstreetmap.org/?mlat=";
	foo<<lon;
	foo<<"&mlon=";
	foo<<lat;
	foo<<"&zoom=18";
	foo2<<"http://localhost:8111/load_and_zoom?left=";
	foo2<<lat-0.002;
	foo2<<"&right=";
	foo2<<lat+0.002;
	foo2<<"&top=";
	foo2<<lon+0.002;
	foo2<<"&bottom=";
	foo2<<lon-0.002;
	if(lon<1 || lat<1)
		return "";
	return htmlgen::div("plinki", "", (htmlgen::link(foo.str(), "X")+"</br>"+htmlgen::link(foo2.str(), "J")));
}

set <string> linieDoUsuniecia(set <string>& istniejace, osm_base* roo, long long root)
{
	set <string> wynik;
	relation akt=roo->relations[root];
	map <string, string> tags = akt.getTags();
	if(istniejace.find(tags["ref"])==istniejace.end())
	{
		if(tags["type"]=="route_master")
		{
			wynik.insert(tags["ref"]);
		}
	}
	int s1=akt.members.size();
	for(int i=0; i<s1; i++)
	{
		if(akt.members[i].member_type==RELATION)
		{
			long long teraz_id=akt.members[i].member_id;
			if(roo->relations.find(teraz_id)!=roo->relations.end())
			{
				relation teraz=roo->relations[teraz_id];
				auto nowe = linieDoUsuniecia(istniejace, roo, teraz_id);
				wynik.insert(nowe.begin(), nowe.end());
			}
		}
	}
	return wynik;
}

set <string> wszystkieLinie(ztmread_for_html* bazaZtm)
{
	set <string> wynik;
	auto it1=bazaZtm->dane_linia.begin();
	while(it1!=bazaZtm->dane_linia.end())
	{
		string kand=it1->first;
		if(kand[0]!='K')
		if(kand[0]!='S')
		if(kand[0]!='W')
		{
			wynik.insert(kand);
		}
		it1++;
	}
	return wynik;
}
string zeraWiodace (string jeden)
{
	string wynik;
	for(int i=0; i<(10-jeden.length()); i++)
	{
		wynik+="0";
	}
	wynik+=jeden;
	return wynik;
}
long long getParentRelation (string name)
{
	if(name.length()<=2)
		return 3651333;
	if(name[0]=='1' || name[0]=='2')
		return 3651331;
	if(name[0]=='3')
		return 3651332;
	if(name[0]=='4')
		return 3651328;
	if(name[0]=='5' || name[0]=='E')
		return 3651327;
	if(name[0]=='7' || name[0]=='8')
		return 3651329;
	if(name[0]=='N')
		return 3651326;
	return 4656333;
}

struct galk
{
	osm_base* bazaOsm;
	ztmread_for_html* bazaZtm;
	string ztmBasePath;
	string osmBasePath;
	string pathHTML;
	string ztmBaseFreshTime;
	set <string> linieDoPrzerobienia;
	map <string, OsmStopData> osmStopData;
	//map <string, vector <WariantTrasy> > warianty;
	set <long long> changeNodes;
	set <long long> changeWays;
	bool czyWszystkie;
	void readArg(char** argv)
	{
		ztmBasePath=argv[1];
		osmBasePath=argv[2];
		//outPath=argv[3];
		pathHTML=argv[4];
		string czyW=argv[5];
		if(czyW=="-all")
		{
			czyWszystkie=1;
		}
		else
		{
			czyWszystkie=0;
		}
	}
	void readInput()
	{
		if(czyWszystkie)
		{
			return;
		}
		cout<<"PODAJ LINIE"<<endl;
		char data[10000];
		cin.getline(data, 10000);
		stringstream linki1;
		linki1<<data;
		string linki2;
		while(linki1>>linki2)
		{
			linieDoPrzerobienia.insert(linki2);
		}

	}
	string divOsmLink(long long id, char t)
	{
		string type;
		if(t=='N')
			type="node";
		if(t=='W')
			type="way";
		if(t=='R')
			type="relaton";
		if(id==0)
			htmlgen::div("komorka", "", "-");
		stringstream foo;
		foo<<"<a href=\"http://openstreetmap.org/"<<type<<"/"<<id<<"\">"<<type<<" "<<id<<"</a>";
		return htmlgen::div("komorka", "", foo.str());
	}
	string divOsmRow(int arraySize, string* elem)
	{
		stringstream foo;
		for(int i=0; i<arraySize; i++)
		{
			foo<<elem[i];
		}
		return htmlgen::div("wiersz", "", foo.str());
	}
	string divOsmTable(vector <string> elem)
	{
		stringstream foo;
		for(int i=0; i<elem.size(); i++)
		{
			foo<<elem[i];
		}
		return htmlgen::div("tabela", "", foo.str());
	}
	string dokladnePrzystanki(string idPrzystanek, string idLinia, int idWariantu, int idKol)
	{
		string poprzedni = "POCZĄTKOWY";
		string kolejny = "KOŃCOWY";
		if(idKol>0)
		{
			string poprzedniId = bazaZtm->dane_linia[idLinia][idWariantu][idKol-1];
			poprzedni = bazaZtm->przystanki[poprzedniId].name;
		}
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+1)
		{
			string kolejnyId = bazaZtm->dane_linia[idLinia][idWariantu][idKol+1];
			kolejny = bazaZtm->przystanki[kolejnyId].name;
		}
		string ostatniId = bazaZtm->dane_linia[idLinia][idWariantu][bazaZtm->dane_linia[idLinia][idWariantu].size()-1];
		string ostatni = bazaZtm->przystanki[ostatniId].name;
		string info = idLinia + " -> "+ostatni;
		return info;
	}
	vector <string> przystanekKierunki(string p)
	{
		vector <string> wynik;
		for(auto& it2 : bazaZtm->dane_linia)
		{
			for(int i=0; i<it2.second.size(); i++)
			{
				for(int j=0; j<it2.second[i].size(); j++)
				{
					if(it2.second[i][j]==p)
						wynik.push_back(dokladnePrzystanki(p, it2.first, i, j));
				}
			}
		}
		if(wynik.size()<3)
		{
			for(int i=wynik.size(); i<3; i++)
				wynik.push_back("-");
		}
		return wynik;
	}
	galk(char** argv)
	{
		readArg(argv);
		readInput();
		bazaOsm = new osm_base(osmBasePath);
		osmStopData = loadOsmStopData(bazaOsm);
		bazaZtm = new ztmread_for_html (ztmBasePath);
		string n2=pathHTML+"/zbb.html";
		fstream plik5(n2.c_str(), ios::out | ios::trunc);
		plik5.precision(9);
		htmlHead(plik5);
		char buff[20];
		time_t now = time(NULL);
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
		string buff2=buff;
		plik5<<htmlgen::div("gentime", "", "Wygenerowano: "+buff2)<<endl;
		vector <string> tabela;
		for(auto& it1 : osmStopData)
		{
			if(bazaZtm->przystanki.find(it1.first)!=bazaZtm->przystanki.end())
			{
				vector <string> kierunki=przystanekKierunki(it1.first);
				string refDiv = htmlgen::div("komorka", "", it1.first);
				string refName = htmlgen::div("komorka", "", it1.second.name);
				string k1 = htmlgen::div("komorka", "", kierunki[0]);
				string k2 = htmlgen::div("komorka", "", kierunki[1]);
				string k3 = htmlgen::div("komorka", "", kierunki[2]);
				string row[] = {refDiv, refName, divOsmLink(it1.second.bus_stop, 'N'), divOsmLink(it1.second.stop_position, 'N'), divOsmLink(it1.second.platform, it1.second.platform_type), k1, k2, k3};
				tabela.push_back(divOsmRow(8, row));
			}
		}
		for(auto& it1 : bazaZtm->przystanki)
		{
			if(osmStopData.find(it1.first)==osmStopData.end())
			{
				vector <string> kierunki=przystanekKierunki(it1.first);
				string refDiv = htmlgen::div("komorka", "", it1.first);
				string refName = htmlgen::div("komorka", "", it1.second.name);
				string k1 = htmlgen::div("komorka", "", kierunki[0]);
				string k2 = htmlgen::div("komorka", "", kierunki[1]);
				string k3 = htmlgen::div("komorka", "", kierunki[2]);
				string row[] = {refDiv, refName, divOsmLink(0, 'N'), divOsmLink(0, 'N'), divOsmLink(0, 'N'), k1, k2, k3};
				tabela.push_back(divOsmRow(8, row));
			}
		}
		plik5<<divOsmTable(tabela);
		htmlTile(plik5);
		plik5.close();
	}
	~galk()
	{
		delete bazaOsm;
		delete bazaZtm;
	}
};
int main(int argc, char** argv)
{
	galk foo(argv);
}
