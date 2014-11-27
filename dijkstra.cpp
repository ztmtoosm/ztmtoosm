#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include "stringspecial.hpp"
#include "ztmosmpor.hpp"
#include "osm_base.hpp"
#include "dijkstra.hpp"
using namespace std;
using namespace rapidxml;
struct way_part_data
{
	long long way_id;
	long long from;
	long long to;
};
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
struct dij_data
{
	bool ok;
	vector <long long> all_nodes;
	vector <long long> all_ways;
	map<long long, set<long long> > split_data;
	dij_data(vector <long long> stop_nodes, dijkstra* dij)
	{
		ok=1;
		vector <way_part_data> dtt;
		int s5=stop_nodes.size();
		for(int i=0; i<s5-1; i++)
		{
			if(ok)
			{
			dij->oblicz(stop_nodes[i], stop_nodes[i+1]);
			punkt* from = dij->punkty[stop_nodes[i]];
			punkt* destination = dij->punkty[stop_nodes[i+1]];
			if(from==NULL || destination==NULL)
			{
				ok=0;
			}
			else
			{
				vector <long long> partial_nodes;
				vector <long long> partial_ways;
				vector <way_part_data> dtt2;
				long long destination_id=destination->osm_id;
				partial_nodes.push_back(destination_id);
				while(destination!=from && destination!=NULL && destination->from!=NULL)
				{
					way_part_data foo;
					foo.to=destination->osm_id;
					foo.way_id=destination->from->osm_id;
					partial_ways.push_back(destination->from->osm_id);
					destination=destination->from->start;
					foo.from=destination->osm_id;
					destination_id=destination->osm_id;
					partial_nodes.push_back(destination_id);
					dtt2.push_back(foo);
				}
				for(int j=partial_nodes.size()-1; j>=0; j--)
				{
					if(all_nodes.size()==0 || all_nodes[all_nodes.size()-1]!=partial_nodes[j])
					{
						all_nodes.push_back(partial_nodes[j]);
					}
				}
				for(int j=partial_ways.size()-1; j>=0; j--)
				{
					if(all_ways.size()==0 || all_ways[all_ways.size()-1]!=partial_ways[j])
					{
						all_ways.push_back(partial_ways[j]);
					}
				}
				for(int j=dtt2.size()-1; j>=0; j--)
				{
					dtt.push_back(dtt2[j]);
				}
			}
			}
		}
		long long akt_way=-1;
		long long akt_from=-1;
		long long akt_to=-1;
		for(int i=0; i<dtt.size(); i++)
		{
			if(akt_way!=-1 && dtt[i].way_id!=akt_way)
			{
				split_data[akt_way].insert(akt_from);
				split_data[akt_way].insert(akt_to);
			}
			if(dtt[i].way_id!=akt_way)
			{
				akt_way=dtt[i].way_id;
				akt_from=dtt[i].from;
			}
			akt_to=dtt[i].to;
		}
		if(akt_way!=-1)
		{
			split_data[akt_way].insert(akt_from);
			split_data[akt_way].insert(akt_to);
		}
	}
};
string nazwa_skrocona(string nazwa)
{
	if(nazwa=="")
		return nazwa;
	string wynik;
	stringstream plt;
	plt<<nazwa;
	string tmp1;
	vector <string> tmp;
	while(plt>>tmp1)
		tmp.push_back(tmp1);
	for(int i=0; i<tmp.size()-1; i++)
	{
		wynik+=tmp[i];
		if(i<tmp.size()-2)
			wynik+=" ";
	}
	return wynik;
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
	osm_base* bazaOsm;
	ztmread_for_html* bazaZtm;
	bool blad;
	void init1()
	{
		blad=0;
		vector <przystanek_big> przystanki=bazaZtm->dane_linia[nazwa][wariantId];
		for(int i=0; i<przystanki.size(); i++)
		{
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
	relation generateRelationWithoutIdVersion(set <long long>& changeNodes, set <long long>& changeWays)
	{
		string typ_maly=nazwa_mala(nazwa);
		string typ_duzy=nazwa_duza(nazwa);
		relation rel;
		rel.modify=1;
		rel.tags["network"]="ZTM Warszawa";
		rel.tags["type"]="route";
		rel.tags["route"]=typ_maly;
		rel.tags["ref"]=nazwa;
		map <string, vector< vector<przystanek_big> > >::iterator it1=bazaZtm->dane_linia.find(nazwa);
		rel.tags["name"]=typ_duzy+" "+nazwa+": "+nazwa_skrocona((it1->second)[wariantId][0].name_osm)+" => "+nazwa_skrocona((it1->second)[wariantId][(it1->second)[wariantId].size()-1].name_osm);
		rel.tags["from"]=nazwa_skrocona((it1->second)[wariantId][0].name_osm);
		rel.tags["to"]=nazwa_skrocona((it1->second)[wariantId][(it1->second)[wariantId].size()-1].name_osm);
		/*
		rel.tags["note"]="stan na "+today;
		*/
		rel.tags["source"]="Rozkład jazdy ZTM Warszawa, trasa wygenerowana przez bot";
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
			relation_member foo;
			foo.member_type=NODE;
			foo.member_id=stopSigns[i];
			foo.role=role;
			rel.members.push_back(foo);
		}
		return rel;
	}
	void generateGPX(ostream& plik5)
	{
		dij_data d1(stopPositions, dij);
		vector <long long> wszystkieWierzcholki=d1.all_nodes;
		plik5<<"<trk>"<<endl;
		plik5<<"<name>"<<nazwa<<" wariant "<<wariantId<<"</name>"<<endl;
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
class PrzegladanieCzyPrawidloweLinie
{
	set <string> doPrzerobienia;
	bool sprawdzLinie(string linia, vector <vector <przystanek_big> > drugi)
	{
		if(doPrzerobienia.find(linia)==doPrzerobienia.end())
			return false;
		int s9=drugi.size();
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
					cout<<"error: "<<data.name<<"-"<<data.id<<": "<<"BRAK STOP_POSITION"<<endl;
					return false;
				}
				else
				{
					if(data.pos_error)
					{
						cout<<"error: "<<data.name<<"-"<<data.id<<": "<<"STOP_POSITION nie leży na drodze"<<endl;
						return false;
					}
				}
			}
		}
		return true;
	}
	public:
	set <string> prawidlowe;
	PrzegladanieCzyPrawidloweLinie(ztmread_for_html* bazaZtm, set <string> doPrzerobieniaW)
	{
		doPrzerobienia=doPrzerobieniaW;
		set <string>::iterator it1=doPrzerobienia.begin();
		while(it1!=doPrzerobienia.end())
		{
			if(sprawdzLinie(*it1, bazaZtm->dane_linia[*it1]))
			{
				prawidlowe.insert(*it1);
			}
			it1++;
		}
	}
};
struct galk
{
	osm_base* bazaOsm;
	ztmread_for_html* bazaZtm;
	string ztmBasePath;
	string osmBasePath;
	string outPath;
	string ztmBaseFreshTime;
	set <string> linieDoPrzerobienia;
	vector <dijkstra*> algorytmy;
	map <string, vector <WariantTrasy> > warianty;
	void readInput(char** argv)
	{
		ztmBasePath=argv[1];
		osmBasePath=argv[2];
		outPath=argv[3];
		ztmBaseFreshTime=argv[4];
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

	bool generujLinie(string nazwa, int linia_licznik, ostream& plik5)
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
		for(int i=0; i<warianty[nazwa].size(); i++)
		{
			warianty[nazwa][i].generateGPX(plik5);
			relation nowa=warianty[nazwa][i].generateRelationWithoutIdVersion(changeNodes, changeWays);
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
		cout<<"GENEROWANIE2 "<<nazwa<<endl;
		string typ_maly=nazwa_mala(nazwa);
		string typ_duzy=nazwa_duza(nazwa);
		relation rel;
		rel.modify=1;
		rel.id=(-50)-linia_licznik*100;
		if(stareId!=0)
			rel.id=stareId;
		rel.tags["network"]="ZTM Warszawa";
		rel.tags["type"]="route_master";
		rel.tags["route_master"]=typ_maly;
		rel.tags["ref"]=nazwa;
		rel.tags["url"]="http://ztm.waw.pl/rozklad_nowy.php?c=182&l=1&q="+nazwa;
		rel.tags["source"]="Rozkład jazdy ZTM Warszawa, trasa wygenerowana przez bot";
		rel.tags["name"]=typ_duzy+" "+nazwa;
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
		bazaOsm->relations[rel.id]=rel;
		return true;
	}
	galk(char** argv)
	{
		readInput(argv);
		bazaOsm = new osm_base(osmBasePath);
		loadAlgorithms();
		bazaZtm = new ztmread_for_html (osmBasePath, ztmBasePath, merge(algorytmy));
		PrzegladanieCzyPrawidloweLinie przegl(bazaZtm, linieDoPrzerobienia);
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
		string gpxPath = outPath+".gpx";
		fstream plik5(gpxPath.c_str(), ios::out | ios::trunc);
		plik5.precision(9);
		int licznik=1000;
		plik5<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?><gpx>"<<endl;
		while(it2!=warianty.end())
		{
			if(!generujLinie(it2->first, licznik, plik5))
			{
				warianty.erase(it2);
			}
			it2++;
			licznik++;
		}
		plik5<<"</gpx>"<<endl;
		plik5.close();
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
		osm_base bazuka3=bazaOsm->wybrane(changeNodes, changeWays, pusty);
		osm_base bazuka4=bazaOsm->modified();
		osm_base bazuka2=osm_base(bazuka4, bazuka3);
		bazuka2.wypisz(outPath);
		it2=warianty.begin();
		cout<<"WYGENEROWANO LINIE: "<<endl;
		while(it2!=warianty.end())
		{
			cout<<it2->first<<endl;
			it2++;
		}
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
	/*set<long long> wyb_w;
	set<long long> wyb_n;
	set<long long> wyb_r;
	int s9=ztm.dane.size();
	map <string, vector< vector<przystanek_big> > >::iterator it1=ztm.dane_linia.begin();
	bool przerabiaj=1;
	int linia_licznik=0;
	// map<string, vector <vector <long long> > > ids;
	// map<string, vector <vector <long long> > > high_stops;
	while(it1!=ztm.dane_linia.end())
	{
		string akt_linia=it1->first;
		dijkstra* dij = &dijBus;
		if(akt_linia=="WKD" || akt_linia[0]=='K' || akt_linia[0]=='S')
			dij=&dijRail;
		else
			if(akt_linia.length()<3)
				dij=&dijTram;
		przerabiaj=1;
		if(linki.find(akt_linia)==linki.end())
			przerabiaj=0;
		if(przerabiaj)
		{
		cout<<"#"<<akt_linia<<endl;
		bool ok=1;
		bool ok2=1;
		int s9=it1->second.size();
		for(int i=0; i<s9; i++)
		{
			int s8=(it1->second)[i].size();
			vector <long long> ids1;
			vector <long long> hig1;
			for(int j=0; j<s8; j++)
			{
				przystanek_big& data=(it1->second)[i][j];
				if(data.stop_position==0)
				{
					cout<<"error: "<<data.name<<"-"<<data.id<<": "<<"BRAK STOP_POSITION"<<endl;
					ok=0;
				}
				else
				{
					if(data.pos_error)
					{
						cout<<"error: "<<data.name<<"-"<<data.id<<": "<<"STOP_POSITION nie leży na drodze"<<endl;
						ok=0;
					}
				}
				ids1.push_back(data.stop_position);
				if(data.bus_stop!=0)
				{
					hig1.push_back(data.bus_stop);
				}
				else
				{
					hig1.push_back(data.stop_position);
				}
			}
			if(ok)
			{
				ids[akt_linia].push_back(ids1);
				high_stops[akt_linia].push_back(hig1);
			}
		}
		if(ok && ok2)
		{
			for(int g=0; g<ids[akt_linia].size(); g++)
			{
				dij_data d0(ids[akt_linia][g], dij);
				if(!d0.ok)
					ok=0;
				if(ok && ok2)
				{
					
				}
			}
		}
		if(!ok || !ok2)
		{
			if(ids.find(akt_linia)!=ids.end())
				ids.erase(ids.find(akt_linia));
			if(high_stops.find(akt_linia)!=high_stops.end())
				high_stops.erase(high_stops.find(akt_linia));
	
		}
		}
		it1++;
	}


	map<string, vector <vector <long long> > >::iterator it7=ids.begin();
	plik5<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?><gpx>"<<endl;
	while(it7!=ids.end())
	{
		string akt_linia=it7->first;
		dijkstra* dij = &dijBus;
		string typ_maly="bus";
		string typ_duzy="Bus";
		if(akt_linia=="WKD" || akt_linia[0]=='K' || akt_linia[0]=='S')
		{
			typ_maly="train";
			if(akt_linia[0]=='S')
				typ_duzy="SKM";
			else
				typ_duzy="Koleje Mazowieckie";
			dij=&dijRail;
		}
		else
		{
			if(akt_linia.length()<3)
			{
				dij=&dijTram;
				typ_maly="tram";
				typ_duzy="Tram";
			}
		}
		string arglin=akt_linia+".gpx";
		plik5.precision(9);
		linia_licznik++;
		vector <long long> relation_ids;
		vector <long long> prop_id=relacje_linia(bazuka, 3651336,akt_linia).second;
		it1=ztm.dane_linia.find(akt_linia);
		for(int g=0; g<ids[akt_linia].size(); g++)
		{
			long long aktoo=-1;
			long long aktoo2=-1;
			relation rel;
			rel.modify=1;
			rel.tags["network"]="ZTM Warszawa";
			rel.tags["type"]="route";
			rel.tags["route"]=typ_maly;
			rel.tags["ref"]=akt_linia;
			rel.tags["name"]=typ_duzy+" "+akt_linia+": "+nazwa_skrocona((it1->second)[g][0].name_osm)+" => "+nazwa_skrocona((it1->second)[g][(it1->second)[g].size()-1].name_osm);
			rel.tags["from"]=nazwa_skrocona((it1->second)[g][0].name_osm);
			rel.tags["to"]=nazwa_skrocona((it1->second)[g][(it1->second)[g].size()-1].name_osm);
			rel.tags["note"]="stan na "+today;
			rel.tags["source"]="Rozkład jazdy ZTM Warszawa, trasa wygenerowana przez bot";
			long long kand_id=(g*-1)-linia_licznik*100;
			if(prop_id.size()>0)
			{
				cout<<"WYBIERZ RELACJĘ DLA "<<rel.tags["name"]<<endl;
				for(int i=0; i<prop_id.size(); i++)
				{
					cout<<i+1<<" "<<bazuka.relations[prop_id[i]].tags["name"]<<" "<<prop_id[i]<<endl;
				}
				int kand2=0;
				if(czy_rel)
					cin>>kand2;
				if(kand2>0)
				{
					kand_id=prop_id[kand2-1];
				}
			}
			relation_ids.push_back(kand_id);
			rel.id=relation_ids[relation_ids.size()-1];
			plik5<<"<trk>"<<endl;
			plik5<<"<name>"<<rel.tags["name"]<<"</name>"<<endl;
			plik5<<"<trkseg>"<<endl;
			dij_data d1(ids[akt_linia][g], dij);
			vector <long long> way_id2=d1.all_ways;
			vector <long long> terids=d1.all_nodes;
			for(int i=0; i<terids.size(); i++)
			{
				plik5<<"<trkpt lat=\""<<bazuka.nodes[terids[i]].lat<<"\" lon=\""<<bazuka.nodes[terids[i]].lon<<"\"></trkpt>"<<endl;
			}
			for(int i=0; i<way_id2.size(); i++)
			{
				wyb_w.insert(way_id2[i]);
				relation_member foo;
				foo.member_type=WAY;
				foo.member_id=way_id2[i];
				foo.role="";
				rel.members.push_back(foo);
			}
			for(int i=0; i<high_stops[akt_linia][g].size(); i++)
			{
				string role="stop";
				if(i==0)
					role="stop_entry_only";
				if(i==high_stops[akt_linia][g].size()-1)
					role="stop_exit_only";
				wyb_n.insert(high_stops[akt_linia][g][i]);
				relation_member foo;
				foo.member_type=NODE;
				foo.member_id=high_stops[akt_linia][g][i];
				foo.role=role;
				rel.members.push_back(foo);
			}
			if(rel.id>0)
				rel.version=bazuka.relations[rel.id].version;
			bazuka.relations[rel.id]=rel;
			plik5<<"</trkseg></trk>"<<endl;
		}
		relation rel;
		rel.modify=1;
		rel.id=(-50)-linia_licznik*100;
		long long prop_ids=relacje_linia(bazuka, 3651336,akt_linia).first;
		if(prop_ids!=0)
			rel.id=prop_ids;
		rel.tags["network"]="ZTM Warszawa";
		rel.tags["type"]="route_master";
		rel.tags["route_master"]=typ_maly;
		rel.tags["ref"]=akt_linia;
		rel.tags["url"]="http://ztm.waw.pl/rozklad_nowy.php?c=182&l=1&q="+akt_linia;
		rel.tags["note"]="stan na "+today;
		rel.tags["source"]="Rozkład jazdy ZTM Warszawa, trasa wygenerowana przez bot";
		rel.tags["name"]=typ_duzy+" "+akt_linia;
		for(int g=0; g<relation_ids.size(); g++)
		{
				relation_member foo;
				foo.member_type=RELATION;
				foo.member_id=relation_ids[g];
				foo.role="";
				rel.members.push_back(foo);
		}
		if(rel.id>0)
			rel.version=bazuka.relations[rel.id].version;
		bazuka.relations[rel.id]=rel;
		it7++;
	}
	plik5<<"</gpx>"<<endl;
	plik5.close();
	osm_base bazuka3=bazuka.wybrane(wyb_n, wyb_w, wyb_r);
	osm_base bazuka4=bazuka.modified();
	osm_base bazuka2=osm_base(bazuka4, bazuka3);
	bazuka2.wypisz(sciezka);
	it7=ids.begin();
	while(it7!=ids.end())
	{
		cout<<it7->first<<endl;
		it7++;
	}*/
}
