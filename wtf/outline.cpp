#include <map>
#include "stringspecial.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <expat.h>
using namespace std;
#if defined(__amigaos__) && defined(__USE_INLINE__)
#include <proto/expat.h>
#endif

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define BUFFSIZE        8192

vector <string> waznetagi;
vector <string> akt_nodes;
map <string, string> akt_tags;
string akt_typ;
fstream plikt;
char Buff[BUFFSIZE];

struct punkt
{
	double lat;
	double lon;
};
string akt_id;
map <string, punkt> punkty;
static void XMLCALL
start(void *data, const char *el, const char **attr)
{
	int i;
  string element=el;
  if(element=="node" || element=="way")
  {
	  akt_typ=element;
  }
  if(akt_typ=="node" && element=="node")
  {
	  string id;
	  punkt nowy;
	for (i = 0; attr[i]; i += 2) {
	 string key=attr[i];
	 string value=attr[i+1];
	  if(key=="lat")
	  {
		  stringstream foo;
		  foo<<value;
		  foo>>nowy.lat;
	  }
	 if(key=="id")
	  {
		  id=value;
	  }
	  if(key=="lon")
	  {
		 stringstream foo;
		  foo<<value;
		  foo>>nowy.lon;
	  }
	  }
	  punkty[id]=nowy;
  }
  if(akt_typ=="way" && element=="way")
  {
	for (i = 0; attr[i]; i += 2) {
	 string key=attr[i];
	 string value=attr[i+1];
	  if(key=="id")
	  {
		  akt_id=value;
	  }
	  }

  }
  if(akt_typ=="way" && element=="tag")
  {
	  string k, v;
	for (i = 0; attr[i]; i += 2) {
	 string key=attr[i];
	 string value=attr[i+1];
	  if(key=="k")
	  {
		  k=value;
	  }
	  if(key=="v")
	  {
		  v=value;
	  }
	  }
	  akt_tags[k]=v;
  }
  if(akt_typ=="way" && element=="nd")
  {
	for (i = 0; attr[i]; i += 2) {
	 string key=attr[i];
	 string value=attr[i+1];
	  if(key=="ref")
	  {
		  akt_nodes.push_back(value);
	  }
  	}
  }
}

static void XMLCALL
end(void *data, const char *el)
{
	string element = el;
	if(element=="way" || element=="node")
	{
		akt_typ="";
	}
	if(element=="way")
	{
		bool way1=1;
		bool way2=1;
		double przelicznik=-1;
		if(akt_tags["oneway"]=="yes")
			way2=0;
		if(akt_tags["oneway"]=="-1")
			way1=0;
		if(akt_tags["highway"]=="motorway")
			way2=0;
		if(akt_tags["junction"]=="roundabout")
			way2=0;
		if(akt_tags["highway"]=="motorway_link")
			way2=0;
		if(akt_tags["oneway"]=="no")
			way2=1;
		if(akt_tags.find("highway")!=akt_tags.end())
		{
			//przelicznik=4;
			if(akt_tags["highway"]=="motorway")
				przelicznik=0.8;
			if(akt_tags["highway"]=="motorway_link")
				przelicznik=1;
			if(akt_tags["highway"]=="trunk")
				przelicznik=0.8;
			if(akt_tags["highway"]=="trunk_link")
				przelicznik=1;
			if(akt_tags["highway"]=="primary")
				przelicznik=1;
			if(akt_tags["highway"]=="primary_link")
				przelicznik=1;
			if(akt_tags["highway"]=="secondary")
				przelicznik=1;
			if(akt_tags["highway"]=="secondary_link")
				przelicznik=1;
			if(akt_tags["highway"]=="tertiary")
				przelicznik=1;
			if(akt_tags["highway"]=="tertiary_link")
				przelicznik=1;
			if(akt_tags["highway"]=="residential")
				przelicznik=1.2;
			if(akt_tags["highway"]=="unclassified")
				przelicznik=1.2;
			if(akt_tags["highway"]=="service")
				przelicznik=1.2;
			if(akt_tags["highway"]=="living_street")
				przelicznik=1.2;
	
		}
		if(przelicznik>0)
		{
			int s1=akt_nodes.size();
			for(int i=0; i<s1-1; i++)
			{
				double distance_abs=distance(punkty[akt_nodes[i]].lon, punkty[akt_nodes[i]].lat, punkty[akt_nodes[i+1]].lon, punkty[akt_nodes[i+1]].lat)*przelicznik;
				if(way1)
					cout<<akt_nodes[i]<<" "<<akt_nodes[i+1]<<" "<<akt_id<<" "<<distance_abs<<endl;
				if(way2)
					cout<<akt_nodes[i+1]<<" "<<akt_nodes[i]<<" "<<akt_id<<" "<<distance_abs<<endl;
			}
		}
		akt_nodes.clear();
		akt_tags.clear();
	
	}
}

int
main(int argc, char *argv[])
{
	plikt.open(argv[1], ios::out | ios::trunc);
  XML_Parser p = XML_ParserCreate(NULL);
  if (! p) {
    fprintf(stderr, "Couldn't allocate memory for parser\n");
    exit(-1);
  }

  XML_SetElementHandler(p, start, end);

  for (;;) {
    int done;
    int len;

    len = (int)fread(Buff, 1, BUFFSIZE, stdin);
    if (ferror(stdin)) {
      fprintf(stderr, "Read error\n");
      exit(-1);
    }
    done = feof(stdin);

    if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
      fprintf(stderr, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n",
              XML_GetCurrentLineNumber(p),
              XML_ErrorString(XML_GetErrorCode(p)));
      exit(-1);
    }

    if (done)
      break;
  }
  XML_ParserFree(p);
  plikt.precision(9);
  map<string, punkt>::iterator it1=punkty.begin();
  while(it1!=punkty.end())
  {
	  plikt<<(it1->first)<<" "<<(it1->second).lat<<" "<<(it1->second).lon<<endl;
  	it1++;
  }
  plikt.close();
  return 0;
}
