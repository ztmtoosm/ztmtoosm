#ifndef HTMLGEN
#define HTMLGEN
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

void htmlHead(fstream& plik5)
{
	plik5<<"<!doctype html>"<<endl;
	plik5<<"<html lang=\"pl\">"<<endl;
	plik5<<"<head>"<<endl;
	plik5<<"<link rel=\"stylesheet\" href=\"http://openlayers.org/en/v3.0.0/css/ol.css\" type=\"text/css\">"<<endl;
	plik5<<"<link rel=\"stylesheet\" href=\"ter.css\" type=\"text/css\">"<<endl;
	plik5<<"<script src=\"http://openlayers.org/en/v3.0.0/build/ol.js\" type=\"text/javascript\"></script>"<<endl;
	plik5<<"</head>"<<endl;
	plik5<<"<body>"<<endl;
	plik5<<"<div id=\"map\" class=\"map\"></div>"<<endl;
	plik5<<"<div id=\"hdd\">"<<endl;
}
void htmlTile(fstream& plik5)
{
	plik5<<"</div>"<<endl;
	plik5<<"<script src=\"nasz.js\" type=\"text/javascript\"></script>"<<endl;
	plik5<<"</body>"<<endl;
	plik5<<"</html>"<<endl;
			
}
struct htmlgen
{
	static string zlozTabele(string tableclass, string lineclass, string cellclass, vector <vector <string> > tabela)
	{
		string wynik;
		int s1=tabela.size();
		for(int i=0; i<s1; i++)
		{
			string wynik0;
			for(int j=0; j<tabela[i].size(); j++)
			{
				wynik0+=div(cellclass, "", tabela[i][j]);
			}
			wynik+=div(lineclass, "", wynik0);
		}
		return div(tableclass, "", wynik);
	}
	static void zlozHtml(string content, string file_out, string css_file)
	{
		fstream plik(file_out.c_str(), fstream::trunc | fstream::out);
		string wynik;
		wynik+="<!DOCTYPE html>\n<html lang=\"pl\">\n<head> <meta charset=\"utf8\">";
		wynik+="<link rel=\"stylesheet\" type=\"text/css\" href=\""+css_file+"\">";
		wynik+="</head>\n<body>\n";
		wynik+=content+"\n";
		wynik+="</body></html>";
		plik<<wynik<<endl;
		plik.close();
	}
	static string div(string classname, string id, string info)
	{
		string wynik;
		wynik+="<div class=\"";
		wynik+=classname;
		wynik+="\" id=\"";
		wynik+=id;
		wynik+="\">";
		wynik+=info;
		wynik+="</div>\n";
		return wynik;
	}
	static string link(string link, string info)
	{
		string wynik;
		wynik+="<a href=\"";
		wynik+=link;
		wynik+="\">";
		wynik+=info;
		wynik+="</a>";
		return wynik;
	}
};

#endif
