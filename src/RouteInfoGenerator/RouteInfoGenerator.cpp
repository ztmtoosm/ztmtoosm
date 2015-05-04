#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include "../../include/RouteInfoGenerator/ztmosmpor.hpp"
#include "../../include/osmbase/osm_base.hpp"
#include "../../include/RouteInfoGenerator/HTMLHeadersRouteinfo.hpp"
#include "../../include/RouteInfoGenerator/PrzegladanieCzyPrawidloweStareLinie.hpp"
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
	stringstream foo;
	foo<<"http://www.openstreetmap.org/?mlat=";
	foo<<lon;
	foo<<"&mlon=";
	foo<<lat;
	foo<<"&zoom=18";
	return htmlgen::link(foo.str(), "X");
}

class PrzegladanieCzyPrawidloweNoweLinie
{
	set <string> doPrzerobienia;
	map <string, OsmStopData>* osmStops;
	ztmread_for_html* bazaZtm;
	bool sprawdzLinie(string linia, vector <vector <string> > drugi, map <string, OsmStopData>* osmStops, map<string, string>* infoHTML, set <string>* blednePrzystanki)
	{
		string bledy;
		if(doPrzerobienia.find(linia)==doPrzerobienia.end())
			return false;
		int s9=drugi.size();
		bool ok=1;
		for(int i=0; i<s9; i++)
		{
			int s8=(drugi)[i].size();
			for(int j=0; j<s8; j++)
			{
				OsmStopData data=(*osmStops)[drugi[i][j]];
				if(data.stop_position==0)
				{
					bledy+=htmlgen::div("stop_pos_non", "", bazaZtm->przystanki[drugi[i][j]].name+" "+drugi[i][j]+" brak STOP_POSITION");
					blednePrzystanki->insert(drugi[i][j]);
					ok=0;
				}
				else
				{
				/*	
					TODO BRAK STOP POSITION NA DRODZE
					if(data.pos_error)
					{
						bledy+=htmlgen::div("stop_pos_way", "", data.name+" "+data.id+" STOP_POSITION nie leży na drodze");
						ok=0;
					}*/
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
	PrzegladanieCzyPrawidloweNoweLinie(map<string, OsmStopData>* osmStops, ztmread_for_html* bazaZtmW, set <string> doPrzerobieniaW, map <string, string>* infoHTML, set<string>* blednePrzystanki)
	{
		bazaZtm = bazaZtmW;
		doPrzerobienia=doPrzerobieniaW;
		set <string>::iterator it1=doPrzerobienia.begin();
		while(it1!=doPrzerobienia.end())
		{
			if(sprawdzLinie(*it1, bazaZtm->dane_linia[*it1], osmStops, infoHTML, blednePrzystanki))
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

/*
class JSONObjectCore
{
	map <string, string> tagsNormal;
	map <string, JSONObject> tagsObject;
	void addNumber (string key, int value)
	{
		stringstream foo;
		foo<<value;
		string value2;
		foo>>value2;
		tagsNormal[key]=value2;
	}
	virtual string print()
	{
	}
};
*/

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
	string jsonEnc (string k)
	{
		string wynik;
		for(int i=0; i<k.length(); i++)
		{
			if(wynik[i]=='"')
			{
				wynik+="\\";
			}
			wynik+=k[i];
		}
		return wynik;
	}
	void addTag(int i, string key, string value, fstream& plik)
	{
		if(i>0)
			plik<<",";
		plik<<"{\"key\":\""<<jsonEnc(key)<<"\",\"value\":\""<<jsonEnc(value)<<"\"}";
	}
	void addMember(int i, string category, long long id, string role, fstream& plik)
	{
		if(i>0)
			plik<<",";
		plik<<"{\"category\":\""<<category<<"\",\"id\":\""<<id<<"\",\"role\":\""<<role<<"\"}";
	}
	void addTags(map<string, string> k, fstream& plik)
	{
		plik<<"\"tags\":[";
		auto it1 = k.begin();
		while(it1!=k.end())
		{
			int id = 1;
			if(it1 == k.begin())
				id = 0;
			addTag(id, it1->first, it1->second, plik);
			it1++;
		}
		plik<<"],"<<endl;
	}
	string otoczeniePrzystanku(string idPrzystanek, string idLinia, int idWariantu, int idKol)
	{
		string poprzedni = "POCZĄTKOWY";
		string kolejny = "KOŃCOWY";
		if(idKol>0)
		{
			string poprzedniId = bazaZtm->dane_linia[idLinia][idWariantu][idKol-1];
			poprzedni = bazaZtm->przystanki[poprzedniId].name;
		}
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol)
		{
			string kolejnyId = bazaZtm->dane_linia[idLinia][idWariantu][idKol+1];
			kolejny = bazaZtm->przystanki[kolejnyId].name;
		}
		string ostatniId = bazaZtm->dane_linia[idLinia][idWariantu][bazaZtm->dane_linia[idLinia][idWariantu].size()-1];
		string ostatni = bazaZtm->przystanki[ostatniId].name;
		string info = idLinia + " -> "+ostatni+" poprzedni: "+poprzedni+" kolejny: "+kolejny;
		return htmlgen::div("otoczenieliniaprzystanek", "", info);
	}
	string wypiszBlednyPrzystanek(string it1)
	{
		string info = it1 + " " + bazaZtm->przystanki[it1].name+" ";
		info += " "+htmlgen::div("stopinfo", "", bazaZtm->przystanki[it1].stopinfo);
		info += " "+htmlgen::div("znaczniklink", "", znacznikLink(bazaZtm->przystanki[it1].lat, bazaZtm->przystanki[it1].lon));
		for(auto& it2 : bazaZtm->dane_linia)
		{
			for(int i=0; i<it2.second.size(); i++)
			{
				for(int j=0; j<it2.second[i].size(); j++)
				{
					if(it2.second[i][j]==it1)
						info+=otoczeniePrzystanku(it1, it2.first, i, j);
				}
			}
		}
		return info;
	}
	void generujLinie(string nazwa)
	{
		cout<<"GENEROWANIE "<<nazwa<<endl;
		vector <long long> stareRelacje=relacje_linia(bazaOsm, 3651336, nazwa).second;
		long long stareId=relacje_linia(bazaOsm, 3651336, nazwa).first;
		if(stareId==0)
			stareId=-1;
		string nazwaGEN=pathHTML+"/js"+nazwa+".json";
		fstream plik(nazwaGEN.c_str(), ios::out | ios::trunc);
		vector <long long> noweRelacje;
		int s1 = bazaZtm -> dane_linia[nazwa].size();
		plik<<"[";
		for(int i=0; i<s1; i++)
		{
			vector <string> wariant = bazaZtm -> dane_linia[nazwa][i];
			string from = substituteWhiteCharsBySpace(osmStopData[wariant[0]].name);
			string to = substituteWhiteCharsBySpace(osmStopData[wariant[wariant.size()-1]].name);
			int wariantOsmRelId = i-100;
			if(i<stareRelacje.size())
				wariantOsmRelId = stareRelacje[i];
			map <string, string> tags;
			if(i>0)
				plik<<",";
			plik<<"{ \"id\":"<<wariantOsmRelId<<","<<endl;
			plik<<"\"track_type\":\""<<nazwa_mala(nazwa)<<"\",\"parentrel\":[],"<<endl;
			tags["ref"]=nazwa;
			tags["type"]="route";
			tags["network"]="ZTM Warszawa";
			tags["route"]=nazwa_mala(nazwa);
			tags["from"]=from;
			tags["to"]=to;
			tags["name"]=nazwa_duza(nazwa)+" "+nazwa+": "+from+" => "+to;
			tags["source"]="Rozkład jazdy ZTM Warszawa, trasa wygenerowana przez bot";
			/*vector <string> miasta = miejscowosci(bazaZtm->dane_linia[nazwa][i];
			string via;
			for(int i=1; i<(miasta.size()-1); i++)
			{
				via+=miasta[i];
				if(i<(miasta.size()-2))
					via+=", ";
			}*/
			/*if(via!="")
			{
				tags["via"]=via;
			}*/
			addTags(tags, plik);
			plik<<"\"track\":[";
			for(int j=0; j<wariant.size(); j++)
			{
				if(j>0)
					plik<<",";
				plik<<osmStopData[wariant[j]].stop_position;
			}
			plik<<"]";
			plik<<",\"members\":[";
			for(int j=0; j<wariant.size(); j++)
			{
				string dopisek="";
				if(j==0)
					dopisek="_entry_only";
				if(j==wariant.size()-1)
					dopisek="_exit_only";
				if(osmStopData[wariant[j]].bus_stop!=0)
					addMember(j, "N", osmStopData[wariant[j]].bus_stop, "stop"+dopisek, plik);
				else if(osmStopData[wariant[j]].stop_position!=0)
					addMember(j, "N", osmStopData[wariant[j]].stop_position, "stop"+dopisek, plik);
				if(osmStopData[wariant[j]].platform!=0)
				{
					string type="";
					type+=osmStopData[wariant[j]].platform_type;
					addMember(1, type, osmStopData[wariant[j]].platform, "platform"+dopisek, plik);
				}
			}
			plik<<"]}";
			noweRelacje.push_back(wariantOsmRelId);
		}
		if(s1<stareRelacje.size())
		{
			for(int i=s1; i<stareRelacje.size(); i++)
			{
				plik<<",{\"track\":[],\"members\":[], \"id\":"<<stareRelacje[i]<<",\"tags\":[], \"todelete\":true}";
			}
		}
		map <string, string> tags;
		tags["type"] = "route_master";
		tags["ref"] = nazwa;
		tags["source"]="Rozkład jazdy ZTM Warszawa, trasa wygenerowana przez bot";
		tags["name"] = nazwa_duza(nazwa)+" "+nazwa;
		tags["type"] = "route_master";
		tags["url"] = "http://ztm.waw.pl/rozklad_nowy.php?c=182&l=1&q="+nazwa;
		tags["route_master"] = nazwa_mala(nazwa);
		tags["network"] = "ZTM Warszawa";
		plik<<",{\"id\":"<<stareId<<",\"parentrel\":["<<getParentRelation(nazwa)<<"],"<<endl;
		addTags(tags, plik);
		plik<<"\"track\":[],\"members\":[";
		for(int i=0; i<noweRelacje.size(); i++)
		{
			addMember(i, "R", noweRelacje[i], "", plik);
		}
		plik<<"]}]";
		/*
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
		*/
	}

	map <string, string> infoHTML;
	string ok_route(string cos)
	{
		return htmlgen::div("ok_route", cos, "Trasa "+cos+" wygenerowana, pokaż...");
	}
	string oklink(string linia)
	{
		return "<a href=\"wyszukiwarka2.html?linia="+linia+"\" target=\"_blank\">Pokaż wygenerowany zestaw</a>";
	}
	void wypiszRaport()
	{

	}
	galk(char** argv)
	{
		readArg(argv);
		readInput();
		bazaOsm = new osm_base(osmBasePath);
		osmStopData = loadOsmStopData(bazaOsm);
		bazaZtm = new ztmread_for_html (ztmBasePath);
		if(czyWszystkie)
			linieDoPrzerobienia=wszystkieLinie(bazaZtm);
		cout<<"xxxxxx"<<endl;
		PrzegladanieCzyPrawidloweStareLinie przegl0(bazaOsm, bazaZtm, linieDoPrzerobienia, &infoHTML);
		set <string> etap=przegl0.nieprawidlowe;
		if(!czyWszystkie)
			etap=linieDoPrzerobienia;
		set <string> blednePrzystanki;
		PrzegladanieCzyPrawidloweNoweLinie przegl(&osmStopData, bazaZtm, etap, &infoHTML, &blednePrzystanki);
		linieDoPrzerobienia=przegl.prawidlowe;
		set <string>::iterator it1=linieDoPrzerobienia.begin();
		int licznik=1000;
		string n2=pathHTML+"/openlayers.html";
		if(!czyWszystkie)
			n2=pathHTML+"/openlayers2.html";
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
		auto it2=linieDoPrzerobienia.begin();
		while(it2!=linieDoPrzerobienia.end())
		{
			generujLinie(*it2);
			slownik0[zeraWiodace(*it2)]=*it2;
			slownikX[zeraWiodace(*it2)]=make_pair(*it2, 1);
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
		plik5<<htmlgen::div("partx", "", "Błędne Przystanki")<<endl;
		for(string it1 : blednePrzystanki)
		{
			plik5<<htmlgen::div("bprzyst", "", wypiszBlednyPrzystanek(it1))<<endl;
		}
		plik5<<htmlgen::div("partx", "", "Trasy wygenerowane...")<<endl;
		auto it0prim=slownik0.begin();
		while(it0prim!=slownik0.end())
		{
			plik5<<htmlgen::div("linia_green", "", infoHTML[it0prim->second]+oklink(it0prim->second))<<endl;
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
