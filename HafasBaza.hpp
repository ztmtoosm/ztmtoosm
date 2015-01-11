#include "readztm.hpp"
#include "osm_base.hpp"
#include "starelinie.hpp"
struct wspolrzedne
{
	double lon;
	double lat;
};

struct OneStopRecord
{
	int time;
	string linia;
	int kurs;
	int poz;
};

bool srt(OneStopRecord& alfa, OneStopRecord& beta)
{
	return alfa.time>beta.time;
}

struct HafasStop
{
	string id;
	string name;
	wspolrzedne wspol;
	string miejscowosc;
	vector <OneStopRecord> recordy;
};

struct HafasKurs
{
	vector <pair <string, int> > postoje;
};

struct DijPoprzednik
{
	string poprz;
	OneStopRecord record;
	int endpoint;
};

struct DijData
{
	set <string> odwiedzone;
	set <pair<int, string> > kandydaci1;
	map <string, int> kandydaci2;
	map <string, DijPoprzednik> poprzednik;
	void update(string node, DijPoprzednik pop, int time)
	{
		if(odwiedzone.find(node)!=odwiedzone.end())
			return;
		if(kandydaci2.find(node)!=kandydaci2.end())
		{
			int oldtime=kandydaci2[node];
			if(oldtime>time)
			{
				kandydaci1.erase(kandydaci1.find(pair<int, string>(oldtime, node)));
				kandydaci2[node]=time;
				poprzednik[node]=pop;
				kandydaci1.insert(pair<int, string>(time, node));
			}
		}
		else
		{
				kandydaci2[node]=time;
				poprzednik[node]=pop;
				kandydaci1.insert(pair<int, string>(time, node));
		}
	}
	int wstaw(string node, int time)
	{
		kandydaci1.insert(pair<int, string>(time, node));
		kandydaci2[node]=time;
	}
	int odwiedz(string node, int ewstart)
	{
		int oldtime=ewstart;
		if(kandydaci2.find(node)!=kandydaci2.end())
		{
			oldtime=kandydaci2[node];
			if(kandydaci1.find(pair<int, string>(oldtime, node))!=kandydaci1.end())
				kandydaci1.erase(kandydaci1.find(pair<int, string>(oldtime, node)));
			kandydaci2.erase(node);
		}
		odwiedzone.insert(node);
		return oldtime;
	}
	vector <DijPoprzednik> poprzednicy(string start)
	{
		vector <DijPoprzednik> wynik;
		while(poprzednik.find(start)!=poprzednik.end())
		{
			wynik.push_back(poprzednik[start]);
			start=poprzednik[start].poprz;
		}
		reverse(wynik.begin(), wynik.end());
		return wynik;
	}
};

struct HafasLinia
{
	string id;
	vector <HafasKurs> kursy;
	vector <vector <string> > trasy;
};

class HafasBazaLoader;

class HafasBaza
{
	friend class HafasBazaLoader;
	public:
	HafasBaza() {};
	//private:
	map <string, HafasLinia> linie;
	map <string, HafasStop> przystanki;
	map <pair<string, string>, vector <wspolrzedne> > laczniki;
	void sortRecordy()
	{
		auto it1=przystanki.begin();
		while(it1!=przystanki.end())
		{
			sort((it1->second).recordy.begin(), (it1->second).recordy.end(), srt);
			it1++;
		}
	}
	int binsearch(vector <OneStopRecord> recordy, int time)
	{
		int left=0;
		int right=recordy.size()-1;
		while(right-left>2)
		{
			if(recordy[(left+right)/2].time<time)
			{
				left=(left+right)/2;
			}
			else
			{
				right=(left+right)/2;
			}
		}
		return left;
	}
	vector <DijPoprzednik> dijkstra(int time, string start, string stop)
	{
		DijData dix;
		dix.wstaw(start, time);
		while(!dix.kandydaci1.empty() && dix.odwiedzone.find(stop)==dix.odwiedzone.end())
		{
			string akt=dix.kandydaci1.begin()->second;
			cout<<"dijk "<<akt<<endl;
			int akttime=dix.odwiedz(akt, time);
			vector <OneStopRecord> recordy=przystanki[akt].recordy;
			bool ok=1;
			for(int i=binsearch(recordy, akttime); i<recordy.size() && ok; i++)
			{
				if(recordy[i].time>akttime)
				{
					string linia=recordy[i].linia;
					int time=recordy[i].time;
					int kurs=recordy[i].kurs;
					int poz=recordy[i].poz;
					auto postoje=linie[linia].kursy[kurs].postoje;
					for(int j=poz+1; j<postoje.size(); j++)
					{
						DijPoprzednik pop;
						pop.poprz=akt;
						pop.endpoint=j;
						pop.record=recordy[i];
						dix.update(postoje[j].first, pop, postoje[j].second+1);
					}
					if(recordy[i].time>akttime*4*60*60)
						ok=0;
				}
			}
		}
		return dix.poprzednicy(stop);
	}
	void dijkstra(string start, string stop, int time, ostream& strim)
	{
		vector <DijPoprzednik> dll=dijkstra(time, start, stop);
		vector <pair<string, int> > kurr;
		vector <pair<int, int> > zakresy;
		for(int i=0; i<dll.size(); i++)
		{
			kurr.push_back(pair<string, int>(dll[i].record.linia, dll[i].record.kurs));
			zakresy.push_back(pair<int, int>(dll[i].record.poz, dll[i].endpoint));
		}
		wypiszKurs(kurr, zakresy, strim);
	}
	void wypiszPartRoute(int starttime, int stoptime, string start, string stop, ostream& strim, bool ostatni)
	{
		vector <wspolrzedne> obrobka;
		if(laczniki.find(pair<string, string>(start, stop))!=laczniki.end())
		{
			obrobka=laczniki[pair<string, string>(start, stop)];
		}
		else
		{
			obrobka.push_back(przystanki[start].wspol);
			obrobka.push_back(przystanki[stop].wspol);
		}
		for(int i=0; i<obrobka.size(); i++)
		{
			if(ostatni || i<obrobka.size()-1)
			{
				strim<<"{";
				strim<<"\"lat\":"<<obrobka[i].lat;
				strim<<", "<<"\"lon\":"<<obrobka[i].lon;
				if(i==0)
				{
					strim<<", "<<"\"name\":\""<<avoid_cudzyslow(przystanki[start].name)<<"\"";
					strim<<", "<<"\"id\":\""<<przystanki[start].id<<"\"";
					if(starttime>0)
						strim<<", "<<"\"time\":"<<starttime;
				}
				if(i==obrobka.size()-1)
				{
					strim<<", "<<"\"name\":\""<<avoid_cudzyslow(przystanki[stop].name)<<"\"";
					strim<<", "<<"\"id\":\""<<przystanki[stop].id<<"\"";
					if(stoptime>0)
						strim<<", "<<"\"time\":"<<stoptime;
				}
				strim<<"}";
				if(i<obrobka.size()-1)
					strim<<",";
			}
		}
	}
	void wypiszRoute(string id, ostream& strim)
	{
		strim<<"[";
		for(int i=0; i<linie[id].trasy.size(); i++)
		{
			strim<<"[";
			for(int j=0; j<linie[id].trasy[i].size()-1; j++)
			{
				bool ostatni=(j==linie[id].trasy[i].size()-2);
				wypiszPartRoute(-1, -1, linie[id].trasy[i][j], linie[id].trasy[i][j+1], strim, ostatni);
			}
			strim<<"]";
			if(i<linie[id].trasy.size()-1)
				strim<<",";
		}
		strim<<"]";
	}
	void wypiszKurs(vector <pair<string, int> >kursy, vector<pair<int, int> >zakresy, ostream& strim)
	{
		strim<<"[";
		for(int i=0; i<kursy.size(); i++)
		{
			strim<<"[";
			vector <pair<string, int> > postoje = linie[kursy[i].first].kursy[kursy[i].second].postoje;
			for(int j=zakresy[i].first; j<zakresy[i].second; j++)
			{
				bool ostatni=(j==zakresy[i].second-1);
				wypiszPartRoute(postoje[j].second, postoje[j+1].second, postoje[j].first, postoje[j+1].first, strim, ostatni);
			}
			strim<<"]";
			if(i<kursy.size()-1)
				strim<<",";
		}
		strim<<"]";
	}
	void wypiszRoute(vector <string> id, ostream& strim)
	{
		strim<<"[";
		for(int g=0; g<id.size(); g++)
		{
			for(int i=0; i<linie[id[g]].trasy.size(); i++)
			{
				strim<<"[";
				for(int j=0; j<linie[id[g]].trasy[i].size()-1; j++)
				{
					bool ostatni=(j==linie[id[g]].trasy[i].size()-2);
					wypiszPartRoute(-1, -1,linie[id[g]].trasy[i][j], linie[id[g]].trasy[i][j+1], strim, ostatni);
				}
				strim<<"]";
				if(i<linie[id[g]].trasy.size()-1 || g<id.size()-1)
					strim<<",";
			}
		}
		strim<<"]";
	}
};

class HafasBazaLoader : ztmread
{
	HafasBaza* baza;
	private:
	void nowy_kurs(kurs nowy)
	{
		HafasKurs nowy2;
		for(int i=0; i<nowy.postoje.size(); i++)
		{
			OneStopRecord recr;
			recr.time=nowy.postoje[i].time;
			recr.linia=nowy.linia;
			recr.kurs=baza->linie[nowy.linia].kursy.size();
			recr.poz=i;
			baza->przystanki[nowy.postoje[i].stop_id].recordy.push_back(recr);
			nowy2.postoje.push_back(pair<string, int>(nowy.postoje[i].stop_id,nowy.postoje[i].time));
		}
		baza->linie[nowy.linia].kursy.push_back(nowy2);
	}
	void nowy_przystanek(przystanek nowy)
	{
		HafasStop nowy2;
		nowy2.name=nowy.name;
		nowy2.id=nowy.id;
		nowy2.miejscowosc=nowy.miejscowosc;
		nowy2.wspol.lon=nowy.lon;
		nowy2.wspol.lat=nowy.lat;
		baza->przystanki[nowy.id]=nowy2;
	}
	void nowa_linia(string nazwa, vector <vector <string> > trasy)
	{
		baza->linie[nazwa].id=nazwa;
		baza->linie[nazwa].trasy=trasy;
	}
	public:
	HafasBazaLoader (HafasBaza* baz, string sciezka) : baza(baz), ztmread(sciezka)
	{
		run();
		baza->sortRecordy();
	}
};
class OsmBazaLoader
{
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
		cout<<"KOLEJNY BFS "<<start<<" "<<stop<<endl;
		return wynik;
	}
	void easyNodes(long long rel, osm_base* base, HafasBaza *base2)
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
						if(base->nodes[nods[j]].tags["public_transport"]=="stop_position")
						{
							refs[base->nodes[nods[j]].tags["ref"]]=nods[j];
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
				if(ok && base->nodes[node_id].tags["ref"]!="")
				{
					string ref=base->nodes[node_id].tags["ref"];
					ref1.push_back(ref);
				}
			}
		}
		for(int i=0; i<ref1.size()-1; i++)
		{
			vector <long long> bfstmp=bfs(refs[ref1[i]], refs[ref1[i+1]], prepareBfs);
			vector <wspolrzedne> wynik;
			for(int j=0; j<bfstmp.size(); j++)
			{
				wspolrzedne foo;
				foo.lat=base->nodes[bfstmp[j]].lat;
				foo.lon=base->nodes[bfstmp[j]].lon;
				wynik.push_back(foo);
			}
			base2->laczniki[pair<string, string>(ref1[i], ref1[i+1])]=wynik;
		}
		cout<<"KOLEJNY KURS "<<rel<<endl;
	}
	void easyNodes(string linia, osm_base* base, HafasBaza *base2)
	{
		vector <long long> relacje=relacje_linia(base, 3651336, linia).second;
		vector <vector <long long> > wynik;
		for(int i=0; i<relacje.size(); i++)
		{
			easyNodes(relacje[i], base, base2);
		}
		cout<<"KOLEJNA LINIA "<<linia<<endl;
	}
	public:
	OsmBazaLoader(HafasBaza* baz, string sciezka)
	{
		osm_base bazaOsm(sciezka);
		auto it1=bazaOsm.nodes.begin();
		while(it1!=bazaOsm.nodes.end())
		{
			if((it1->second).tags["public_transport"]=="stop_position")
			{
				string ref=(it1->second).tags["ref"];
				if(baz->przystanki.find(ref)!=baz->przystanki.end())
				{
					wspolrzedne wsp;
					wsp.lat=(it1->second).lat;
					wsp.lon=(it1->second).lon;
					baz->przystanki[ref].wspol=wsp;
					baz->przystanki[ref].name=(it1->second).tags["name"];
				}
			}
			it1++;
		}
		cout<<"BFS ANALIZA START"<<endl;
		auto it2=baz->linie.begin();
		while(it2!=baz->linie.end())
		{
			easyNodes(it2->first, &bazaOsm, baz);
			it2++;
		}
	}
};
