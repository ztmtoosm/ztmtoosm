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
#include "WlasciwosciLokalne.hpp"
using namespace std;

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
	set <string> prawidlowe;
	map <string, set<string> > nieprawidlowe;
	ztmread_for_html* bazaZtm;
	map <string, OsmStopData>* osmStops;
	set <string> sprawdzLinie(string linia, vector <vector <string> > drugi)
	{
		set <string> wynik;
		int s9=drugi.size();
		for(int i=0; i<s9; i++)
		{
			int s8=(drugi)[i].size();
			for(int j=0; j<s8; j++)
			{
				OsmStopData data=(*osmStops)[drugi[i][j]];
				if(data.stop_position==0)
				{
					wynik.insert(drugi[i][j]);
				}
			}
		}
		return wynik;
	}
	public:
	set <string> getPrawidlowe()
	{
		return prawidlowe;
	}
	set <string> getNieprawidlowe()
	{
		set <string> wynik;
		for(auto& it1 : nieprawidlowe)
		{
			if(it1.first.size()>0)
				wynik.insert(it1.first);
		}
		return wynik;
	}
	map <string, set<string> > getNieprawidloweMap()
	{
		return nieprawidlowe;
	}
	PrzegladanieCzyPrawidloweNoweLinie(map<string, OsmStopData>* osmStopsW, ztmread_for_html* bazaZtmW, set<string> doPrzerobienia)
	{
		osmStops = osmStopsW;
		bazaZtm = bazaZtmW;
		for(auto& it1 : doPrzerobienia)
		{
			set <string> wynTmp = sprawdzLinie(it1, bazaZtm->dane_linia[it1]);
			if(wynTmp.size()==0)
			{
				prawidlowe.insert(it1);
			}
			else
			{
				nieprawidlowe[it1]=wynTmp;
			}
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

struct SpecialSortedString
{
	string str;
	bool operator < (const SpecialSortedString& rhs) const
	{
		string l=str;
		string r=rhs.str;
		return zeraWiodace(l)<zeraWiodace(r);
	}
	bool operator == (const SpecialSortedString& rhs) const
	{
		string l=str;
		string r=rhs.str;
		return l==r;
	}
	static set <SpecialSortedString> convertSet(set<string> normal)
	{
		set <SpecialSortedString> wynik;
		for(auto it1 : normal)
		{
			SpecialSortedString foo;
			foo.str=it1;
			wynik.insert(foo);
		}
		return wynik;
	}
	static set <string> convertToNormal(set<SpecialSortedString> extra)
	{
		set <string> wynik;
		for(auto it1 : extra)
		{
			wynik.insert(it1.str);
		}
		return wynik;
	}
};


class OsmBazaLoaderBis
{
	set <pair<string, string> > dodane;
	vector <long long> bfs(long long start, long long stop, map <long long, vector <long long> >& prepareBfs)
	{
		vector <long long> wynik;
		vector <long long> kolejka;
		int poz=0;
		kolejka.push_back(start);
		set <long long> odwiedzone;
		odwiedzone.insert(start);
		map <long long, long long> ojc;
		bool ok=1;
		while(poz!=kolejka.size() && ok)
		{
			vector <long long> nachte=prepareBfs[kolejka[poz]];
			for(int i=0; i<nachte.size(); i++)
			{
				if(odwiedzone.find(nachte[i])==odwiedzone.end())
				{
					kolejka.push_back(nachte[i]);
					ojc[nachte[i]]=kolejka[poz];
					odwiedzone.insert(nachte[i]);
					if(nachte[i]==stop)
						ok=0;
				}
			}
			poz++;
		}
		long long akt=stop;
		while(ojc.find(akt)!=ojc.end())
		{
			wynik.push_back(akt);
			akt=ojc[akt];
		}
		wynik.push_back(start);
		reverse(wynik.begin(), wynik.end());
		return wynik;
	}

	void easyNodes(long long rel, osm_base* base, fstream& plikOut)
	{
		relation akt_rel = base->relations[rel];
		map <long long, vector <long long> > prepareBfs;
		map <string, long long> refs;
		for(int i=0; i<akt_rel.members.size(); i++)
		{
			if(akt_rel.members[i].member_type==WAY)
			{
				long long way_id=akt_rel.members[i].member_id;
				if(akt_rel.members[i].role!="platform")
				if(akt_rel.members[i].role!="platform_entry_only")
				if(akt_rel.members[i].role!="platform_exit_only")
				{
					vector <long long> nods=base->ways[way_id].nodes;
					for(int j=0; j<nods.size()-1; j++)
					{
						prepareBfs[nods[j]].push_back(nods[j+1]);
						prepareBfs[nods[j+1]].push_back(nods[j]);
					}
					for(int j=0; j<nods.size(); j++)
					{
						if(base->nodes[nods[j]].getTags()["public_transport"]=="stop_position")
						{
							refs[base->nodes[nods[j]].getTags()["ref"]]=nods[j];
						}
					}
				}
			}
		}
		vector <string> ref1;
		for(int i=0; i<akt_rel.members.size(); i++)
		{
			if(akt_rel.members[i].member_type==NODE)
			{
				bool ok=0;
				long long node_id=akt_rel.members[i].member_id;
				if(akt_rel.members[i].role=="stop")
					ok=1;
				if(akt_rel.members[i].role=="stop_entry_only")
					ok=1;
				if(akt_rel.members[i].role=="stop_exit_only")
					ok=1;
				if(ok && base->nodes[node_id].getTags()["ref"]!="")
				{
					string ref=base->nodes[node_id].getTags()["ref"];
					ref1.push_back(ref);
				}
			}
		}
		for(int i=0; i<ref1.size()-1; i++)
		{
			vector <long long> bfstmp=bfs(refs[ref1[i]], refs[ref1[i+1]], prepareBfs);
			if(dodane.find(pair<string, string> (ref1[i], ref1[i+1]))==dodane.end())
			{
				dodane.insert(pair<string,string>(ref1[i], ref1[i+1]));
				for(int j=0; j<bfstmp.size(); j++)
				{
					double lat=base->nodes[bfstmp[j]].lat;
					double lon=base->nodes[bfstmp[j]].lon;
					plikOut<<ref1[i]<<"	"<<ref1[i+1]<<"	"<<j+1<<"	"<<lat<<"	"<<lon<<endl;
				}

			}
		}
	}
	public:
	OsmBazaLoaderBis(vector <long long>& rels, osm_base* bazaOsm, string sciezka_out)
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

struct MainClass
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
		if(typ==2)
			gen.loadedVariables[0]="panel-success panel-linia";
		if(typ==1)
			gen.loadedVariables[0]="panel-warning panel-linia";
		if(typ==0)
			gen.loadedVariables[0]="panel-info panel-linia";
		gen.loadedVariables[1]=name;
		gen.loadedVariables[2]=content;
		plik<<gen.loadTemplate(pathTemplate+"/lineHeader.template");
		cout<<"LOADER STOP"<<endl;
	}
	string dodajInfoNiewygenerowane(set <string> errPrzyst, string linia, HtmlExtraGenerator& gen)
	{
		stringstream foo1;
		foo1<<errPrzyst.size();
		string message = "Brak stop_position dla przystanków: ";
		int licznik = 0;
		for(auto it1 : errPrzyst)
		{
			if(licznik>0)
				message+=", ";
			message+=htmlgen::link(miasto+".html#"+it1, bazaZtm->przystanki[it1].name+" ("+it1+")", "");
			licznik++;
		}
		gen.loadedVariables[0]=linia+"niewygenerowane";
		gen.loadedVariables[1]=message;
		gen.loadedVariables[2]=foo1.str();
		return gen.loadTemplate(pathTemplate+"/errLine.template");
	}
	string dodajInfoRoznice(set <string> ein, set <string> zwei, bool brakRelacji, string linia, HtmlExtraGenerator& gen)
	{
		stringstream foo1;
		foo1<<ein.size()+zwei.size()+(int)(brakRelacji);
		string message = "Przystanki na których nie zatrzymuje się linia, ale OSM twierdzi inaczej: ";
		int licznik = 0;
		if(ein.size()==0)
			message+="brak";
		for(auto it1 : ein)
		{
			if(licznik>0)
				message+=", ";
			message+=htmlgen::link(miasto+".html#"+it1, osmStopData[it1].name+" ("+it1+")", "");
			licznik++;
		}
		message += ". Przystanki na których zatrzymuje się linia nieuwzględnione w OSM: ";
		licznik = 0;
		for(auto it1 : zwei)
		{
			if(licznik>0)
				message+=", ";
			message+=htmlgen::link(miasto+".html#"+it1, bazaZtm->przystanki[it1].name+" ("+it1+")", "");
			licznik++;
		}
		if(zwei.size()==0)
			message+="brak";
		message+=". ";
		if(brakRelacji)
			message += "Niespójność/brak relacji.";
		gen.loadedVariables[0]=linia+"roznice";
		gen.loadedVariables[1]=message;
		gen.loadedVariables[2]=foo1.str();
		return gen.loadTemplate(pathTemplate+"/diffLine.template");
	}
	string dodajInfoNormalne(pair <long long, vector <long long> > daneLinia, string linia, HtmlExtraGenerator& gen, set<long long>& niespojne)
	{
		stringstream messageStream;
		messageStream<<"<li class=\"list-group-item\">";
		messageStream<<"route_master: ";
		if(daneLinia.first!=0)
			messageStream<<htmlgen::link("http://openstreetmap.org/relation/"+toXstring(daneLinia.first), toXstring(daneLinia.first), "");
		else
			messageStream<<" <span class=\"label label-danger\">BRAK</span>";
		messageStream<<"</li>";
		for(int i=0; i<daneLinia.second.size(); i++)
		{
			messageStream<<"<li class=\"list-group-item\">";
			messageStream<<"route: ";
			messageStream<<htmlgen::link("http://openstreetmap.org/relation/"+toXstring(daneLinia.second[i]), toXstring(daneLinia.second[i]), "");
			if(niespojne.find(daneLinia.second[i])!=niespojne.end())
			{
				messageStream<<" <span class=\"label label-danger\">NIESPÓJNA</span>";
			}
			messageStream<<" <a class=\"label label-info\" href=\""<<"http://analyser.openstreetmap.fr/cgi-bin/index.py?relation="<<daneLinia.second[i]<<"\">RA</a>";
			messageStream<<"</li>";
		}
		gen.loadedVariables[0]=linia+"info";
		gen.loadedVariables[1]=messageStream.str();
		return gen.loadTemplate(pathTemplate+"/infoLine.template");
	}
	string dodajProgress(int val1, int val2, int val3, HtmlExtraGenerator& gen)
	{
		double suma = val1+val2+val3;
		double vall1 = val1;
		double vall2 = val2;
		double vall3 = val3;
		vall1/=suma/100.0;
		vall2/=suma/100.0;
		vall3/=suma/100.0;
		gen.loadedVariables[0]=toXstring(vall1);
		gen.loadedVariables[1]=toXstring(vall2);
		gen.loadedVariables[2]=toXstring(vall3);
		return gen.loadTemplate(pathTemplate+"/progress.template");
	}
	string aktTime()
	{
		char buff[20];
		time_t now = time(NULL);
		strftime(buff, 20, "%d.%m.%Y %H:%M", localtime(&now));
		string buff2=buff;
		return buff2;
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


	string divOsmTable(vector <string> elem)
	{
		stringstream foo;
		foo<<"<table class=\"table table-striped\">";
		for(int i=0; i<elem.size(); i++)
		{
			foo<<elem[i];
		}
		foo<<"</table>";
		return foo.str();
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
			return htmlgen::div("komorka", "", "-");
		stringstream foo;
		foo<<"<a href=\"http://openstreetmap.org/"<<type<<"/"<<id<<"\">"<<type<<" "<<id<<"</a>";
		return htmlgen::div("komorka", "", foo.str());
	}
	string dokladnePrzystanki(string idPrzystanek, string idLinia, int idWariantu, int idKol)
	{
		string poprzedni = "POCZĄTKOWY";
		string kolejny = "KOŃCOWY";
		string poprzedniId, kolejnyId;
		if(idKol>0)
		{
			poprzedniId = bazaZtm->dane_linia[idLinia][idWariantu][idKol-1];
			poprzedni = bazaZtm->przystanki[poprzedniId].name;
		}
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+1)
		{
			kolejnyId = bazaZtm->dane_linia[idLinia][idWariantu][idKol+1];
			kolejny = bazaZtm->przystanki[kolejnyId].name;
		}
		string aktualny = bazaZtm->przystanki[idPrzystanek].name;;
		string ostatniId = bazaZtm->dane_linia[idLinia][idWariantu][bazaZtm->dane_linia[idLinia][idWariantu].size()-1];
		string ostatni = bazaZtm->przystanki[ostatniId].name;
		string pierwszyId = bazaZtm->dane_linia[idLinia][idWariantu][0];
		string pierwszy = bazaZtm->przystanki[pierwszyId].name;
		string info=htmlgen::link("Pelne"+miasto+".html#pelne"+idLinia, idLinia);
		info+=": ";
		if(idKol>1)
		{
			info+=htmlgen::link("#"+pierwszyId, pierwszy+" ("+pierwszyId+")");
			info+=" - ";
		}
		if(idKol>2)
		{
			info+="... - ";
		}
		if(idKol>0)
		{
			info+=htmlgen::link("#"+poprzedniId, poprzedni+" ("+poprzedniId+")");
			info+=" - ";
		}
		info+=aktualny+" ("+idPrzystanek+")";
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+1)
		{
			info+=" - ";
			info+=htmlgen::link("#"+kolejnyId, kolejny+" ("+kolejnyId+")");
		}
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+3)
		{
			info+=" - ...";
		}
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+2)
		{
			info+=" - ";
			info+=htmlgen::link("#"+ostatniId, ostatni+" ("+ostatniId+")");
		}
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
		return wynik;
	}

	std::string escapeJsonString(const std::string& input) {
		std::ostringstream ss;
		for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
		//C++98/03:
		//for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
			switch (*iter) {
				case '\\': ss << "\\\\"; break;
				case '"': ss << "\\\""; break;
				case '/': ss << "\\/"; break;
				case '\b': ss << "\\b"; break;
				case '\f': ss << "\\f"; break;
				case '\n': ss << "\\n"; break;
				case '\r': ss << "\\r"; break;
				case '\t': ss << "\\t"; break;
				default: ss << *iter; break;
			}
		}
		return ss.str();
	}

	string wyszName(char type, long long id)
	{
		if(type=='N' && bazaOsm->nodes.find(id)!=bazaOsm->nodes.end())
		{
			return bazaOsm->nodes[id].getTags()["name"];
		}
		if(type=='W' && bazaOsm->nodes.find(id)!=bazaOsm->nodes.end())
		{
			return bazaOsm->ways[id].getTags()["name"];
		}
		if(type=='R' && bazaOsm->nodes.find(id)!=bazaOsm->nodes.end())
		{
			return bazaOsm->relations[id].getTags()["name"];
		}
		return "";
	}

	bool getLatLon(char type, long long id, double& lat, double& lon)
	{
		if(type=='R' && bazaOsm->relations.find(id)!=bazaOsm->relations.end())
		{
			relation& r = bazaOsm->relations[id];
			for(int i=0; i<r.members.size(); i++)
			{
				if(r.members[i].member_type==WAY)
				{
					id=r.members[i].member_id;
					type='W';
				}
			}
		}

		if(type=='W' && bazaOsm->ways.find(id)!=bazaOsm->ways.end())
		{
			way& r = bazaOsm->ways[id];
			for(int i=0; i<r.nodes.size(); i++)
			{
				id=r.nodes[i];
				type='N';
			}
		}

		if(type=='N' && bazaOsm->nodes.find(id)!=bazaOsm->nodes.end())
		{
			lat = bazaOsm->nodes[id].lat;
			lon = bazaOsm->nodes[id].lon;
			return true;
		}
		return false;
	}

	double getDistance(char type1, long long id1, char type2, long long id2)
	{
		double lat1, lon1;
		double lat2, lon2;
		bool t1 = getLatLon(type1, id1, lat1, lon1);
		bool t2 = getLatLon(type2, id2, lat2, lon2);
		if(t1 && t2)
		{
			return distance(lon1, lat1, lon2, lat2)*1000.0;
		}
		return -1.0;
	}

	bool getWsp(double& lat, double& lon, char type1, long long id1, char type2, long long id2)
	{
		bool t1 = getLatLon(type1, id1, lat, lon);
		if(t1)
			return true;
		bool t2 = getLatLon(type2, id2, lat, lon);
		if(t2)
			return true;
		return false;
	}

	MainClass(char** argv)
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
		if(miasto=="Łódź")
			wlasciwosci = new WlasciwosciLokalneLodz();
		bazaOsm = new osm_base(osmBasePath);
		cerr<<"BAZA OSM - OK"<<endl;
		osmStopData = loadOsmStopData(bazaOsm, wlasciwosci->getRefKey());
		cerr<<"OSM-DATA - OK"<<endl;
		bazaZtm = new ztmread_for_html (ztmBasePath, miasto);
		cerr<<"Załadowano dane "<<bazaZtm<<endl;
		if(czyWszystkie)
			linieDoPrzerobienia=wlasciwosci->wszystkieLinie(bazaZtm);
		PrzegladanieCzyPrawidloweStareLinie przegl0(bazaOsm, bazaZtm, linieDoPrzerobienia, wlasciwosci->getRootRelation(), wlasciwosci->getRefKey());
		cerr<<"etap1 "<<endl;
		set <string> etap=przegl0.nieprawidlowe;
		if(!czyWszystkie)
			etap=linieDoPrzerobienia;
		cerr<<"etap2 "<<endl;
		PrzegladanieCzyPrawidloweNoweLinie przegl(&osmStopData, bazaZtm, etap);
		cerr<<"etap3 "<<endl;
		linieDoPrzerobienia=przegl.getPrawidlowe();
		string linieHTMLPath=pathHTML+"/Pelne"+miasto+"bis.html";
		string przystankiHTMLPath=pathHTML+"/"+miasto+"bis.html";
		string jsonPath=pathHTML+"/List"+miasto+".json";
		string json2Path=pathHTML+"/Przystanki"+miasto+".json";
		string wspRoutePath=pathHTML+"/Trasy"+miasto+".txt";
		fstream lineHTMLStream(linieHTMLPath.c_str(), ios::out | ios::trunc);
		fstream przystankiHTMLStream(przystankiHTMLPath.c_str(), ios::out | ios::trunc);
		fstream jsonStream(jsonPath.c_str(), ios::out | ios::trunc);
		fstream json2Stream(json2Path.c_str(), ios::out | ios::trunc);

		lineHTMLStream.precision(9);
		json2Stream.precision(9);
		uzupelnij(lineHTMLStream, pathTemplate+"/theme.template");
		lineHTMLStream<<miasto<<" - linie";

		htmlGenerator.loadedVariables[0]=miasto+".html";
		htmlGenerator.loadedVariables[1]="Zestawienie przystanków";
		lineHTMLStream<<htmlGenerator.loadTemplate(pathTemplate+"/themeA.template");

		lineHTMLStream<<"Stan na: ";
		lineHTMLStream<<aktTime();
		uzupelnij(lineHTMLStream, pathTemplate+"/themeB.template");

		przystankiHTMLStream.precision(9);
		uzupelnij(przystankiHTMLStream, pathTemplate+"/theme.template");
		przystankiHTMLStream<<miasto<<" - przystanki";

		htmlGenerator.loadedVariables[0]="Pelne"+miasto+".html";
		htmlGenerator.loadedVariables[1]="Zestawienie linii";
		przystankiHTMLStream<<htmlGenerator.loadTemplate(pathTemplate+"/themeA.template");
		przystankiHTMLStream<<"Stan na: ";
		przystankiHTMLStream<<aktTime();
		uzupelnij(przystankiHTMLStream, pathTemplate+"/themeB.template");


		/*
		stringstream p5_tmp, p6_tmp, p7_tmp;
		for(string it1 : blednePrzystanki)
		{
			p5_tmp<<htmlgen::div("bprzyst", "", wypiszBlednyPrzystanek(it1))<<endl;
		}
		plik5<<htmlgen::div("blstops", "", p5_tmp.str())<<endl;
		plik5<<htmlgen::div("partx", "", "Linie do usunięcia")<<endl;
		*/

		/*
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
		*/
		lineHTMLStream<<dodajProgress(linieDoPrzerobienia.size(), przegl.getNieprawidlowe().size(), przegl0.prawidlowe.size(), htmlGenerator)<<endl;

		auto doUsuniecia = linieDoUsuniecia(bazaZtm, bazaOsm, wlasciwosci->getRootRelation());
		if(doUsuniecia.size()>0)
		{
			stringstream p6_tmp;
			lineHTMLStream<<"<h2>Linie do usunięcia <span class=\"badge\">"<<doUsuniecia.size()<<"</span></h2>";
			for(auto& it1 : doUsuniecia)
			{
				p6_tmp<<it1<<" ";
			}
			string p6_xxx = p6_tmp.str();
			lineHTMLStream<<htmlgen::div("do_usuniecia", "", p6_xxx)<<endl;
		}

		jsonStream<<"[";
		int licznikx=0;
		auto linieDoPrzerobieniaSorted = SpecialSortedString::convertSet(linieDoPrzerobienia);
		lineHTMLStream<<"<h2>Linie wygenerowane <span class=\"badge\">"<<linieDoPrzerobieniaSorted.size()<<"</span></h2>";
		for(auto it1 : linieDoPrzerobieniaSorted)
		{
			if(licznikx>0)
				jsonStream<<",";
			jsonStream<<"\""<<it1.str<<"\"";
			generujLinie(it1.str);
			string message1 = dodajInfoNormalne(przegl0.relacjeDlaLinii[it1.str], it1.str, htmlGenerator, przegl0.badRelations);
			message1 += dodajInfoRoznice(przegl0.onlyOsmStop[it1.str], przegl0.onlyZtmStop[it1.str], (przegl0.badLines.find(it1.str)!=przegl0.badLines.end()), it1.str, htmlGenerator);
			dodajLinieDoHTML(lineHTMLStream, 2, it1.str, message1, htmlGenerator);
			licznikx++;
		}
		jsonStream<<"]";
		jsonStream.close();
		auto linieNiewygenerowaneSorted = SpecialSortedString::convertSet(przegl.getNieprawidlowe());
		auto linieNiewygenerowaneMap = przegl.getNieprawidloweMap();
		lineHTMLStream<<"<h2>Linie niewygenerowane <span class=\"badge\">"<<linieNiewygenerowaneSorted.size()<<"</span></h2>";
		for(auto it1 : linieNiewygenerowaneSorted)
		{
			set<string> errPrzyst = linieNiewygenerowaneMap[it1.str];
			string message1 = dodajInfoNormalne(przegl0.relacjeDlaLinii[it1.str], it1.str, htmlGenerator, przegl0.badRelations);
			message1 += dodajInfoRoznice(przegl0.onlyOsmStop[it1.str], przegl0.onlyZtmStop[it1.str], (przegl0.badLines.find(it1.str)!=przegl0.badLines.end()), it1.str, htmlGenerator);
			message1 += dodajInfoNiewygenerowane(errPrzyst, it1.str, htmlGenerator);
			dodajLinieDoHTML(lineHTMLStream,1, it1.str, message1, htmlGenerator);
		}

		if(czyWszystkie)
		{
			auto linieNormalneSorted = SpecialSortedString::convertSet(przegl0.prawidlowe);
			lineHTMLStream<<"<h2>Linie bez zmian <span class=\"badge\">"<<linieNormalneSorted.size()<<"</span></h2>";
			for(auto it1 : linieNormalneSorted)
			{
				string message1 = dodajInfoNormalne(przegl0.relacjeDlaLinii[it1.str], it1.str, htmlGenerator, przegl0.badRelations);
				dodajLinieDoHTML(lineHTMLStream, 0, it1.str, message1, htmlGenerator);
			}
		}
		/*
		cout<<"BLUE-END"<<endl;
		plik5<<testBadStops()<<endl;
		bool rss=false;
		if(miasto=="Warszawa")
			rss=true;
		htmlTile(plik5, rss);
		plik5.close();
		cout<<"ZZZZ-END"<<endl;
		*/
		int jsonTableRowCount = 0;
		json2Stream.precision(9);
		json2Stream<<"[";
		for(auto& it1 : osmStopData)
		{
			if(bazaZtm->przystanki.find(it1.first)!=bazaZtm->przystanki.end())
			{
				int powod = 1000;
				if(it1.second.stop_position==0)
					powod = 1;
				auto& it2 = bazaZtm->przystanki[it1.first];
				stringstream line;
				line.precision(9);
				if(jsonTableRowCount>0)
					line<<",";
				line<<"{";
				line<<"\"id\":\""<<it1.first<<"\"";
				line<<",\"name\":\""<<escapeJsonString(it2.name)<<"\"";
				line<<",\"lon\": "<<it2.lon<<" ";
				line<<",\"lat\": "<<it2.lat<<" ";
				line<<",\"latlon_jakosc\": "<<it2.wsp_jakosc<<" ";
				double lat2, lon2;
				bool okWsp2 = getWsp(lat2, lon2, 'N', it1.second.stop_position, 'N', it1.second.bus_stop);
				if(okWsp2)
				{
					line<<",\"lon2\": "<<lon2<<" ";
					line<<",\"lat2\": "<<lat2<<" ";
				}
				line<<",\"kierunki\":[";
				vector <string> kierunki=przystanekKierunki(it1.first);
				for(int i=0; i<kierunki.size(); i++)
				{
					if(i>0)
						line<<",";
					line<<"\""<<escapeJsonString(kierunki[i])<<"\"";
				}
				line<<"]";
				line<<",\"bus_stop\":"<<it1.second.bus_stop<<" ";
				line<<",\"bus_stop_name\":\""<<escapeJsonString(wyszName('N', it1.second.bus_stop))<<"\"";
				line<<",\"stop_position\": "<<it1.second.stop_position<<" ";
				line<<",\"stop_position_name\":\""<<escapeJsonString(wyszName('N', it1.second.stop_position))<<"\"";
				line<<",\"platform\": "<<it1.second.platform<<" ";
				line<<",\"platform_name\":\""<<escapeJsonString(wyszName(it1.second.platform_type, it1.second.platform))<<"\"";
				line<<",\"additional\":\""<<escapeJsonString(it2.stopinfo+" ; "+it2.miejscowosc)<<"\"";
				line<<",\"BS_SP\": "<<getDistance('N', it1.second.bus_stop, 'N', it1.second.stop_position);
				if(it1.second.bus_stop!=0 && powod==1000 && getDistance('N', it1.second.bus_stop, 'N', it1.second.stop_position)>150)
					powod = 3;
				line<<",\"SP_PL\": "<<getDistance(it1.second.platform_type, it1.second.platform, 'N', it1.second.stop_position);
				line<<",\"PL_BS\": "<<getDistance('N', it1.second.bus_stop, it1.second.platform_type, it1.second.platform);
				line<<",\"powod\": "<<powod;
				if(it1.second.platform_type=='N' || it1.second.platform_type=='W' || it1.second.platform_type=='R')
					line<<",\"platform_type\":\""<<it1.second.platform_type<<"\"";

				/*
				line<<"<tr id=\""<<it1.first<<"\">";
				line<<"<td>"<<it1.first<<"</td>";
				line<<"<td>"<<it2.name<<"</td>";
				line<<"<td>"<<it2.lon<<"</td>";
				line<<"<td>"<<it2.lat<<"</td>";
				line<<"<td>"<<it2.stopinfo<<"</td>";
				line<<"<td>"<<divOsmLink(it1.second.bus_stop, 'N')<<"</td>";
				line<<"<td>"<<divOsmLink(it1.second.stop_position, 'N')<<"</td>";
				line<<"<td>"<<divOsmLink(it1.second.platform, it1.second.platform_type)<<"</td>";
				vector <string> kierunki=przystanekKierunki(it1.first);
				line<<"<td>"<<kierunki[0]<<"</td>";
				line<<"<td>"<<kierunki[1]<<"</td>";
				line<<"<td>"<<kierunki[2]<<"</td>";*/
				/*
				string refDiv = htmlgen::div("komorka", "", it1.first);
				string refName = htmlgen::div("komorka", "", it1.second.name);
				string k1 = htmlgen::div("komorka", "", kierunki[0]);
				string k2 = htmlgen::div("komorka", "", kierunki[1]);
				string k3 = htmlgen::div("komorka", "", kierunki[2]);
				string row[] = {refDiv, refName, divOsmLink(it1.second.bus_stop, 'N'), divOsmLink(it1.second.stop_position, 'N'), divOsmLink(it1.second.platform, it1.second.platform_type), k1, k2, k3};
				*/
				line<<"}"<<endl;
				json2Stream<<line.str();
				jsonTableRowCount++;
			}
		}

		for(auto& it2 : bazaZtm->przystanki)
		{
			if(osmStopData.find(it2.first)==osmStopData.end())
			{
				int powod = 1000;
				stringstream line;
				if(jsonTableRowCount>0)
					line<<",";
				line<<"{";
				line<<"\"id\":\""<<it2.first<<"\"";
				line<<",\"name\":\""<<escapeJsonString(it2.second.name)<<"\"";
				line<<",\"lon\": "<<it2.second.lon<<" ";
				line<<",\"lat\": "<<it2.second.lat<<" ";
				line<<",\"latlon_jakosc\": "<<it2.second.wsp_jakosc<<" ";
				line<<",\"powod\": "<<powod;
				line<<",\"kierunki\":[";
				vector <string> kierunki=przystanekKierunki(it2.first);
				for(int i=0; i<kierunki.size(); i++)
				{
					if(i>0)
						line<<",";
					line<<"\""<<escapeJsonString(kierunki[i])<<"\"";
				}
				line<<"]";
				line<<"}"<<endl;
				json2Stream<<line.str();
				jsonTableRowCount++;
			}
		}

		//przystankiHTMLStream<<divOsmTable(tabela)<<endl;
		json2Stream<<"]";
		uzupelnij(lineHTMLStream, pathTemplate+"/theme2.template");
		if(miasto=="Warszawa")
			lineHTMLStream<<"<script src=\"jq.js\"></script><script src=\"jfeed.js\"></script><script src=\"rss-test.js\"></script>";
		lineHTMLStream<<"</body></html>";
		lineHTMLStream.close();
		json2Stream.close();

		htmlGenerator.loadedVariables[0]=miasto;
		//przystankiHTMLStream<<htmlGenerator.loadTemplate(pathTemplate+"/theme5.template");

		uzupelnij(przystankiHTMLStream, pathTemplate+"/theme3.template");
		przystankiHTMLStream<<"Przystanki"<<miasto<<".json";
		uzupelnij(przystankiHTMLStream, pathTemplate+"/theme4.template");
		przystankiHTMLStream.close();
		vector <long long> rels;
		for(auto& it1 : przegl0.relacjeDlaLinii)
		{
			for(auto& it2 : it1.second.second)
			{
				rels.push_back(it2);
			}
		}
		OsmBazaLoaderBis(rels, bazaOsm, wspRoutePath);
	}
	~MainClass()
	{
		delete bazaOsm;
		delete bazaZtm;
	}
};




int main(int argc, char** argv)
{
	MainClass foo(argv);
}
