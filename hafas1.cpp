#include "HafasBaza.hpp"
#include "fcgi_stdio.h"
void wypisz(stringstream& lol)
{
	printf("%s", lol.str().c_str());
}
map <string, string> mapaenv()
{
	map<string, string> wynik;
	string podstawa=getenv("QUERY_STRING");
	int s1=podstawa.length();
	int i=0;
	while(i<s1)
	{
		string key, value;
		while(podstawa[i]!='=' && i<s1)
		{
			key+=podstawa[i];
			i++;
		}
		i++;
		while(podstawa[i]!='&' && i<s1)
		{
			value+=podstawa[i];
			i++;
		}
		i++;
		wynik[key]=value;
	}
	return wynik;
}
int main(int argc, char** argv)
{
	string sciezka, sciezka2;
	sciezka="/home/marcin/programowanie/ztmtoosm/ztmtoosm/RA141229.TXT";
	sciezka2="/home/marcin/programowanie/ztmtoosm/ztmtoosm/latest.osm";
	cout<<"ok1"<<endl;
	HafasBaza* baza=new HafasBaza();
	HafasBazaLoader loader(baza, sciezka);
	cout<<"ok2"<<endl;
	//OsmBazaLoader loader2(baza, sciezka2);
	baza->wypelnij_sciezki();
	//baza->dijkstra_print(argv[1], argv[2], time(NULL), cout);
	//return 0;
	while(FCGI_Accept() >= 0)
	{
		cout<<"elo"<<endl;
		stringstream wyp;
		wyp.precision(9);
		wyp<<"Content-type: application/json\n\n";
		map<string, string>env=mapaenv();
		int tryb=0;
		if(env["line"]!="")
			tryb=1;
		if(tryb==1)
		{
		//	baza->wypiszRoute(env["line"], wyp);
		}
		if(tryb==0)
		{
			if(env["from"]=="")
			{
				env["from"]="701301";
			}
			if(env["to"]=="")
			{
				env["to"]="122201";
			}
			if(env["time"]=="")
			{
				env["time"]="38520";
			}
			baza->dijkstra_print(env["from"], env["to"], stringtoint(env["time"]), wyp);
		}
		wypisz(wyp);
	}
}
