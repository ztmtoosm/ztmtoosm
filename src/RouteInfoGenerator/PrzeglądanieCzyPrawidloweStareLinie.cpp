#include "PrzegladanieCzyPrawidloweStareLinie.hpp"
using namespace std;

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

set <string> linieDoUsuniecia(ScheduleHandlerInternal* baza_ztm, osm_base* roo, long long root)
{
	set <string> wsadowy;
	for(auto& it1 : baza_ztm->dane_linia)
	{
		wsadowy.insert(it1.first);
	}
	return linieDoUsuniecia(wsadowy, roo, root);
}


pair <long long, vector <long long> > relacje_linia(osm_base* roo, long long root, string linia)
{
	pair<long long, vector <long long> > wynik;
	wynik.first=0;
	relation akt=roo->relations[root];
	map <string, string> tags = akt.getTags();
	if(tags["ref"]==linia)
	{
		if(tags["type"]=="route")
		{
			wynik.second.push_back(akt.id);
		}
		if(tags["type"]=="route_master")
		{
			wynik.first=akt.id;
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
				pair<long long, vector <long long> > tmp=relacje_linia(roo, teraz_id, linia);
				if(tmp.first!=0)
				{
					wynik.first=tmp.first;
				}
				for(int j=0; j<tmp.second.size(); j++)
				{
					wynik.second.push_back(tmp.second[j]);
				}
			}
		}
	}
	return wynik;
}


set <string> extract_ref(osm_base* baza, long long rel, string ref_key)
{
	set<string> wynik;
	relation akt=baza->relations[rel];
	int s1=akt.members.size();
	for(int i=0; i<s1; i++)
	{
		if(akt.members[i].member_type==NODE)
		{
			long long teraz_id=akt.members[i].member_id;
			if(baza->nodes.find(teraz_id)!=baza->nodes.end())
			{
				node teraz=baza->nodes[teraz_id];
				auto tags=teraz.getTags();
				if(tags["highway"]=="bus_stop" || tags["railway"]=="tram_stop" || tags["public_transport"]=="stop_position")
				{
					if(tags.find(ref_key)!=tags.end())
					{
						wynik.insert(tags[ref_key]);
					}
				}
			}
		}
		if(akt.members[i].member_type==WAY)
		{
			long long teraz_id=akt.members[i].member_id;
			if(baza->ways.find(teraz_id)!=baza->ways.end())
			{
				way teraz=baza->ways[teraz_id];
				auto tags = teraz.getTags();
				if(tags["highway"]=="platform" || tags["railway"]=="platform")
				{
					if(tags.find(ref_key)!=tags.end())
					{
						wynik.insert(tags[ref_key]);
					}
				}
			}

		}
	}
	return wynik;
}

map <long long, string> szukaj_na_drogach(osm_base& baza, long long rel, map<long long, string> nodes_to_be)
{
	relation akt=baza.relations[rel];
	int s1=akt.members.size();
	for(int i=0; i<s1; i++)
	{
		if(akt.members[i].member_type==WAY)
		{
			long long teraz_id=akt.members[i].member_id;
			if(baza.ways.find(teraz_id)!=baza.ways.end())
			{
				way teraz=baza.ways[teraz_id];
				int s2=teraz.nodes.size();
				for(int j=0; j<s2; j++)
				{
					if(nodes_to_be.find(teraz.nodes[j])!=nodes_to_be.end())
					{
						nodes_to_be.erase(nodes_to_be.find(teraz.nodes[j]));
					}
				}
			}
		}
	}
	return nodes_to_be;
}
set <string> PrzegladanieCzyPrawidloweStareLinie::oldRelationStops(string linia, osm_base* baza)
{
	set <string> wynik0;
	vector <long long> rels=relacje_linia(baza, rootRel, linia).second;
	for(int i=0; i<rels.size(); i++)
	{
		set <string> tmp1=extract_ref(baza, rels[i], ref_key);
		wynik0.insert(tmp1.begin(), tmp1.end());
	}
	return wynik0;
}

set <string> PrzegladanieCzyPrawidloweStareLinie::newRelationStops(string linia, ScheduleHandlerInternal* baza)
{
	set <string> wynik;
	for(int i=0; i<(baza->dane_linia[linia]).size(); i++)
	{
		vector <string> przystanki=baza->dane_linia[linia][i];
		for(int j=0; j<przystanki.size(); j++)
		{
			wynik.insert(przystanki[j]);
		}
	}
	return wynik;
}

void PrzegladanieCzyPrawidloweStareLinie::zbadajLinie(string linia, osm_base* bazaOsm, ScheduleHandlerInternal* bazaZtm)
{
	relacjeDlaLinii[linia]=relacje_linia(bazaOsm, rootRel, linia);
	set <string> osm_list=oldRelationStops(linia, bazaOsm);
	set <string> ztm_list=newRelationStops(linia, bazaZtm);
	set <string>::iterator it1=osm_list.begin();
	set <string>::iterator it2=ztm_list.begin();
	vector <long long> rels=relacje_linia(bazaOsm, rootRel, linia).second;
	bool prawi=1;
	for(int i=0; i<rels.size(); i++)
	{
			if(!relationCohesion(rels[i], bazaOsm))
			{
				badRelations.insert(rels[i]);
				badLines.insert(linia);
				prawi=0;
			}
	}
	while(it1!=osm_list.end())
	{
		if(ztm_list.find(*it1)==ztm_list.end())
		{
			onlyOsmStop[linia].insert(*it1);
			prawi=0;
		}
		it1++;
	}
	while(it2!=ztm_list.end())
	{
		if(osm_list.find(*it2)==osm_list.end())
		{
			onlyZtmStop[linia].insert(*it2);
			prawi=0;
		}
		it2++;
	}
	if(prawi)
	{
		prawidlowe.insert(linia);
	}
	else
	{
		nieprawidlowe.insert(linia);
	}
}
PrzegladanieCzyPrawidloweStareLinie::PrzegladanieCzyPrawidloweStareLinie(osm_base* bazaOsm, ScheduleHandlerInternal* bazaZtm, set <string> doPrzerobienia, long long rootRelW, string ref_keyW)
{
	ref_key = ref_keyW;
	rootRel = rootRelW;
	set <string>::iterator it1=doPrzerobienia.begin();
	while(it1!=doPrzerobienia.end())
	{
		zbadajLinie(*it1, bazaOsm, bazaZtm);
		it1++;
	}
}


set <string> PrzegladanieCzyPrawidloweNoweLinie::sprawdzLinie(string linia, vector <vector <string> > drugi)
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

set <string> PrzegladanieCzyPrawidloweNoweLinie::getPrawidlowe()
{
	return prawidlowe;
}
set <string> PrzegladanieCzyPrawidloweNoweLinie::getNieprawidlowe()
{
	set <string> wynik;
	for(auto& it1 : nieprawidlowe)
	{
		if(it1.first.size()>0)
			wynik.insert(it1.first);
	}
	return wynik;
}
map <string, set<string> > PrzegladanieCzyPrawidloweNoweLinie::getNieprawidloweMap()
{
	return nieprawidlowe;
}
PrzegladanieCzyPrawidloweNoweLinie::PrzegladanieCzyPrawidloweNoweLinie(map<string, OsmStopData>* osmStopsW, ScheduleHandlerInternal* bazaZtmW, set<string> doPrzerobienia)
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
