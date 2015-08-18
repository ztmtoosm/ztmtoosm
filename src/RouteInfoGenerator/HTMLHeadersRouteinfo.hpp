#ifndef HTMLHEADERSROUTEINFO
#define HTMLHEADERSROUTEINFO
#include <fstream>
#include <sstream>
#include <map>
#include <fstream>
using namespace std;
void htmlHead(fstream& plik5);
void htmlTile(fstream& plik5, bool rsstest);
void uzupelnij(fstream& plik5, string path);
bool okVariable(string str);
struct HtmlExtraGenerator
{
	map<string, string> loadedTemplates;
	map<int, string> loadedVariables;
	string loadTemplate(string path);
};
#endif
