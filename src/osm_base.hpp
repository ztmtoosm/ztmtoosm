#ifndef OSMAPI
#define OSMAPI
#include "../xml/XmlInspector.hpp"
#include <vector>
#include <cstring>
#include <map>
#include "stringspecial.hpp"
using namespace std;
struct every_member
{
	static map <string, int> kodyKey;
	static vector <string> kodyKey2;
	int version;
	long long id;
	private:
	int tagiSize;
	int* tagi1;
	int* tagi2;
	public:
	map<string, string> getTags() const
	{
		map <string, string> wynik;
		for(int i=0; i<tagiSize; i++)
		{
			wynik[kodyKey2[tagi1[i]]]=kodyKey2[tagi2[i]];
		}
		return wynik;
	}
	void setTags (map<string, string> tags)
	{
		if(tagi1!=NULL)
		{
			delete tagi1;
			tagi1=NULL;
			delete tagi2;
			tagi2=NULL;
		}
		tagiSize=tags.size();
		if(tags.size()>0)
		{
			tagi1 = new int[tags.size()];
			tagi2 = new int[tags.size()];
		}
		auto it1=tags.begin();
		int licznik=0;
		while(it1!=tags.end())
		{
			if(kodyKey.find(it1->first)==kodyKey.end())
			{
				int s1=kodyKey2.size();
				kodyKey.insert(make_pair(it1->first, s1));
				kodyKey2.push_back(it1->first);
			}
			if(kodyKey.find(it1->second)==kodyKey.end())
			{
				int s1=kodyKey2.size();
				kodyKey.insert(make_pair(it1->second, s1));
				kodyKey2.push_back(it1->second);
			}
			tagi1[licznik]=kodyKey[it1->first];
			tagi2[licznik]=kodyKey[it1->second];
			licznik++;
			it1++;
		}
	}
	bool modify;
	bool todelete;
	every_member()
	{
		tagi1=NULL;
		tagi2=NULL;
		tagiSize=0;
		modify=0;
		todelete=0;
	}
	every_member(every_member& foo)
	{
		tagi1=NULL;
		tagi2=NULL;
		tagiSize=0;
		version=foo.version;
		id=foo.id;
		modify=foo.modify;
		todelete=foo.todelete;
		setTags(foo.getTags());
	}
	every_member(const every_member& foo)
	{
		tagi1=NULL;
		tagi2=NULL;
		tagiSize=0;
		version=foo.version;
		id=foo.id;
		modify=foo.modify;
		todelete=foo.todelete;
		map<string,string> tags=foo.getTags();
		setTags(tags);
	}
	every_member& operator=(every_member const& foo)
	{
		tagi1=NULL;
		tagi2=NULL;
		tagiSize=0;
		version=foo.version;
		id=foo.id;
		modify=foo.modify;
		todelete=foo.todelete;
		map<string,string> tags=foo.getTags();
		setTags(tags);
	
	}
	~every_member()
	{
		if(tagi1!=NULL)
		{
			delete[] tagi1;
			delete[] tagi2;
		}
	}
};

map <string, int> every_member::kodyKey;
vector <string> every_member::kodyKey2;
	
struct node : every_member
{
	double lat;
	double lon;
	node(node& foo) : every_member(foo)
	{
		lat=foo.lat;
		lon=foo.lon;
	}
	node(const node& foo) : every_member(foo)
	{
		lat=foo.lat;
		lon=foo.lon;
	}
	node() : every_member()
	{
	}
};

struct way : every_member
{
	vector <long long> nodes;
	way(way& foo) : every_member(foo)
	{
		nodes=foo.nodes;
	}
	way(const way& foo) : every_member(foo)
	{
		nodes=foo.nodes;
	}
	way() : every_member()
	{
	}
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
	relation(relation& foo) : every_member(foo)
	{
		members=foo.members;
	}
	relation(const relation& foo) : every_member(foo)
	{
		members=foo.members;
	}
	relation() : every_member()
	{
	}
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
			if(it1->second.modify || it1->second.todelete)
				nowa.nodes[it1->first]=it1->second;
			it1++;
		}
		map<long long, way>::iterator it2=ways.begin();
		while(it2!=ways.end())
		{
			if(it2->second.modify || it2->second.todelete)
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
			if(it3->second.modify || it3->second.todelete)
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
		set <long long> nodes_potrzebne;
		new_ways=-1;
		/*
		file <> xmlFile(sciezka.c_str());
		xml_document<> doc;
		doc.parse<0>(xmlFile.data());
		xml_node<>* root=doc.first_node("osm");
		*/
		load_ways(sciezka, nodes_potrzebne);
		load_nodes(sciezka, nodes_potrzebne);
		load_relations(sciezka);
		/*
		for(xml_node <> *tag=root->first_node("way"); tag; tag=tag->next_sibling("way"))
		{
			load_way(tag, nodes_potrzebne);
		}
		*/
		/*
		for(xml_node <> *tag=root->first_node("node"); tag; tag=tag->next_sibling("node"))
		{
			load_node(tag, nodes_potrzebne);
		}
		*/
		/*
		for(xml_node <> *tag=root->first_node("relation"); tag; tag=tag->next_sibling("relation"))
		{
			load_relation(tag);
		}
		*/
	}
	
	private:
	
	void wypisz_tags(every_member& teraz, ostream& plik)
	{
		map<string, string> tags = teraz.getTags();
		map<string, string>::iterator it1=tags.begin();
		while(it1!=tags.end())
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
	void load_tag(Xml::Inspector<Xml::Encoding::Utf8Writer> &ins, map<string, string>& lista)
	{
		string k, v;
		Xml::Inspector<Xml::Encoding::Utf8Writer>::SizeType i;
		for (i = 0; i < ins.GetAttributesCount(); ++i)
		{
			if(ins.GetAttributeAt(i).Name=="k")
				k=ins.GetAttributeAt(i).Value;
			if(ins.GetAttributeAt(i).Name=="v")
				v=ins.GetAttributeAt(i).Value;
		}
		lista[k]=v;
	}
	long long load_nd(Xml::Inspector<Xml::Encoding::Utf8Writer> &ins)
	{
		Xml::Inspector<Xml::Encoding::Utf8Writer>::SizeType i;
		for (i = 0; i < ins.GetAttributesCount(); ++i)
		{
			if(ins.GetAttributeAt(i).Name=="ref")
			{
				long long value=fromstring<long long>(ins.GetAttributeAt(i).Value);
				cout<<value<<endl;
				return value;
			}
		}
		cout<<"0000000000000000000000"<<endl;
		return 0;
	}
	void load_basic(Xml::Inspector<Xml::Encoding::Utf8Writer> &ins, every_member& foo)
	{
		Xml::Inspector<Xml::Encoding::Utf8Writer>::SizeType i;
		for (i = 0; i < ins.GetAttributesCount(); ++i)
		{
			if(ins.GetAttributeAt(i).Name=="id")
				foo.id=fromstring<long long>(ins.GetAttributeAt(i).Value);
			if(ins.GetAttributeAt(i).Name=="version")
				foo.version=fromstring<int>(ins.GetAttributeAt(i).Value);
		}
	}
	void load_member(Xml::Inspector<Xml::Encoding::Utf8Writer> &ins, relation& foo)
	{
		relation_member foo2;
		Xml::Inspector<Xml::Encoding::Utf8Writer>::SizeType i;
		for (i = 0; i < ins.GetAttributesCount(); ++i)
		{
			if(ins.GetAttributeAt(i).Name=="ref")
				foo2.member_id=fromstring<long long>(ins.GetAttributeAt(i).Value);
			if(ins.GetAttributeAt(i).Name=="role")
				foo2.role=(ins.GetAttributeAt(i).Value);
			if(ins.GetAttributeAt(i).Name=="type")
			{
				string type=(ins.GetAttributeAt(i).Value);
				if(type=="node")
					foo2.member_type=NODE;
				if(type=="way")
					foo2.member_type=WAY;
				if(type=="relation")
					foo2.member_type=RELATION;
			}
		}
		foo.members.push_back(foo2);
	}
	void load_latlon(Xml::Inspector<Xml::Encoding::Utf8Writer> &ins, node& foo)
	{
		Xml::Inspector<Xml::Encoding::Utf8Writer>::SizeType i;
		for (i = 0; i < ins.GetAttributesCount(); ++i)
		{
			if(ins.GetAttributeAt(i).Name=="lat")
				foo.lat=fromstring<double>(ins.GetAttributeAt(i).Value);
			if(ins.GetAttributeAt(i).Name=="lon")
				foo.lon=fromstring<double>(ins.GetAttributeAt(i).Value);
		}
	}
	void load_ways(string sciezka, set <long long>& nodes_potrzebne)
	{
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(sciezka.c_str());
		while (inspector.Inspect())
		{
			if (inspector.GetInspected() == Xml::Inspected::StartTag)
			{
				if(inspector.GetName()=="way")
				{
					way foo;
					load_basic(inspector, foo);
					map <string, string> tags;
					bool ok=1;
					bool dodaj=0;
					while (inspector.Inspect() && ok)
					{
						if (inspector.GetInspected() == Xml::Inspected::EndTag)
						{
							if(inspector.GetName() == "way")
							{
								ok=0;
								if(tags.find("highway")!=tags.end())
									dodaj=1;
								if(tags.find("railway")!=tags.end())
									dodaj=1;
								if(tags.find("public_transport")!=tags.end())
									dodaj=1;
								if(dodaj)
								{
									foo.setTags(tags);
									ways[foo.id]=foo;
									for(int i=0; i<foo.nodes.size(); i++)
									{
										nodes_potrzebne.insert(foo.nodes[i]);
									}
								}
							}
						}
						else
						{
							if(inspector.GetName() == "nd")
								foo.nodes.push_back(load_nd(inspector));
							if(inspector.GetName() == "tag")
								load_tag(inspector, tags);	
						}
					}
				}
			}
		}
	}
	void load_nodes(string sciezka, set <long long>& nodes_potrzebne)
	{
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(sciezka.c_str());
		while (inspector.Inspect())
		{
			if (inspector.GetInspected() == Xml::Inspected::StartTag || inspector.GetInspected() == Xml::Inspected::EmptyElementTag)
			{
				if(inspector.GetName()=="node")
				{
					node foo;
					load_basic(inspector, foo);
					load_latlon(inspector, foo);
					map <string, string> tags;
					bool ok=1;
					bool dodaj=0;
					if (inspector.GetInspected() == Xml::Inspected::EmptyElementTag)
					{
						if(inspector.GetName() == "node")
						{
							ok=0;
							if(tags.find("highway")!=tags.end())
								dodaj=1;
							if(tags.find("railway")!=tags.end())
								dodaj=1;
							if(tags.find("public_transport")!=tags.end())
								dodaj=1;
							if(nodes_potrzebne.find(foo.id)!=nodes_potrzebne.end())
								dodaj=1;
							if(dodaj)
							{
								foo.setTags(tags);
								nodes[foo.id]=foo;
							}
						}
					}
					while (inspector.Inspect() && ok)
					{
						if (inspector.GetInspected() == Xml::Inspected::EndTag)
						{
							if(inspector.GetName() == "node")
							{
								ok=0;
								if(tags.find("highway")!=tags.end())
									dodaj=1;
								if(tags.find("railway")!=tags.end())
									dodaj=1;
								if(tags.find("public_transport")!=tags.end())
									dodaj=1;
								if(nodes_potrzebne.find(foo.id)!=nodes_potrzebne.end())
									dodaj=1;
								if(dodaj)
								{
									foo.setTags(tags);
									nodes[foo.id]=foo;
								}
							}
						}
						else
						{
							if(inspector.GetName() == "tag")
								load_tag(inspector, tags);	
						}
					}

				}
			}
		}
	}
	void load_relations(string sciezka)
	{
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(sciezka.c_str());
		while (inspector.Inspect())
		{
			if (inspector.GetInspected() == Xml::Inspected::StartTag)
			{
				if(inspector.GetName()=="relation")
				{
					relation foo;
					load_basic(inspector, foo);
					map <string, string> tags;
					bool ok=1;
					bool dodaj=1;
					while (inspector.Inspect() && ok)
					{
						if (inspector.GetInspected() == Xml::Inspected::EndTag)
						{
							if(inspector.GetName() == "relation")
							{
								ok=0;
								if(dodaj)
								{
									foo.setTags(tags);
									relations[foo.id]=foo;
								}
							}
						}
						else
						{
							if(inspector.GetName() == "tag")
								load_tag(inspector, tags);	
							if(inspector.GetName() == "member")
								load_member(inspector, foo);	
					
						}
					}
				}
			}
		}
	}
};
#endif
