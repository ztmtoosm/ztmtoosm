#ifndef OSMAPI
#define OSMAPI
#include <vector>
#include <cstring>
#include <map>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "stringspecial.hpp"
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
using namespace std;
using namespace rapidxml;
struct every_member
{
	int version;
	long long id;
	char type;
	bool modify;
	bool todelete;
	every_member(char typ)
	{
		type=typ;
		modify=0;
		todelete=0;
	}
};

struct node : every_member
{
	node() : every_member('N') {};
	double lat;
	double lon;
};

struct way : every_member
{
	way() : every_member('W') {};
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
	relation() : every_member('R') {};
	vector <relation_member > members;
};
class osm_base
{
	public:
	virtual node getNode(long long id) = 0;
	virtual long long getNextNode(long long id) = 0;
	virtual way getWay(long long id) = 0;
	virtual long long getNextWay(long long id) = 0;
	virtual relation getRelation(long long id) = 0;
	virtual long long getNextRelation(long long id) = 0;
	virtual void addNode(pair<node, map<string, string> > foo) = 0;
	virtual void addWay(pair<way, map<string,string> > foo) = 0;
	virtual void addRelation(pair<relation, map<string, string> >foo) = 0;
	virtual bool existNode(long long id) = 0;
	virtual bool existWay(long long id) = 0;
	virtual bool existRelation(long long id) = 0;
	virtual map <string, string> getAllTags(long long id, char type) = 0;
	virtual string getTag(long long id, char type, string key) = 0;
	public:
	int new_ways;
	void wypisz(string sciezka)
	{
		int licznik=0;
		fstream plik6(sciezka.c_str(), ios::out | ios::trunc);
		plik6.precision(9);
		plik6<<"<osm version='0.6'>"<<endl;
		long long st1=getNextNode(0);
		while(st1!=0)
		{
			cout<<st1<<" "<<licznik<<endl;
			licznik++;
			node akt=getNode(st1);
			wypisz_node(akt, plik6);
			st1=getNextNode(st1);
		}
		long long st2=getNextWay(0);
		while(st2!=0)
		{

			cout<<st2<<" x "<<licznik<<endl;
			licznik++;
			way akt=getWay(st2);
			wypisz_way(akt, plik6);
			st2=getNextWay(st2);
		}
		long long st3=getNextRelation(0);
		while(st3!=0)
		{
			cout<<st3<<" y "<<licznik<<endl;
			licznik++;
			relation akt=getRelation(st3);
			wypisz_relation(akt, plik6);
			st3=getNextRelation(st3);
		}
		plik6<<"</osm>"<<endl;
	}
	vector <pair<long long, vector<long long> > > rozdziel_way(long long id, set <long long> rozdzielacze)
	{
		way akt_way=getWay(id);
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
				addWay(nowa);
			}
			long long st1=getNextRelation(0);
			while(st1!=0)
			{
				relation akt = getRelation(st1);
				vector <relation_member> members=akt.members;
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
					akt.members=members2;
					akt.modify=1;
					addRelation(akt);
				}
				st1=getNextRelation(st1);
			}
		}
		return partial;
	}
	osm_base()
	{
		new_ways=-1;
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
	void wypisz_tags(every_member& teraz, ostream& plik)
	{
		map<string, string> tagi=getAllTags(teraz.id, teraz.type);
		map<string, string>::iterator it1= tagi.begin();
		while(it1!=tagi.end())
		{
			plik<<"<tag k=\""<<it1->first<<"\" v=\""<<avoid_cudzyslow(it1->second)<<"\"/>"<<endl;
			it1++;
		}
	}
	void wypisz_id(every_member& teraz, ostream& plik)
	{
		plik<<" id=\""<<teraz.id<<"\"";
		if(teraz.todelete==true)
		{
			cout<<"TODELETE TODELETE TODELETE"<<" "<<teraz.id<<endl;
				plik<<" action=\"delete\"";
		}
		else
		{
			if(teraz.modify)
			{
				plik<<" action=\"modify\"";
			}
		}
		if(teraz.id>=0)
		{
			plik<<" version=\""<<teraz.version<<"\"";
		}
	}
};

class osm_base_standard : public osm_base
{
	public:
	map<long long, node> nodes;
	map<long long, way> ways;
	map<long long, relation> relations;
	map<long long, map<string, string> > tagsNodes;
	map<long long, map<string, string> > tagsWays;
	map<long long, map<string, string> > tagsRelations;
	virtual node getNode(long long id)
	{
		return nodes[id];
	}
	virtual long long getNextNode(long long id)
	{
		auto it1 = nodes.find(id);
		it1++;
		if(id==0)
			it1=nodes.begin();
		if(it1==nodes.end()) return 0;
		return it1->first;
	}
	virtual way getWay(long long id)
	{
		return ways[id];
	}
	virtual long long getNextWay(long long id)
	{
		auto it1 = ways.find(id);
		it1++;
		if(id==0)
			it1=ways.begin();
		if(it1==ways.end()) return 0;
		return it1->first;
	
	}
	virtual relation getRelation(long long id)
	{
		return relations[id];
	}
	virtual long long getNextRelation(long long id)
	{
		auto it1 = relations.find(id);
		it1++;
		if(id==0)
			it1=relations.begin();
		if(it1==relations.end()) return 0;
		return it1->first;
	}
	virtual void addNode(node foo)
	{
		nodes[foo.id]=foo;
	}
	virtual void addWay(way foo)
	{
		ways[foo.id]=foo;
	}
	virtual void addRelation(relation foo)
	{
		relations[foo.id]=foo;
	}
	virtual bool existNode(long long id)
	{
		return (nodes.find(id)!=nodes.end());
	}
	virtual bool existWay(long long id)
	{
		return (ways.find(id)!=ways.end());
	}
	virtual bool existRelation(long long id)
	{
		return (relations.find(id)!=relations.end());
	}
	virtual map <string, string> getAllTags(long long id, char type)
	{
		if(type=='N')
			return tagsNodes[id];
		if(type=='W')
			return tagsWays[id];
		if(type=='R')
			return tagsRelations[id];
	}
	virtual string getTag(long long id, char type, string key)
	{
		if(type=='N')
			return tagsNodes[id][key];
		if(type=='W')
			return tagsWays[id][key];
		if(type=='R')
			return tagsRelations[id][key];
	}
	osm_base_standard (osm_base* root, bool modified)
	{
		long long st1=root->getNextNode(0);
		while(st1!=0)
		{
			node akt = root->getNode(st1);
			if(akt.modify || akt.todelete)
				nodes[akt.id]=akt;
			st1=root->getNextNode(st1);
		}
		long long st2=root->getNextWay(0);
		while(st2!=0)
		{
			way akt=root->getWay(st2);
			if(akt.modify || akt.todelete)
			{
				ways[akt.id]=akt;
				int s1=akt.nodes.size();
				for(int i=0; i<s1; i++)
				{
					nodes[akt.nodes[i]]=root->getNode(akt.nodes[i]);
				}
			}
			st2=root->getNextWay(st2);
		}
		long long st3=root->getNextRelation(0);
		while(st3!=0)
		{
			relation akt=root->getRelation(st3);
			if(akt.modify || akt.todelete)
				relations[akt.id]=akt;
			st3=root->getNextRelation(st3);
		}
	}
	
	osm_base_standard (osm_base *root, set<long long> n, set<long long> w, set<long long> r)
	{
		auto it1 = w.begin();
		while(it1 != w.end())
		{
			way foo=root->getWay(*it1);
			for(int j=0; j<foo.nodes.size(); j++)
			{
				n.insert(foo.nodes[j]);
			}
			it1++;
		}
		auto it2 = n.begin();
		while(it2 != n.end())
		{
			nodes[*it2]=root->getNode(*it2);
			it2++;
		}
		auto it3 = w.begin();
		while(it3 != w.end())
		{
			ways[*it3]=root->getWay(*it3);
			it3++;
		}
		auto it4 = r.begin();
		while(it4 != r.end())
		{
			relations[*it4]=root->getRelation(*it4);
			it4++;
		}
	}

	osm_base_standard (osm_base* alfa, osm_base* beta)
	{
		long long st1a=alfa->getNextNode(0);
		while(st1a!=0)
		{
			node foo = alfa->getNode(st1a);
			nodes[foo.id]=foo;
			st1a=alfa->getNextNode(st1a);
		}
		long long st1b=beta->getNextNode(0);
		while(st1b!=0)
		{
			node foo = beta->getNode(st1b);
			nodes[foo.id]=foo;
			st1b=beta->getNextNode(st1b);
		}
		long long st2a=alfa->getNextWay(0);
		while(st1a!=0)
		{
			way foo = alfa->getWay(st2a);
			ways[foo.id]=foo;
			st2a=alfa->getNextWay(st2a);
		}
		long long st2b=beta->getNextWay(0);
		while(st2b!=0)
		{
			way foo = beta->getWay(st2b);
			ways[foo.id]=foo;
			st2b=beta->getNextWay(st2b);
		}
		long long st3a=alfa->getNextRelation(0);
		while(st3a!=0)
		{
			relation foo = alfa->getRelation(st3a);
			relations[foo.id]=foo;
			st3a=alfa->getNextRelation(st3a);
		}
		long long st3b=beta->getNextRelation(0);
		while(st3b!=0)
		{
			relation foo = beta->getRelation(st3b);
			relations[foo.id]=foo;
			st3b=beta->getNextRelation(st3b);
		}
	}

	osm_base_standard(string sciezka)
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
			if(foo->type=='N')
				tagsNodes[foo->id][key->value()]=val->value();
			if(foo->type=='W')
				tagsWays[foo->id][key->value()]=val->value();
			if(foo->type=='R')
				tagsRelations[foo->id][key->value()]=val->value();
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
		if(nodes.find(foo->id)==nodes.end() || nodes[foo->id].version<foo2.version)
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
		if(ways.find(foo->id)==ways.end() || ways[foo->id].version<foo2.version)
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
		if(relations.find(foo->id)==relations.end() || relations[foo->id].version<foo2.version)
			relations[foo->id]=foo2;
	}
};

class osm_base_sql : public osm_base
{
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
	void values (long long id, char key, every_member& foo)
	{
		stringstream polecenie;
		polecenie<<"SELECT * FROM tags WHERE id="<<id;
		polecenie<<" AND type=\""<<key<<"\"";
		polecenie<<" AND keya=\""<<"VERSION"<<"\"";
		foo.id=id;
		res = stmt->executeQuery(polecenie.str());
		while(res->next())
		{
			foo.version=fromstring<int>(res->getString("valuea"));
		}
		delete res;
	}
	map<string, string> values (long long id, char key)
	{
		stringstream polecenie;
		polecenie<<"SELECT * FROM tags WHERE id="<<id;
		polecenie<<" AND type=\""<<key<<"\"";
		map <string, string> wynik;
		res = stmt->executeQuery(polecenie.str());
		while(res->next())
		{
			string key=res->getString("keya");
			if(key!="ID" && key!="VERSION")
				wynik.insert(pair<string, string>(res->getString("keya"), res->getString("valuea")));
		}
		delete res;
		return wynik;
	}
	string values (long long id, char key, string keya)
	{
		stringstream polecenie;
		polecenie<<"SELECT * FROM tags WHERE id="<<id;
		polecenie<<" AND type=\""<<key<<"\"";
		polecenie<<" AND keya=\""<<keya<<"\"";
		res = stmt->executeQuery(polecenie.str());
		while(res->next())
		{
			return res->getString("valuea");
		}
		delete res;
		return "";
	}
	public:
	map<long long, node> nodes;
	map<long long, way> ways;
	map<long long, relation> relations;
	map<long long, map<string, string> > tagsNodes;
	map<long long, map<string, string> > tagsWays;
	map<long long, map<string, string> > tagsRelations;
	virtual map <string, string> getAllTags(long long id, char type)
	{
		if(type=='N')
		{
			if(nodes.find(id)!=nodes.end())
				return tagsNodes[id];
			return values(id, type);
		}
		if(type=='W')
		{
			if(ways.find(id)!=ways.end())
				return tagsWays[id];
			return values(id, type);
		}
		if(type=='R')
		{
			return tagsRelations[id];
		}
	}
	virtual string getTag(long long id, char type, string key)
	{
		if(type=='N')
		{
			if(nodes.find(id)!=nodes.end())
				return tagsNodes[id][key];
			return values(id, type, key);
		}
		if(type=='W')
		{
			if(ways.find(id)!=ways.end())
				return tagsWays[id][key];
			return values(id, type, key);
		}
		if(type=='R')
		{
			return tagsRelations[id][key];
		}
	
	}
	virtual node getNode(long long id)
	{
		if(nodes.find(id)!=nodes.end())
			return nodes[id];
		node nowy;
		values(id, 'N', nowy);
		stringstream polecenie;
		polecenie<<"SELECT * FROM nodes WHERE id="<<id;
		res = stmt->executeQuery(polecenie.str());
		while(res->next())
		{
			nowy.lat=res->getDouble("lat");
			nowy.lon=res->getDouble("lon");
		}
		delete res;
		return nowy;
	}
	long long getNextNodeSql(long long id)
	{
		stringstream polecenie;
		polecenie<<"SELECT * FROM nodes WHERE id>"<<id;
		polecenie<<" AND type=N AND keya=\"ID\" ORDER BY id LIMIT 1";
		long long wynik=0;
		res = stmt->executeQuery(polecenie.str());
		while(res->next())
		{
			wynik=fromstring<long long>(res->getString("valuea"));
		}
		if(nodes.find(wynik)!=nodes.end())
			return getNextNodeSql(wynik);
		return wynik;
	}
	virtual long long getNextNodeMap(long long id)
	{
		if(nodes.size()==0) return 0;
		auto it1 = nodes.find(id);
		it1++;
		if(id==0)
			it1=nodes.begin();
		if(it1==nodes.end()) return 0;
		return it1->first;
	}
	virtual long long getNextNode(long long id)
	{
		long long wynik=0;
		if(id==0 || nodes.find(id)!= nodes.end())
			wynik=getNextNodeMap(id);
		if(wynik==0)
			wynik=getNextNodeSql(id);
		cout<<wynik<<endl;
		return wynik;
	}
	
	virtual way getWay(long long id)
	{
		if(ways.find(id)!=ways.end())
			return ways[id];
		way nowy;
		values(id, 'W', nowy);
		stringstream polecenie;
		polecenie<<"SELECT * FROM ways WHERE id="<<id<<" ";
		polecenie<<"ORDER BY licznik";
		res = stmt->executeQuery(polecenie.str());
		while(res->next())
		{
			nowy.nodes.push_back(res->getInt64("ref"));
		}
		delete res;
		return nowy;
	}
	virtual long long getNextWaySql(long long id)
	{
		stringstream polecenie;
		polecenie<<"SELECT * FROM tags";
		polecenie<<" WHERE type=\"W\"";
		polecenie<<" AND id>"<<id;
		polecenie<<" AND keya=\"ID\" LIMIT 1";
		long long wynik=0;
		res = stmt->executeQuery(polecenie.str());
		while(res->next())
		{
			wynik=fromstring<long long>(res->getString("valuea"));
		}
		if(ways.find(wynik)!=ways.end())
			return getNextWaySql(wynik);
		return wynik;
	}

	virtual long long getNextWayMap(long long id)
	{
		if(ways.size()==0) return 0;
		auto it1 = ways.find(id);
		it1++;
		if(id==0)
			it1=ways.begin();
		if(it1==ways.end()) return 0;
		return it1->first;
	}

	virtual long long getNextWay(long long id)
	{
		long long wynik=0;
		if(id==0 || ways.find(id)!=ways.end())
			wynik=getNextWayMap(id);
		if(wynik==0)
			wynik=getNextWaySql(id);
		cout<<wynik<<endl;
		return wynik;
	}

	virtual relation getRelation(long long id)
	{
		return relations[id];
	}
	virtual long long getNextRelation(long long id)
	{
		auto it1 = relations.find(id);
		it1++;
		if(id==0)
			it1=relations.begin();
		if(it1==relations.end()) return 0;
		return it1->first;
	}
	virtual void addNode(node foo)
	{
		nodes[foo.id]=foo;
	}
	virtual void addWay(way foo)
	{
		ways[foo.id]=foo;
	}
	virtual void addRelation(relation foo)
	{
		relations[foo.id]=foo;
	}
	virtual bool existNode(long long id)
	{
		if (nodes.find(id)!=nodes.end()) return true;
		
		stringstream polecenie;
		polecenie<<"SELECT * FROM tags WHERE id="<<id;
		polecenie<<" AND type=\"N\"";
		polecenie<<" AND keya=\"ID\"";
		bool wynik=0;
		res = stmt->executeQuery(polecenie.str());
		while(res->next())
		{
			wynik=1;
		}
		delete res;
		return wynik;
	}
	virtual bool existWay(long long id)
	{
		if (ways.find(id)!=ways.end()) return true;
			
		stringstream polecenie;
		polecenie<<"SELECT * FROM tags WHERE id="<<id;
		polecenie<<" AND type=\"W\"";
		polecenie<<" AND keya=\"ID\"";
		bool wynik=0;
		res = stmt->executeQuery(polecenie.str());
		while(res->next())
		{
			wynik=1;
		}
		delete res;
		return wynik;
	}
	virtual bool existRelation(long long id)
	{
		return (relations.find(id)!=relations.end());
	}
	osm_base_sql(string sciezka)
	{
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "");
		con->setSchema("osm");
		stmt = con->createStatement();
		new_ways=-1;
		file <> xmlFile(sciezka.c_str());
		xml_document<> doc;
		doc.parse<0>(xmlFile.data());
		xml_node<>* root=doc.first_node("osm");
		for(xml_node <> *tag=root->first_node("relation"); tag; tag=tag->next_sibling("relation"))
		{
			load_relation(tag);
		}
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
			if(foo->type=='N')
				tagsNodes[foo->id][key->value()]=val->value();
			if(foo->type=='W')
				tagsWays[foo->id][key->value()]=val->value();
			if(foo->type=='R')
				tagsRelations[foo->id][key->value()]=val->value();
		}
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
		if(relations.find(foo->id)==relations.end() || relations[foo->id].version<foo2.version)
			relations[foo->id]=foo2;
	}


};

#endif
