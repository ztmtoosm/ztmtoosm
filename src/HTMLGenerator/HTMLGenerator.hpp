#ifndef HTMLGENERATOR
#define HTMLGENERATOR
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
using namespace std;
struct htmlgen
{
	static string zlozTabele(string tableclass, string lineclass, string cellclass, vector <vector <string> > tabela);
	static void zlozHtml(string content, string file_out, string css_file);
	static string div(string classname, string id, string info);
	static string link(string link, string info);
};
#endif
