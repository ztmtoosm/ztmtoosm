#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include "src/stringspecial.hpp"
#include "src/ztmosmpor.hpp"
#include "src/osm_base.hpp"
#include "src/dijkstra.hpp"
#include "src/dij_data.hpp"
#include "src/htmlgen.hpp"
#include "src/starelinie.hpp"
using namespace std;

set <long long> pustyGen()
{
	set <long long> pusty;
	pusty.insert(3651331);
	pusty.insert(3651332);
	pusty.insert(3651333);
	pusty.insert(3651328);
	pusty.insert(3651327);
	pusty.insert(3651329);
	pusty.insert(3651326);
	pusty.insert(3651335);
	pusty.insert(3651336);
	return pusty;
}

set <long long> merge(vector <dijkstra* > dij)
{
	set <long long> wynik;
	for(int i=0; i<dij.size(); i++)
	{
		wynik.insert(dij[i]->eee3.begin(), dij[i]->eee3.end());
	}
	return wynik;
}



struct WariantTrasy
{
	dijkstra* dij;
	string nazwa;
	int wariantId;
	vector <long long> stopPositions;
	vector <long long> stopSigns;
	vector <long long> stopPlatforms;
	osm_base* bazaOsm;
	ztmread_for_html* bazaZtm;
	bool blad;
	void init1()
	{
		blad=0;
		vector <przystanek_big> przystanki=bazaZtm->dane_linia[nazwa][wariantId];
		for(int i=0; i<przystanki.size(); i++)
		{
			stopPlatforms.push_back(przystanki[i].platform);
			stopPositions.push_back(przystanki[i].stop_position);
			if(przystanki[i].bus_stop!=0)
			{
				stopSigns.push_back(przystanki[i].bus_stop);
			}
			else
			{
				stopSigns.push_back(przystanki[i].stop_position);
			}
		}
	}
	void init2()
	{
		dij_data d0(stopPositions, dij);
		if(d0.ok)
		{
			map <long long, set<long long> >::iterator it1=d0.split_data.begin();
			while(it1!=d0.split_data.end())
			{
				vector <pair<long long, vector <long long> > > rozd=bazaOsm->rozdziel_way(it1->first, it1->second);
				for(int i=0; i<rozd.size(); i++)
				{
					for(int j=0; j<rozd[i].second.size()-1; j++)
					{
						dij->dij_podmien(it1->first, rozd[i].first, rozd[i].second[j], rozd[i].second[j+1]);
						dij->dij_podmien(it1->first, rozd[i].first, rozd[i].second[j+1], rozd[i].second[j]);
					}
				}
				it1++;
			}
		}
		else
		{
			blad=1;
			cout<<"BŁĄD - LINIA "<<nazwa<<endl;
		}
	}
	vector <string> miejscowosci(vector <przystanek_big> xyz)
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
	void generateRelationWithoutIdVersion(set <long long>& changeNodes, set <long long>& changeWays, relation& rel)
	{
		string typ_maly=nazwa_mala(nazwa);
		string typ_duzy=nazwa_duza(nazwa);
		map <string, string> tags;
		rel.modify=1;
		tags["network"]="ZTM Warszawa";
		tags["type"]="route";
		tags["route"]=typ_maly;
		tags["ref"]=nazwa;
		map <string, vector< vector<przystanek_big> > >::iterator it1=bazaZtm->dane_linia.find(nazwa);
		vector <string> miasta = miejscowosci((it1->second)[wariantId]);
		for(int i=0; i<miasta.size(); i++)
		{
			cout<<miasta[i]<<" ";
		}
		cout<<endl;
		string from = substituteWhiteCharsBySpace((it1->second)[wariantId][0].name_osm);
		string to = substituteWhiteCharsBySpace((it1->second)[wariantId][(it1->second)[wariantId].size()-1].name_osm);
		if(miasta.size()>1)
		{
			if(miasta[0]!="Warszawa")
				from += ", "+miasta[0];
			if(miasta[miasta.size()-1]!="Warszawa")
				to += ", "+miasta[miasta.size()-1];
		}
		string via;
		for(int i=1; i<(miasta.size()-1); i++)
		{
			via+=miasta[i];
			if(i<(miasta.size()-2))
				via+=", ";
		}
		tags["name"]=typ_duzy+" "+nazwa+": "+from+" => "+to;
		tags["from"] = from;
		tags["to"] = to;
		if(via!="")
		{
			tags["via"] = via;
		}
		/*
		rel.tags["note"]="stan na "+today;
		*/
		tags["source"]="Rozkład jazdy ZTM Warszawa, trasa wygenerowana przez bot";
		dij_data d1(stopPositions, dij);
		vector <long long> wszystkieDrogi=d1.all_ways;
		vector <long long> wszystkieWierzcholki=d1.all_nodes;
		for(int i=0; i<wszystkieWierzcholki.size(); i++)
			changeNodes.insert(wszystkieWierzcholki[i]);
		for(int i=0; i<wszystkieDrogi.size(); i++)
		{
			relation_member foo;
			foo.member_type=WAY;
			foo.member_id=wszystkieDrogi[i];
			changeWays.insert(wszystkieDrogi[i]);
			foo.role="";
			rel.members.push_back(foo);
		}
		for(int i=0; i<stopSigns.size(); i++)
		{
			string role="stop";
			if(i==0)
				role="stop_entry_only";
			if(i==stopSigns.size()-1)
				role="stop_exit_only";
			string rolep="platform";
			if(i==0)
				rolep="platform_entry_only";
			if(i==stopSigns.size()-1)
				rolep="platform_exit_only";
			relation_member foo;
			foo.member_type=NODE;
			foo.member_id=stopSigns[i];
			foo.role=role;
			rel.members.push_back(foo);
			if(stopPlatforms[i]>0)
			{
				changeWays.insert(stopPlatforms[i]);
				relation_member foop;
				foop.member_type=WAY;
				foop.member_id=stopPlatforms[i];
				foop.role=rolep;
				rel.members.push_back(foop);
			}
		}
		rel.setTags(tags);
	}
	void generateGPX(ostream& plik5)
	{
		dij_data d1(stopPositions, dij);
		vector <long long> wszystkieWierzcholki=d1.all_nodes;
		plik5<<"<trk>"<<endl;
		map <string, vector< vector<przystanek_big> > >::iterator it1=bazaZtm->dane_linia.find(nazwa);
		
		plik5<<"<name>"<<nazwa<<" "<<substituteWhiteCharsBySpace((it1->second)[wariantId][0].name_osm)+" => "+substituteWhiteCharsBySpace((it1->second)[wariantId][(it1->second)[wariantId].size()-1].name_osm)<<"</name>";
		plik5<<"<trkseg>"<<endl;
		for(int i=0; i<wszystkieWierzcholki.size(); i++)
		{
			plik5<<"<trkpt lat=\""<<bazaOsm->nodes[wszystkieWierzcholki[i]].lat<<"\" lon=\""<<bazaOsm->nodes[wszystkieWierzcholki[i]].lon<<"\"></trkpt>"<<endl;
		}
		plik5<<"</trkseg>"<<endl;
		plik5<<"</trk>"<<endl;
	}
	WariantTrasy(ztmread_for_html* bazaZtmW, osm_base* bazaOsmW, dijkstra* dijW, string nazwaW, int wariantIdW)
	{
		bazaZtm=bazaZtmW;
		bazaOsm=bazaOsmW;
		dij=dijW;
		nazwa=nazwaW;
		wariantId=wariantIdW;
		init1();
		init2();	
	}
};
class PrzegladanieCzyPrawidloweNoweLinie
{
	set <string> doPrzerobienia;
	bool sprawdzLinie(string linia, vector <vector <przystanek_big> > drugi, map<string, string>* infoHTML)
	{
		string bledy;
		if(doPrzerobienia.find(linia)==doPrzerobienia.end())
			return false;
		int s9=drugi.size();
		bool ok=1;
		for(int i=0; i<s9; i++)
		{
			int s8=(drugi)[i].size();
			vector <long long> ids1;
			vector <long long> hig1;
			for(int j=0; j<s8; j++)
			{
				przystanek_big& data=(drugi)[i][j];
				if(data.stop_position==0)
				{
					bledy+=htmlgen::div("stop_pos_non", "", data.name+" "+data.id+" brak STOP_POSITION");
					ok=0;
				}
				else
				{
					if(data.pos_error)
					{
						bledy+=htmlgen::div("stop_pos_way", "", data.name+" "+data.id+" STOP_POSITION nie leży na drodze");
						ok=0;
					}
				}
			}
		}
		if(!ok)
		{
			bledy=htmlgen::div("bledy2", "", "Nie można wygenerować nowej trasy, ponieważ: "+bledy);
			(*infoHTML)[linia]+=bledy;
			return false;
		}
		return true;
	}
	public:
	set <string> prawidlowe;
	set <string> nieprawidlowe;
	PrzegladanieCzyPrawidloweNoweLinie(ztmread_for_html* bazaZtm, set <string> doPrzerobieniaW, map <string, string>* infoHTML)
	{
		doPrzerobienia=doPrzerobieniaW;
		set <string>::iterator it1=doPrzerobienia.begin();
		while(it1!=doPrzerobienia.end())
		{
			if(sprawdzLinie(*it1, bazaZtm->dane_linia[*it1], infoHTML))
			{
				prawidlowe.insert(*it1);
			}
			else
			{
				nieprawidlowe.insert(*it1);
			}
			it1++;
		}
	}
};
set <string> wszystkieLinie(ztmread_for_html* bazaZtm)
{
	set <string> wynik;
	auto it1=bazaZtm->dane_linia.begin();
	while(it1!=bazaZtm->dane_linia.end())
	{
		string kand=it1->first;
		if(kand[0]!='K')
		if(kand[0]!='S')
		if(kand[0]!='L')
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
struct galk
{
	osm_base* bazaOsm;
	ztmread_for_html* bazaZtm;
	string ztmBasePath;
	string osmBasePath;
	string outPath;
	string pathHTML;
	string ztmBaseFreshTime;
	set <string> linieDoPrzerobienia;
	vector <dijkstra*> algorytmy;
	map <string, vector <WariantTrasy> > warianty;
	bool czyWszystkie;
	void readArg(char** argv)
	{
		ztmBasePath=argv[1];
		osmBasePath=argv[2];
		outPath=argv[3];
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
	void loadAlgorithms()
	{
		dijkstra* dijBus = new dijkstra();
		dijBus->load_permisions();
		dijBus->laduj_dijkstra_from_base(bazaOsm);
		DijkstraTram* dijTram = new DijkstraTram();
		dijTram->laduj_dijkstra_from_base(bazaOsm);
		DijkstraRail* dijRail = new DijkstraRail();
		dijRail->laduj_dijkstra_from_base(bazaOsm);
		algorytmy.push_back(dijBus);
		algorytmy.push_back(dijTram);
		algorytmy.push_back(dijRail);
	}
	dijkstra* algorithmForLine(string lineName)
	{
		if(lineName=="WKD" || lineName[0]=='K' || lineName[0]=='S')
			return algorytmy[2];
		else
			if(lineName.length()<3)
				return algorytmy[1];
		return algorytmy[0];
	}

	set <long long> changeNodes;
	set <long long> changeWays;

	bool generujLinie(string nazwa, int linia_licznik)
	{
		cout<<"GENEROWANIE "<<nazwa<<endl;
		vector <long long> stareRelacje=relacje_linia(bazaOsm, 3651336, nazwa).second;
		long long stareId=relacje_linia(bazaOsm, 3651336, nazwa).first;
		vector <long long> noweRelacje;
		for(int i=0; i<warianty[nazwa].size(); i++)
		{
			if(warianty[nazwa][i].blad)
				return false;
		}
		string nazwaGPX=pathHTML+"/"+nazwa+".gpx";
		fstream plik5(nazwaGPX.c_str(), ios::out | ios::trunc);
		plik5<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?><gpx>"<<endl;
		plik5.precision(9);
		cout<<warianty[nazwa].size()<<" "<<stareRelacje.size()<<endl;
		for(int i=0; i<warianty[nazwa].size(); i++)
		{
			warianty[nazwa][i].generateGPX(plik5);
			relation nowa;
			warianty[nazwa][i].generateRelationWithoutIdVersion(changeNodes, changeWays, nowa);
			//GENERUJ ID I WERSJĘ
			if(i<stareRelacje.size())
			{
				nowa.id=stareRelacje[i];
				nowa.version=bazaOsm->relations[stareRelacje[i]].version;
			}
			else
			{
				nowa.id=(i*-1)-linia_licznik*100;
				nowa.version=-1;
			}
			noweRelacje.push_back(nowa.id);
			bazaOsm->relations[nowa.id]=nowa;
		}
		for(int i=warianty[nazwa].size(); i<stareRelacje.size(); i++)
		{
			bazaOsm->relations[stareRelacje[i]].todelete=true;
		}
		string typ_maly=nazwa_mala(nazwa);
		string typ_duzy=nazwa_duza(nazwa);
		relation rel;
		rel.modify=1;
		rel.id=(-50)-linia_licznik*100;
		if(stareId!=0)
			rel.id=stareId;
		map<string,string> tags;
		tags["network"]="ZTM Warszawa";
		tags["type"]="route_master";
		tags["route_master"]=typ_maly;
		tags["ref"]=nazwa;
		tags["url"]="http://ztm.waw.pl/rozklad_nowy.php?c=182&l=1&q="+nazwa;
		tags["source"]="Rozkład jazdy ZTM Warszawa, trasa wygenerowana przez bot";
		tags["name"]=typ_duzy+" "+nazwa;
		for(int g=0; g<noweRelacje.size(); g++)
		{
				relation_member foo;
				foo.member_type=RELATION;
				foo.member_id=noweRelacje[g];
				foo.role="";
				rel.members.push_back(foo);
		}
		if(rel.id>0)
			rel.version=bazaOsm->relations[rel.id].version;
		rel.setTags(tags);
		bazaOsm->relations[rel.id]=rel;
		plik5<<"</gpx>"<<endl;
		plik5.close();
		return true;
	}
	map <string, string> infoHTML;
	string ok_route(string cos)
	{
		return htmlgen::div("ok_route", cos, "Trasa "+cos+" wygenerowana, pokaż...");
	}
	galk(char** argv)
	{
		readArg(argv);
		readInput();
		bazaOsm = new osm_base(osmBasePath);
		loadAlgorithms();
		bazaZtm = new ztmread_for_html (bazaOsm, ztmBasePath, merge(algorytmy));
		cout<<"alpha1"<<endl;
		if(czyWszystkie)
			linieDoPrzerobienia=wszystkieLinie(bazaZtm);
		PrzegladanieCzyPrawidloweStareLinie przegl0(bazaOsm, bazaZtm, linieDoPrzerobienia, &infoHTML);
		set <string> etap=przegl0.nieprawidlowe;
		if(!czyWszystkie)
			etap=linieDoPrzerobienia;
		PrzegladanieCzyPrawidloweNoweLinie przegl(bazaZtm, etap, &infoHTML);
		linieDoPrzerobienia=przegl.prawidlowe;
		set <string>::iterator it1=linieDoPrzerobienia.begin();
		while(it1!=linieDoPrzerobienia.end())
		{
			for(int i=0; i<bazaZtm->dane_linia[*it1].size(); i++)
			{
				warianty[*it1].push_back(WariantTrasy(bazaZtm, bazaOsm, algorithmForLine(*it1), *it1, i));
			}
			it1++;
		}
		map <string, vector<WariantTrasy> >::iterator it2=warianty.begin();
		int licznik=1000;
		string n2=pathHTML+"/openlayers.html";
		fstream plik5(n2.c_str(), ios::out | ios::trunc);
		plik5.precision(9);
		htmlHead(plik5);
		char buff[20];
		time_t now = time(NULL);
		strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
		string buff2=buff;
		plik5<<htmlgen::div("gentime", "", "Wygenerowano: "+buff2)<<endl;
		map <string, string> slownik0;
		map <string, pair<string, int> > slownikX;
		while(it2!=warianty.end())
		{
			if(!generujLinie(it2->first, licznik))
			{
				warianty.erase(it2);
			}
			else
			{
				infoHTML[it2->first]+=ok_route(it2->first);
				slownik0[zeraWiodace(it2->first)]=it2->first;
				slownikX[zeraWiodace(it2->first)]=make_pair(it2->first, 1);
			}
			it2++;
			licznik++;
		}
		map <string, string> slownik1;
		auto it3prim=przegl.nieprawidlowe.begin();
		while(it3prim!=przegl.nieprawidlowe.end())
		{
			slownik1[zeraWiodace(*it3prim)]=*it3prim;
			slownikX[zeraWiodace(*it3prim)]=make_pair(*it3prim, 2);
			it3prim++;
		}
		map <string, string> slownik2;
		auto it4prim=przegl0.prawidlowe.begin();
		while(it4prim!=przegl0.prawidlowe.end())
		{
			slownik2[zeraWiodace(*it4prim)]=*it4prim;
			slownikX[zeraWiodace(*it4prim)]=make_pair(*it4prim, 3);
			it4prim++;
		}
		string divX;
		auto i1prim = slownikX.begin();
		while(i1prim != slownikX.end())
		{
			string znak="tryb_niebieski";
			if(i1prim->second.second==1)
				znak="tryb_zielony";
			if(i1prim->second.second==2)
				znak="tryb_czerwony";
			divX+=htmlgen::div(znak, "", htmlgen::link( "#poczatek"+i1prim->second.first,  i1prim->second.first));
			i1prim++;
		}
		plik5<<htmlgen::div("spist", "", divX)<<endl;
		plik5<<htmlgen::div("partx", "", "Trasy wygenerowane...")<<endl;
		auto it0prim=slownik0.begin();
		while(it0prim!=slownik0.end())
		{
			plik5<<htmlgen::div("linia_green", "", infoHTML[it0prim->second])<<endl;
			it0prim++;
		}
		plik5<<htmlgen::div("partx", "", "Trasy niewygenerowane...")<<endl;
		
		auto it3=slownik1.begin();
		while(it3!=slownik1.end())
		{
			plik5<<htmlgen::div("linia_red", "", infoHTML[it3->second])<<endl;
			it3++;
		}
		if(czyWszystkie)
		{
			plik5<<htmlgen::div("partx", "", "Trasy bez zmian...")<<endl;
			auto it4=slownik2.begin();
			while(it4!=slownik2.end())
			{
				plik5<<htmlgen::div("linia_blue", "", infoHTML[it4->second])<<endl;
				it4++;
			}
		}
		htmlTile(plik5);
		plik5.close();
		set <long long> pusty=pustyGen();
		osm_base bazuka3=bazaOsm->wybrane(changeNodes, changeWays, pusty);
		osm_base bazuka4=bazaOsm->modified();
		osm_base bazuka2=osm_base(bazuka4, bazuka3);
		bazuka2.wypisz(outPath);
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
