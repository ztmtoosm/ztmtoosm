#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include<iostream>
#include <vector>
#include <fstream>
#include <streambuf>
#include <map>
#include <climits>
#include <ctime>
#include <set>
#include <sstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "szkielet.hpp"
#include "conf.h"
using namespace rapidjson;
using namespace std;
class IStreamWrapper {
public:
typedef char Ch;
IStreamWrapper(std::istream& is) : is_(is) {
}
Ch Peek() const { // 1
int c = is_.peek();
return c == std::char_traits<char>::eof() ? '\0' : (Ch)c;
}
Ch Take() { // 2
int c = is_.get();
return c == std::char_traits<char>::eof() ? '\0' : (Ch)c;
}
size_t Tell() const { return (size_t)is_.tellg(); } // 3
Ch* PutBegin() { assert(false); return 0; }
void Put(Ch) { assert(false); }
void Flush() { assert(false); }
size_t PutEnd(Ch*) { assert(false); return 0; }
private:
IStreamWrapper(const IStreamWrapper&);
IStreamWrapper& operator=(const IStreamWrapper&);
std::istream& is_;
};

map <string, string> getTags(Value& tags)
{
	map <string, string> wynik;
	for(int i=0; i<tags.Size(); i++)
	{
		string k = tags[i]["key"].GetString();
		string v = tags[i]["value"].GetString();
		wynik[k]=v;
	}
	return wynik;
}

vector <long long> getFinalTrack (Value& track)
{
	cout<<"--x"<<endl;
	vector <long long> wynik;
	for(int i=0; i<track[0].Size(); i++)
	{
		wynik.push_back(track[0][i].GetInt64());
	}
	cout<<wynik.size()<<endl;
	return wynik;
}

vector <long long> getParentRels (Value& members)
{
	vector <long long> wynik;
	for(int i=0; i<members.Size(); i++)
	{
		wynik.push_back(members[i].GetInt64());
	}
	return wynik;
}
vector <relation_member> getMembers (Value& members)
{
	vector <relation_member> wynik;
	for(int i=0; i<members.Size(); i++)
	{
		relation_member nowa;
		nowa.member_id=fromstring<long long>(members[i]["id"].GetString());
		nowa.role =members[i]["role"].GetString();
		string category = members[i]["category"].GetString();
		if(category=="N")
			nowa.member_type = NODE;
		if(category=="W")
			nowa.member_type = WAY;
		if(category=="R")
			nowa.member_type = RELATION;
		wynik.push_back(nowa);
	}
	return wynik;
}

struct Generator
{
	bool ok;
	Generator(Value& v, string tim)
	{
		Przelicznik* przelicznik = NULL;
		ok=1;
		set<pair<long long, long long> > idOfTrackNodes;
		map <long long, relation> tempRel;
		map <long long, vector <long long> > tempTracks;
		vector <long long> allNodes;
		set <long long> toDel;
		set <long long> parRels;
		map <long long, vector <long long> > parRels2;
		for(int i=0; i<v.Size(); i++)
		{
			if(v[i]["todelete"]!=NULL && v[i]["todelete"].GetBool())
			{
				toDel.insert(v[i]["id"].GetInt64());	
			}
			else
			{
				if(przelicznik==NULL)
				{
					string track_type = v[i]["track_type"].GetString();
					if(track_type=="bus")
						przelicznik = new PrzelicznikBus();
					if(track_type=="tram")
						przelicznik = new PrzelicznikTram();
				}
				long long id = v[i]["id"].GetInt64();
				relation rel;
				rel.id = id;
				rel.version = 0;
				rel.modify = true;
				rel.setTags(getTags(v[i]["tags"]));
				rel.members = getMembers(v[i]["members"]);
				tempRel[id]=rel;
				auto tmp5=getParentRels(v[i]["parentrel"]);
				parRels.insert(tmp5.begin(), tmp5.end());
				parRels2[id]=tmp5;
				tempTracks[id]=getFinalTrack(v[i]["finaltrack"]);
				vector <long long> dupa=getFinalTrack(v[i]["finaltrack"]);
				cout<<(dupa.size()-1)<<endl;
				for(int j=0; j<(signed long long)(dupa.size()-1); j++)
				{
					idOfTrackNodes.insert(make_pair(dupa[j], dupa[j+1]));
				}
				if(dupa.size()>0)
					allNodes.insert(allNodes.end(), dupa.begin(), dupa.end());
			}
		}
		osm_base baza(allNodes, idOfTrackNodes, false);
		Szkielet szkielet(&baza, przelicznik);
		for(auto& it1 : parRels)
		{
			if(it1>0 && baza.relations.find(it1)==baza.relations.end())
			{
				baza.load_relation(it1);
			}
		}
		for(auto& it1 : tempRel)
		{
			if(it1.first>0)
			{
				baza.load_relation(it1.first);
				it1.second.version=baza.relations[it1.first].version;
			}
			baza.relations[it1.first] = it1.second;
			for(auto& it2 : parRels2[it1.first])
			{
				if(!baza.relations[it2].szukajRel(it1.first))
				{
					relation_member foo;
					foo.member_id = it1.first;
					foo.member_type = RELATION;
					foo.role = "";
					baza.relations[it2].members.push_back(foo);
					baza.relations[it2].modify=true;
				}
			}
		}
		for(auto& it1 : toDel)
		{
			baza.load_relation(it1);
			baza.relations[it1].todelete=true;
		}
		for(auto& it1 : tempTracks)
		{
			vector <long long> xD = szkielet.kolejneDrogiFull(it1.second);
			if(xD.size()==0)
				ok = false;
			for(int i=0; i<xD.size(); i++)
			{
				relation_member foo;
				foo.member_id = xD[i];
				foo.member_type = WAY;
				foo.role = "";
				baza.relations[it1.first].members.push_back(foo);
			}
		}
		stringstream pagename;
		pagename<<MYPROJECT_DIR<<"/www/"<<tim<<".osm";
		baza.wypisz(pagename.str());
		ok = szkielet.ok;
	}
};

int main(int argc, char** argv)
{
	string lol;
	stringstream pagename;
	pagename<<MYPROJECT_DIR<<"/www/"<<argv[1]<<".json";
	std::ifstream t(pagename.str().c_str());
	std::string str;
	t.seekg(0, std::ios::end);   
	lol.reserve(t.tellg());
	t.seekg(0, std::ios::beg);
	lol.assign((std::istreambuf_iterator<char>(t)),
	            std::istreambuf_iterator<char>());
	stringstream stt;
	stt<<lol;
	IStreamWrapper is(stt);
	Document d;
	d.ParseStream(is);
	string tim =argv[1];
	Generator gen(d, tim);
	return (int)(gen.ok);
}
