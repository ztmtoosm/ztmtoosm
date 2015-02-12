#include "HafasBaza.hpp"
#include "fcgi_stdio.h"
int main(int argc, char** argv)
{
	string sciezka, sciezka2, sciezka3;
	sciezka="/home/marcin/programowanie/ztmtoosm/ztmtoosm/RA141229.TXT";
	sciezka2="/home/marcin/programowanie/ztmtoosm/ztmtoosm/latest.osm";
	sciezka3="/home/marcin/programowanie/ztmtoosm/ztmtoosm/cudaki.txt";
	HafasBaza* baza=new HafasBaza();
	HafasBazaLoader loader(baza, sciezka);
	OsmBazaLoaderBis loader2(baza, sciezka2, sciezka3);
}
