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
#include "src/szkielet.hpp"
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
	vector <long long> wynik;
	for(int i=0; i<track[0].Size(); i++)
	{
		wynik.push_back(track[0][i].GetInt64());
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
	Generator(Value& v)
	{
		set<pair<long long, long long> > pary;
		map <long long, relation> tempRel;
		map <long long, vector <long long> > tempTracks;
		set <long long> allNodes;
		for(int i=0; i<v.Size(); i++)
		{
			long long id = v[i]["id"].GetInt64();
			cout<<"uuu"<<endl;
			relation rel;
			rel.modify = true;
			rel.setTags(getTags(v[i]["tags"]));
			cout<<"xxx"<<endl;
			rel.members = getMembers(v[i]["members"]);
			tempRel[id]=rel;
			tempTracks[id]=getFinalTrack(v[i]["finaltrack"]);
			cout<<"zzz"<<endl;
			vector <long long> dupa=getFinalTrack(v[i]["finaltrack"]);
			for(int j=0; j<dupa.size()-1; j++)
			{
				pary.insert(make_pair(dupa[j], dupa[j+1]));
			}
			cout<<dupa.size()<<endl;
			allNodes.insert(dupa.begin(), dupa.end());
			cout<<"xxxx"<<endl;
		}
		osm_base baza(allNodes, pary);
		Szkielet szkielet(&baza);
		for(auto& it1 : tempRel)
		{
			baza.relations[it1.first] = it1.second;
		}
		for(auto& it1 : tempTracks)
		{
			vector <long long> xD = szkielet.kolejneDrogiFull(it1.second);
			for(int i=0; i<xD.size(); i++)
			{
				relation_member foo;
				foo.member_id = xD[i];
				foo.member_type = WAY;
				foo.role = "route";
				baza.relations[it1.first].members.push_back(foo);
			}
		}
		baza.wypisz("ngal.osm");
	}
};

int main()
{
	string lol;
	stringstream pagename;
	pagename<<"xdd.txt";
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
	Generator gen(d);
}
