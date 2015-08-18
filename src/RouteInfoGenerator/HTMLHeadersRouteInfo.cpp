#include "HTMLHeadersRouteinfo.hpp"
#include <vector>
void htmlHead(fstream& plik5)
{
	plik5<<"<!doctype html>"<<endl;
	plik5<<"<html lang=\"pl\">"<<endl;
	plik5<<"<meta charset=\"utf-8\">"<<endl;
	plik5<<"<head>"<<endl;
	plik5<<"<link rel=\"stylesheet\" href=\"ter.css\" type=\"text/css\">"<<endl;
	plik5<<"<script src=\"jq.js\" type=\"text/javascript\"></script>"<<endl;
	plik5<<"<script src=\"jfeed.js\" type=\"text/javascript\"></script>"<<endl;
	plik5<<"</head>"<<endl;
	plik5<<"<body>"<<endl;
//	plik5<<"<div id=\"map\" class=\"map\"></div>"<<endl;
	plik5<<"<div id=\"hdd\">"<<endl;
}
void htmlTile(fstream& plik5, bool rsstest)
{
	plik5<<"</div>"<<endl;
	plik5<<"<script src=\"nasz.js\" type=\"text/javascript\"></script>"<<endl;
	if(rsstest)
		plik5<<"<script src=\"rss-test.js\" type=\"text/javascript\"></script>"<<endl;
	plik5<<"</body>"<<endl;
	plik5<<"</html>"<<endl;

}
void uzupelnij(fstream& plik5, string path)
{
	std::ifstream fileStream(path.c_str());
	std::string str((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
	plik5<<str;
	fileStream.close();
}

string HtmlExtraGenerator::loadTemplate(string path)
{
	string str;
	if(loadedTemplates.find(path)==loadedTemplates.end())
	{
		std::ifstream fileStream(path.c_str());
		std::string str2((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
		fileStream.close();
		str=str2;
		loadedTemplates[path]=str;
	}
	else
	{
		str=loadedTemplates[path];
	}
	stringstream foo;
	foo<<str;
	vector<string> tmp;
	string tmp2;
	while(getline(foo,tmp2,'$'))
	{
		tmp.push_back(tmp2);
	}
	string wynik;
	for(int i=0; i<tmp.size(); i++)
	{
		if(i%2==1 && okVariable(tmp[i]))
		{
			stringstream foo2;
			foo2<<tmp[i];
			int var;
			foo2>>var;
			wynik+=loadedVariables[var];
		}
		else
		{
			wynik+=tmp[i];
		}
	}
	return wynik;
}
bool okVariable(string str)
{
	for(int i=0; i<str.length(); i++) {
		if(!isdigit(str[i]))
			return false;
	}
	return true;
}
