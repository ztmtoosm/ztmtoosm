#ifndef OSMAPI
#define OSMAPI
#include <vector>
#include <cstring>
#include <map>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "stringspecial.hpp"
using namespace std;
using namespace rapidxml;
struct every_member
{
	int version;
	long long id;
	map <string, string> tags;
	bool modify;
	bool todelete;
	every_member()
	{
		modify=0;
		todelete=0;
	}
};

struct node : every_member
{
	double lat;
	double lon;
};

struct way : every_member
{
	vector <long long> nodes;
};

enum relation_member_type
{
	NODE, WAY, RELATION
};

struct relation_member
{
	long long member_id;
	relation_member_type member_type;
	string role;
};

struct relation : every_member
{
	vector <relation_member > members;
};

struct osm_base
{
	public:
	int new_ways;
	map<long long, node> nodes;
	map<long long, way> ways;
	map<long long, relation> relations;
	void wypisz(string sciezka)
	{
		fstream plik6(sciezka.c_str(), ios::out | ios::trunc);
		plik6.precision(9);
		plik6<<"<osm version='0.6'>"<<endl;
		map<long long, node>::iterator it1=nodes.begin();
		while(it1!=nodes.end())
		{
			wypisz_node(it1->second, plik6);
			it1++;
		}
		map<long long, way>::iterator it2=ways.begin();
		while(it2!=ways.end())
		{
			wypisz_way(it2->second, plik6);
			it2++;
		}
		map<long long, relation>::iterator it3=relations.begin();
		while(it3!=relations.end())
		{
			wypisz_relation(it3->second, plik6);
			it3++;
		}
		plik6<<"</osm>"<<endl;
	}
	vector <pair<long long, vector<long long> > > rozdziel_way(long long id, set <long long> rozdzielacze)
	{
		way akt_way=ways[id];
		vector <long long> pusty;
		vector <pair<long long, vector <long long> > > partial;
		partial.push_back(pair<long long, vector<long long> >(id, pusty));
		int s1=akt_way.nodes.size();
		for(int i=0; i<s1; i++)
		{
			partial[partial.size()-1].second.push_back(akt_way.nodes[i]);
			if(i!=0 && i!=s1-1)
			{
				if(rozdzielacze.find(akt_way.nodes[i])!=rozdzielacze.end())
				{
					partial.push_back(pair<long long, vector<long long> >(new_ways, pusty));
					partial[partial.size()-1].second.push_back(akt_way.nodes[i]);
					new_ways--;
				}

			}
		}
		if(partial.size()>1)
		{
			for(int i=0; i<partial.size(); i++)
			{
				way nowa=akt_way;
				nowa.nodes=partial[i].second;
				nowa.id=partial[i].first;
				if(nowa.id!=id)
					nowa.version=-1;
				nowa.modify=1;
				ways[nowa.id]=nowa;
			}
			map<long long, relation>::iterator it1=relations.begin();
			while(it1!=relations.end())
			{
				vector <relation_member> members=(it1->second).members;
				vector <relation_member> members2;
				bool ok=0;
				for(int i=0; i<members.size(); i++)
				{
					if(members[i].member_id==id && members[i].member_type==WAY)
					{
						ok=1;
						for(int j=0; j<partial.size(); j++)
						{
							relation_member foo=members[i];
							foo.member_id=partial[j].first;
							members2.push_back(foo);
						}
					}
					else
					{
						members2.push_back(members[i]);
					}
				}
				if(ok)
				{
					(it1->second).members=members2;
					(it1->second).modify=1;
				}
				it1++;
			}
		}
		return partial;
	}
	osm_base(osm_base& alfa, osm_base& beta)
	{
		new_ways=alfa.new_ways;
		nodes=alfa.nodes;
		ways=alfa.ways;
		relations=alfa.relations;
		map<long long, node>::iterator it1=beta.nodes.begin();
		while(it1!=beta.nodes.end())
		{
			if(nodes.find(it1->first)==nodes.end())
				nodes[it1->first]=it1->second;
			it1++;
		}
		map<long long, way>::iterator it2=beta.ways.begin();
		while(it2!=beta.ways.end())
		{
			if(ways.find(it2->first)==ways.end())
				ways[it2->first]=it2->second;
			it2++;
		}
		map<long long, relation>::iterator it3=beta.relations.begin();
		while(it3!=beta.relations.end())
		{
			if(relations.find(it3->first)==relations.end())
				relations[it3->first]=it3->second;
			it3++;
		}
	}
	osm_base modified()
	{
		osm_base nowa;
		map<long long, node>::iterator it1=nodes.begin();
		while(it1!=nodes.end())
		{
			if(it1->second.modify)
				nowa.nodes[it1->first]=it1->second;
			it1++;
		}
		map<long long, way>::iterator it2=ways.begin();
		while(it2!=ways.end())
		{
			if(it2->second.modify)
			{
				nowa.ways[it2->first]=it2->second;
				int s1=it2->second.nodes.size();
				for(int i=0; i<s1; i++)
				{
					nowa.nodes[it2->second.nodes[i]]=nodes[it2->second.nodes[i]];
				}
			}
			it2++;
		}
		map<long long, relation>::iterator it3=relations.begin();
		while(it3!=relations.end())
		{
			if(it3->second.modify)
				nowa.relations[it3->first]=it3->second;
			it3++;
		}
		return nowa;
	}
	osm_base wybrane(set<long long> n, set<long long> w, set<long long> r)
	{
		osm_base nowa;
		map<long long, node>::iterator it1=nodes.begin();
		while(it1!=nodes.end())
		{
			if(n.find(it1->first)!=n.end())
				nowa.nodes[it1->first]=it1->second;
			it1++;
		}
		map<long long, way>::iterator it2=ways.begin();
		while(it2!=ways.end())
		{
			if(w.find(it2->first)!=w.end())
			{
				nowa.ways[it2->first]=it2->second;
				int s1=it2->second.nodes.size();
				for(int i=0; i<s1; i++)
				{
					nowa.nodes[it2->second.nodes[i]]=nodes[it2->second.nodes[i]];
				}
			}
			it2++;
		}
		map<long long, relation>::iterator it3=relations.begin();
		while(it3!=relations.end())
		{
			if(r.find(it3->first)!=r.end())
				nowa.relations[it3->first]=it3->second;
			it3++;
		}
		return nowa;
	}
	osm_base()
	{
		new_ways=-1;
	}
	osm_base(string sciezka)
	{
		new_ways=-1;
		file <> xmlFile(sciezka.c_str());
		xml_document<> doc;
		doc.parse<0>(xmlFile.data());
		xml_node<>* root=doc.first_node("osm");
		for(xml_node <> *tag=root->first_node("node"); tag; tag=tag->next_sibling("node"))
		{
			load_node(tag);
		}
		for(xml_node <> *tag=root->first_node("way"); tag; tag=tag->next_sibling("way"))
		{
			load_way(tag);
		}
		for(xml_node <> *tag=root->first_node("relation"); tag; tag=tag->next_sibling("relation"))
		{
			load_relation(tag);
		}
	}
	
	private:
	string avoid_cudzyslow(string slowo)
	{
		string wynik;
		int s1=slowo.length();
		for(int i=0; i<s1; i++)
		{
			if(slowo[i]!='"' && slowo[i]!='&' && slowo[i]!='<' && slowo[i]!='>' && slowo[i]!='\'')
			{
				wynik+=slowo[i];
			}
			else
			{
				if(slowo[i]=='"')
					wynik+="&quot;";
				if(slowo[i]=='&')
					wynik+="&amp;";
				if(slowo[i]=='<')
					wynik+="&lt;";
				if(slowo[i]=='>')
					wynik+="&gt;";
				if(slowo[i]=='\'')
					wynik+="&apos;";
		
			}
		}
		return wynik;
	}
	void wypisz_tags(every_member& teraz, ostream& plik)
	{
		map<string, string>::iterator it1=teraz.tags.begin();
		while(it1!=teraz.tags.end())
		{
			plik<<"<tag k=\""<<it1->first<<"\" v=\""<<avoid_cudzyslow(it1->second)<<"\"/>"<<endl;
			it1++;
		}
	}
	void wypisz_id(every_member& teraz, ostream& plik)
	{
		plik<<" id=\""<<teraz.id<<"\"";
		if(teraz.modify)
		{
			plik<<" action=\"modify\"";
		}
		if(teraz.id>=0)
		{
			plik<<" version=\""<<teraz.version<<"\"";
		}
	}
	void wypisz_node(node& teraz, ostream& plik)
	{
		plik<<"<node ";
		wypisz_id(teraz, plik);
		plik<<" lat=\""<<teraz.lat<<"\" lon=\""<<teraz.lon<<"\">"<<endl;
		wypisz_tags(teraz, plik);
		plik<<"</node>"<<endl;
	}
	void wypisz_way(way& teraz, ostream& plik)
	{
		plik<<"<way ";
		wypisz_id(teraz, plik);
		plik<<">"<<endl;
		wypisz_tags(teraz, plik);
		for(int i=0; i<teraz.nodes.size(); i++)
		{
			plik<<"<nd ref=\""<<teraz.nodes[i]<<"\"/>"<<endl;
		}
		plik<<"</way>"<<endl;
	}
	void wypisz_relation(relation& teraz, ostream& plik)
	{
		plik<<"<relation ";
		wypisz_id(teraz, plik);
		plik<<">"<<endl;
		wypisz_tags(teraz, plik);
		for(int i=0; i<teraz.members.size(); i++)
		{
			plik<<"<member ";
			plik<<"type=\"";
			if(teraz.members[i].member_type==WAY)
				plik<<"way";
			if(teraz.members[i].member_type==NODE)
				plik<<"node";
			if(teraz.members[i].member_type==RELATION)
				plik<<"relation";
			plik<<"\" ";
			plik<<"role=\""<<teraz.members[i].role<<"\" ";
			plik<<"ref=\""<<teraz.members[i].member_id<<"\"/>"<<endl;
		}
		plik<<"</relation>"<<endl;
	}
	void read_every(xml_node <>* root, every_member* foo)
	{
		xml_attribute<>*id=root->first_attribute("id");
		xml_attribute<>*version=root->first_attribute("version");
		foo->id=fromstring<long long>(id->value());
		if(version!=NULL)
			foo->version=fromstring<int>(version->value());
		for(xml_node <> *tag=root->first_node("tag"); tag; tag=tag->next_sibling("tag"))
		{
			xml_attribute <> *key=tag->first_attribute("k");
			xml_attribute <> *val=tag->first_attribute("v");
			foo->tags[key->value()]=val->value();
		}
	}
	void load_node(xml_node <>* root)
	{
		node foo2;
		node* foo=&foo2;
		xml_attribute<>*lat=root->first_attribute("lat");
		xml_attribute<>*lon=root->first_attribute("lon");
		foo->lat=fromstring<double>(lat->value());
		foo->lon=fromstring<double>(lon->value());
		read_every(root, foo);
		nodes[foo->id]=foo2;
	}
	void load_way(xml_node <>* root)
	{
		way foo2;
		way* foo=&foo2;
		for(xml_node <> *tag=root->first_node("nd"); tag; tag=tag->next_sibling("nd"))
		{
			xml_attribute <> *val=tag->first_attribute("ref");
			foo->nodes.push_back(fromstring<long long>(val->value()));
		}
		read_every(root, foo);
		ways[foo->id]=foo2;
	}
	void load_relation(xml_node <>* root)
	{
		relation foo2;
		relation* foo=&foo2;
		for(xml_node <> *tag=root->first_node("member"); tag; tag=tag->next_sibling("member"))
		{
			relation_member foo2;
			xml_attribute <> *role=tag->first_attribute("role");
			xml_attribute <> *ref=tag->first_attribute("ref");
			xml_attribute <> *type=tag->first_attribute("type");
			string typ=type->value();
			if(typ=="relation")
				foo2.member_type=RELATION;
			if(typ=="way")
				foo2.member_type=WAY;
			if(typ=="node")
				foo2.member_type=NODE;
			foo2.member_id=(fromstring<long long>(ref->value()));
			foo2.role=role->value();
			foo->members.push_back(foo2);
		}
		read_every(root, foo);
		relations[foo->id]=foo2;
	}
	
};
pair <long long, vector <long long> > relacje_linia(osm_base& roo, long long root, string linia)
{
	pair<long long, vector <long long> > wynik;
	wynik.first=0;
	relation akt=roo.relations[root];
	cout<<akt.id<<endl;
	if(akt.tags["ref"]==linia)
	{
		if(akt.tags["type"]=="route")
		{
			wynik.second.push_back(akt.id);
		}
		if(akt.tags["type"]=="route_master")
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
			if(roo.relations.find(teraz_id)!=roo.relations.end())
			{
				relation teraz=roo.relations[teraz_id];
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

set <string> extract_ref(osm_base& baza, long long rel)
{
	set<string> wynik;
	relation akt=baza.relations[rel];
	int s1=akt.members.size();
	for(int i=0; i<s1; i++)
	{
		if(akt.members[i].member_type==NODE)
		{
			long long teraz_id=akt.members[i].member_id;
			if(baza.nodes.find(teraz_id)!=baza.nodes.end())
			{
				node teraz=baza.nodes[teraz_id];
				if(teraz.tags["highway"]=="bus_stop" || teraz.tags["railway"]=="tram_stop" || teraz.tags["public_transport"]=="stop_position")
				{
					if(teraz.tags.find("ref")!=teraz.tags.end())
					{
						wynik.insert(teraz.tags["ref"]);
					}
				}
			}
		}
	}
	return wynik;
}

bool szukaj_na_drogach(osm_base& baza, long long rel, set<long long> nodes_to_be)
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
	return nodes_to_be.empty();
}
#endif
