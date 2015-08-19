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

class WlasciwosciLokalne
{
	public:
	virtual string nazwaMala(string lineName) = 0;
	virtual string nazwaDuza(string lineName) = 0;
	virtual long long getParentRelation (string name) = 0;
	virtual set <string> wszystkieLinie(ztmread_for_html* bazaZtm) = 0;
	virtual long long getRootRelation() = 0;
	virtual string getNazwaMiasta() = 0;
	virtual string getRefKey() = 0;
	virtual string getNetworkName() = 0;
	virtual string getUrlLine(string line) = 0;
	virtual string substituteWhiteCharsBySpace(string nazwa)
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
};

class WlasciwosciLokalneWarszawa : public WlasciwosciLokalne
{
	public:
	string nazwaMala(string lineName)
	{
		if(lineName.length()<3)
			return "tram";
		return "bus";
	}
	string nazwaDuza(string lineName)
	{
		if(lineName.length()<3)
			return "Tram";
		return "Bus";
	}
	string getRefKey()
	{
		return "ref";
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
	long long getRootRelation()
	{
		return 3651336;
	}
	string getNazwaMiasta()
	{
		return "Warszawa";
	}
	string getNetworkName()
	{
		return "ZTM Warszawa";
	}
	string getUrlLine(string line)
	{
		return "http://ztm.waw.pl/rozklad_nowy.php?c=182&l=1&q="+line;
	}
};

class WlasciwosciLokalneSzczecin : public WlasciwosciLokalne
{
	public:
	string nazwaMala(string lineName)
	{
			return "tram";
	}
	string nazwaDuza(string lineName)
	{
			return "Tram";
	}
string getRefKey()
	{
		return "ref";
	}
	long long getParentRelation (string name)
	{
		return 5291742;
	}

	set <string> wszystkieLinie(ztmread_for_html* bazaZtm)
	{
		set <string> wynik;
		auto it1=bazaZtm->dane_linia.begin();
		while(it1!=bazaZtm->dane_linia.end())
		{
			string kand=it1->first;
			wynik.insert(kand);
			it1++;
		}
		return wynik;
	}
	long long getRootRelation()
	{
		return 5291742;
	}
	string getNazwaMiasta()
	{
		return "Szczecin";
	}
	string substituteWhiteCharsBySpace(string nazwa)
	{
		cout<<"KUKU!!!!!!!!!!"<<endl;
		return nazwa;
	}
	string getNetworkName()
	{
		return "ZDiTM Szczecin";
	}
	string getUrlLine(string line)
	{
		return "";
	}
};

class WlasciwosciLokalneGdansk : public WlasciwosciLokalne
{
	public:
	string nazwaMala(string lineName)
	{
			if(lineName.length()==2 && isdigit(lineName[0]))
				return "tram";
			if(lineName=="N0")
				return "tram";
			return "bus";
	}
	string nazwaDuza(string lineName)
	{
			if(lineName.length()==2 && isdigit(lineName[0]))
				return "Tram";
			if(lineName=="N0")
				return "Tram";
			return "Bus";
	}
	string getRefKey()
	{
		return "ref:ztm";
	}
	long long getParentRelation (string name)
	{
		return 5411513;
	}

	set <string> wszystkieLinie(ztmread_for_html* bazaZtm)
	{
		set <string> wynik;
		auto it1=bazaZtm->dane_linia.begin();
		while(it1!=bazaZtm->dane_linia.end())
		{
			string kand=it1->first;
			wynik.insert(kand);
			it1++;
		}
		return wynik;
	}
	long long getRootRelation()
	{
		return 5411513;
	}
	string getNazwaMiasta()
	{
		return "Gdańsk";
	}
	string substituteWhiteCharsBySpace(string nazwa)
	{
		return nazwa;
	}
	string getNetworkName()
	{
		return "ZTM Gdańsk";
	}
	string getUrlLine(string line)
	{
		return "http://www.ztm.gda.pl/rozklady/linia-"+line+".html";
	}
};



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

set <long long> podpieteRelacje(osm_base* roo, long long root)
{
	set <long long> wynik;
	relation akt=roo->relations[root];
	map <string, string> tags = akt.getTags();
	if(tags["type"]=="route_master" || tags["type"]=="route")
	{
		wynik.insert(root);
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
				auto nowe = podpieteRelacje(roo, teraz_id);
				wynik.insert(nowe.begin(), nowe.end());
			}
		}
	}
	return wynik;
}

set <long long> dziwneRelacje(osm_base* roo, long long root)
{
	set <long long> wynik;
	set <long long> reszta = podpieteRelacje(roo, root);
	for(auto& it1 : roo->relations)
	{
		map<string, string> tagi = it1.second.getTags();
		if(tagi["route"]=="bus" || tagi["route"]=="tram")
		{
			if(reszta.find(it1.first)==reszta.end())
			{
				wynik.insert(it1.first);
			}
		}
	}
	return wynik;
}


set <string> linieDoUsuniecia(ztmread_for_html* baza_ztm, osm_base* roo, long long root)
{
	set <string> wsadowy;
	for(auto& it1 : baza_ztm->dane_linia)
	{
		wsadowy.insert(it1.first);
	}
	return linieDoUsuniecia(wsadowy, roo, root);
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
		set <string> badStops;
		for(int i=0; i<s9; i++)
		{
			int s8=(drugi)[i].size();
			for(int j=0; j<s8; j++)
			{
				OsmStopData data=(*osmStops)[drugi[i][j]];
				if(data.stop_position==0)
				{
					blednePrzystanki->insert(drugi[i][j]);
					badStops.insert(drugi[i][j]);
					ok=0;
				}
			}
		}
		for(auto& it : badStops)
		{
			bledy+=htmlgen::div("stop_pos_non", "", bazaZtm->przystanki[it].name+" "+it);
		}
		if(!ok)
		{
			stringstream bd;
			bd<<badStops.size();
			bledy=htmlgen::div("bledy2", "", "Liczba przystanków bez stop_position na linii: "+bd.str()+" "+bledy);
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
	WlasciwosciLokalne* wlasciwosci;
	osm_base* bazaOsm;
	ztmread_for_html* bazaZtm;
	string miasto;
	string ztmBasePath;
	string osmBasePath;
	string pathHTML;
	string pathTemplate;
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
		miasto=argv[3];
		pathHTML=argv[4];
		pathTemplate="/ztmtoosm/templates-www";
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
	/*
	void addLine(int status, string name, fstream& plik)
	{
		string panel_type = "panel-info";
		if(status==1)
			panel_type= "panel-danger";
		if(status==2)
			panel_type="panel-success";
		stringstream wew_str;
		wew_str<<htmlgen::div("panel-heading", "", name);
		plik<<htmlgen::div("panel "+panel_type, "", wew_str.str());
	}*/
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
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+1)
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
	string wypiszPrzystanekDoUsuniecia(string id, string name, double lon, double lat)
	{
		string info = id + " " + name+" ";
		info += " "+htmlgen::div("znaczniklink", "", znacznikLink(lat, lon));
		return info;
	}
	void generujLinie(string nazwa)
	{
		cout<<"GENEROWANIE "<<nazwa<<endl;
		vector <long long> stareRelacje=relacje_linia(bazaOsm, wlasciwosci->getRootRelation(), nazwa).second;
		long long stareId=relacje_linia(bazaOsm, wlasciwosci->getRootRelation(), nazwa).first;
		if(stareId==0)
			stareId=-1;
		string nazwaGEN=pathHTML+"/js"+wlasciwosci->getNazwaMiasta()+nazwa+".json";
		fstream plik(nazwaGEN.c_str(), ios::out | ios::trunc);
		vector <long long> noweRelacje;
		int s1 = bazaZtm -> dane_linia[nazwa].size();
		plik<<"[";
		for(int i=0; i<s1; i++)
		{
			cout<<"s1 "<<nazwa<<endl;
			vector <string> wariant = bazaZtm -> dane_linia[nazwa][i];
			cout<<"s1.5 "<<nazwa<<" "<<wariant.size()<<endl;
			cout<<"s1.5 "<<osmStopData[wariant[0]].name<<endl;
			string from = wlasciwosci->substituteWhiteCharsBySpace(osmStopData[wariant[0]].name);
			cout<<"s1.8 "<<nazwa<<endl;
			string to = wlasciwosci->substituteWhiteCharsBySpace(osmStopData[wariant[wariant.size()-1]].name);
			cout<<"s2"<<endl;
			int wariantOsmRelId = i-100;
			if(i<stareRelacje.size())
				wariantOsmRelId = stareRelacje[i];
			map <string, string> tags;
			if(i>0)
				plik<<",";
			plik<<"{ \"id\":"<<wariantOsmRelId<<","<<endl;
			plik<<"\"track_type\":\""<<wlasciwosci->nazwaMala(nazwa)<<"\",\"parentrel\":[],"<<endl;
			cout<<"s3"<<endl;
			tags["ref"]=nazwa;
			tags["type"]="route";
			tags["network"]=wlasciwosci->getNetworkName();
			tags["route"]=wlasciwosci->nazwaMala(nazwa);
			tags["from"]=from;
			tags["to"]=to;
			tags["name"]=wlasciwosci->nazwaDuza(nazwa)+" "+nazwa+": "+from+" => "+to;
			tags["public_transport:version"] = "2";
			tags["source"]="Rozkład jazdy "+wlasciwosci->getNetworkName()+", trasa wygenerowana przez bot";
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
			cout<<"s4"<<endl;
			plik<<"\"track\":[";
			for(int j=0; j<wariant.size(); j++)
			{
				if(j>0)
					plik<<",";
				cout<<wariant[j]<<endl;
				plik<<osmStopData[wariant[j]].stop_position;
			}
			cout<<"TRA"<<endl;
			plik<<"]";
			plik<<",\"members\":[";
			for(int j=0; j<wariant.size(); j++)
			{
			cout<<"XI1"<<endl;
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
				cout<<"XI2"<<endl;
			}
			plik<<"]}";
			noweRelacje.push_back(wariantOsmRelId);
		}
		cout<<"pppp"<<endl;
		if(s1<stareRelacje.size())
		{
			for(int i=s1; i<stareRelacje.size(); i++)
			{
				plik<<",{\"track\":[],\"members\":[], \"id\":"<<stareRelacje[i]<<",\"tags\":[], \"todelete\":true}";
			}
		}
		cout<<"qqqqqqq"<<endl;
		map <string, string> tags;
		tags["type"] = "route_master";
		tags["ref"] = nazwa;
		tags["source"]="Rozkład jazdy "+wlasciwosci->getNetworkName()+", trasa wygenerowana przez bot";
		tags["name"] = wlasciwosci->nazwaDuza(nazwa)+" "+nazwa;
		tags["type"] = "route_master";
		tags["url"] = wlasciwosci->getUrlLine(nazwa);
		tags["route_master"] = wlasciwosci->nazwaMala(nazwa);
		tags["network"] = wlasciwosci->getNetworkName();
		tags["public_transport:version"] = "2";
		plik<<",{\"id\":"<<stareId<<",\"parentrel\":["<<wlasciwosci->getParentRelation(nazwa)<<"],"<<endl;
		addTags(tags, plik);
		plik<<"\"track\":[],\"members\":[";
		for(int i=0; i<noweRelacje.size(); i++)
		{
			addMember(i, "R", noweRelacje[i], "", plik);
		}
		plik<<"]}]";
	}

	map <string, string> infoHTML;
	string attention(string line)
	{
		return htmlgen::div("attention", "attention"+line, "");
	}
	string ok_route(string cos)
	{
		return htmlgen::div("ok_route", cos, "Trasa "+cos+" wygenerowana, pokaż...");
	}
	string oklink(string linia)
	{
		return "<a href=\"wyszukiwarka2.html?linia="+miasto+linia+"\" target=\"_blank\">Pokaż wygenerowany zestaw</a>"+"</br><a href=\"wyszukiwarka2.html?x=d&linia="+miasto+linia+"\" target=\"_blank\">Pokaż wygenerowany zestaw - routing testowy</a>";
	}

	void dodajLinieDoHTML(fstream& plik, int typ, string name, string content, HtmlExtraGenerator& gen)
	{
		cout<<"LOADER START"<<endl;
		gen.loadedVariables[0]="panel-success";
		gen.loadedVariables[1]=name;
		gen.loadedVariables[2]=content;
		plik<<gen.loadTemplate(pathTemplate+"/lineHeader.template");
		cout<<"LOADER STOP"<<endl;
	}

	galk(char** argv)
	{
		HtmlExtraGenerator htmlGenerator;
		readArg(argv);
		readInput();
		if(miasto=="Warszawa")
			wlasciwosci = new WlasciwosciLokalneWarszawa();
		if(miasto=="Szczecin")
			wlasciwosci = new WlasciwosciLokalneSzczecin();
		if(miasto=="Gdańsk")
			wlasciwosci = new WlasciwosciLokalneGdansk();
		bazaOsm = new osm_base(osmBasePath);
		cerr<<"BAZA OSM - OK"<<endl;
		osmStopData = loadOsmStopData(bazaOsm, wlasciwosci->getRefKey());
		cerr<<"OSM-DATA - OK"<<endl;
		bazaZtm = new ztmread_for_html (ztmBasePath, miasto);
		cerr<<"Załadowano dane "<<bazaZtm<<endl;
		if(czyWszystkie)
			linieDoPrzerobienia=wlasciwosci->wszystkieLinie(bazaZtm);
		PrzegladanieCzyPrawidloweStareLinie przegl0(bazaOsm, bazaZtm, linieDoPrzerobienia, &infoHTML, wlasciwosci->getRootRelation(), wlasciwosci->getRefKey());
		cerr<<"etap1 "<<endl;
		set <string> etap=przegl0.nieprawidlowe;
		if(!czyWszystkie)
			etap=linieDoPrzerobienia;
		set <string> blednePrzystanki;
		cerr<<"etap2 "<<endl;
		PrzegladanieCzyPrawidloweNoweLinie przegl(&osmStopData, bazaZtm, etap, &infoHTML, &blednePrzystanki);
		cerr<<"etap3 "<<endl;
		linieDoPrzerobienia=przegl.prawidlowe;
		set <string>::iterator it1=linieDoPrzerobienia.begin();
		int licznik=1000;
		string n2=pathHTML+"/Pelne"+miasto+".html";
		string n4=pathHTML+"/Pelne"+miasto+"bis.html";
		string n3=pathHTML+"/List"+miasto+".json";
		fstream plik5(n2.c_str(), ios::out | ios::trunc);
		fstream nowyPlik5(n4.c_str(), ios::out | ios::trunc);
		uzupelnij(nowyPlik5, pathTemplate+"/theme.template");
		nowyPlik5<<miasto<<" - linie";
		uzupelnij(nowyPlik5, pathTemplate+"/themeA.template");
		nowyPlik5<<"Stan na: ";
		char buff[20];
		time_t now = time(NULL);
		plik5<<htmlgen::div("partx", "", miasto+" - pełne zestawienie")<<endl;
		strftime(buff, 20, "%d.%m.%Y %H:%M", localtime(&now));
		string buff2=buff;
		nowyPlik5<<buff2;
		uzupelnij(nowyPlik5, pathTemplate+"/themeB.template");
		fstream plik6(n3.c_str(), ios::out | ios::trunc);
		plik5.precision(9);
		htmlHead(plik5);
		plik5<<htmlgen::div("gentime", "", "Wygenerowano: "+buff2)<<endl;
		map <string, string> slownik0;
		map <string, pair<string, int> > slownikX;
		auto it2=linieDoPrzerobienia.begin();
		while(it2!=linieDoPrzerobienia.end())
		{
			cout<<"START "<<*it2<<endl;
			generujLinie(*it2);
			cout<<"STOP "<<*it2<<endl;
			slownik0[zeraWiodace(*it2)]=*it2;
			slownikX[zeraWiodace(*it2)]=make_pair(*it2, 1);
			it2++;
			licznik++;
		}
		cout<<"xyz1"<<endl;
		map <string, string> slownik1;
		auto it3prim=przegl.nieprawidlowe.begin();
		while(it3prim!=przegl.nieprawidlowe.end())
		{
			slownik1[zeraWiodace(*it3prim)]=*it3prim;
			slownikX[zeraWiodace(*it3prim)]=make_pair(*it3prim, 2);
			it3prim++;
		}
		cout<<"xyz2"<<endl;
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
		cout<<"xyz3"<<endl;
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
		stringstream p5_tmp, p6_tmp, p7_tmp;
		for(string it1 : blednePrzystanki)
		{
			p5_tmp<<htmlgen::div("bprzyst", "", wypiszBlednyPrzystanek(it1))<<endl;
		}
		plik5<<htmlgen::div("blstops", "", p5_tmp.str())<<endl;
		plik5<<htmlgen::div("partx", "", "Linie do usunięcia")<<endl;
		auto doUsuniecia = linieDoUsuniecia(bazaZtm, bazaOsm, wlasciwosci->getRootRelation());
		for(auto& it1 : doUsuniecia)
		{
			p6_tmp<<it1<<"</br>";
		}
		plik5<<htmlgen::div("do_usuniecia", "", p6_tmp.str())<<endl;
		plik5<<htmlgen::div("partx", "", "Inne relacje komunikacji w bazie OSM")<<endl;
		auto dziwne = dziwneRelacje(bazaOsm, wlasciwosci->getRootRelation());
		for(auto& it1 : dziwne)
		{
			stringstream wyn, wyn0;
			wyn0<<it1;
			wyn<<htmlgen::div("dziwna_linia_id", "", wyn0.str());
			auto tags = bazaOsm->relations[it1].getTags();
			for(auto& it2 : tags)
			{
				wyn<<htmlgen::div("dziwna_linia_dup", "", it2.first+" = "+it2.second);
			}
			p7_tmp<<htmlgen::div("dziwna_linia", "", wyn.str())<<"</br>";
		}

		cout<<"AAA-END"<<endl;
		plik5<<htmlgen::div("dziwne", "", p7_tmp.str())<<endl;
		plik5<<htmlgen::div("partx", "", "Trasy wygenerowane...")<<endl;
		auto it0prim=slownik0.begin();
		plik6<<"[";
		int licznikx=0;
		while(it0prim!=slownik0.end())
		{
			if(licznikx>0)
				plik6<<",";
			plik6<<"\""<<it0prim->second<<"\"";
			plik5<<htmlgen::div("linia_green", "", infoHTML[it0prim->second]+oklink(it0prim->second)+attention(it0prim->second))<<endl;
			dodajLinieDoHTML(nowyPlik5,2,it0prim->second, "", htmlGenerator);
			it0prim++;
			licznikx++;
		}
		cout<<"ZIK-END"<<endl;
		plik6<<"]";
		plik6.close();
		plik5<<htmlgen::div("partx", "", "Trasy niewygenerowane...")<<endl;
		
		auto it3=slownik1.begin();
		while(it3!=slownik1.end())
		{
			plik5<<htmlgen::div("linia_red", "", infoHTML[it3->second]+attention(it3->second))<<endl;
			dodajLinieDoHTML(nowyPlik5,1,it3->second, "", htmlGenerator);
			it3++;
		}
		cout<<"RED-END"<<endl;
		if(czyWszystkie)
		{
			plik5<<htmlgen::div("partx", "", "Trasy bez zmian...")<<endl;
			auto it4=slownik2.begin();
			while(it4!=slownik2.end())
			{
				plik5<<htmlgen::div("linia_blue", "", infoHTML[it4->second]+attention(it4->second))<<endl;
				dodajLinieDoHTML(nowyPlik5,0,it4->second, "", htmlGenerator);
				it4++;
			}
		}
		cout<<"BLUE-END"<<endl;
		plik5<<testBadStops()<<endl;
		bool rss=false;
		if(miasto=="Warszawa")
			rss=true;
		htmlTile(plik5, rss);
		plik5.close();
		cout<<"ZZZZ-END"<<endl;
		uzupelnij(nowyPlik5, pathTemplate+"/theme2.template");
		cout<<"GGG-END"<<endl;

		nowyPlik5.close();
	}
	~galk()
	{
		delete bazaOsm;
		delete bazaZtm;
	}
	string testBadStops()
	{
		stringstream wynik;
		for(auto& ddd : osmStopData)
		{
			OsmStopData& dataOsm = ddd.second;
			string id = ddd.first;
			if(bazaZtm->przystanki.find(id)==bazaZtm->przystanki.end())
			{
				if(id.length()==6 && (id[4]=='5' || id[4]=='6'))
				{
					double lat = 0;
					double lon = 0;
					if(bazaOsm->nodes.find(dataOsm.stop_position)!=bazaOsm->nodes.end())
					{
						lat = bazaOsm->nodes[dataOsm.stop_position].lat;
						lon = bazaOsm->nodes[dataOsm.stop_position].lon;
					}
					else if(bazaOsm->nodes.find(dataOsm.bus_stop)!=bazaOsm->nodes.end())
					{
						lat = bazaOsm->nodes[dataOsm.bus_stop].lat;
						lon = bazaOsm->nodes[dataOsm.bus_stop].lon;
					}
					wynik<<htmlgen::div("bprzyst", "", wypiszPrzystanekDoUsuniecia(id, dataOsm.name, lon, lat))<<endl;
				}
			}
		}
		return wynik.str();
	}
};
int main(int argc, char** argv)
{
	galk foo(argv);
}
