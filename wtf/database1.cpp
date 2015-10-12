#include "src/HafasBaza.hpp"
int main(int argc, char** argv)
{
	string podstawa=argv[1];
	string sciezka, sciezka2, sciezka3;
	sciezka=podstawa+"/data/ztm_download1.txt";
	sciezka2=podstawa+"/data/latest.osm";
	sciezka3=podstawa+"/data/sciezka.txt";
	HafasBaza* baza=new HafasBaza();
	HafasBazaLoader loader(baza, sciezka);
	OsmBazaLoaderBis loader2(baza, sciezka2, sciezka3);
}
