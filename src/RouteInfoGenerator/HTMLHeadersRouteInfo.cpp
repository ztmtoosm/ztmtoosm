#include "HTMLHeadersRouteinfo.hpp"
void htmlHead(fstream& plik5)
{
	plik5<<"<!doctype html>"<<endl;
	plik5<<"<html lang=\"pl\">"<<endl;
	plik5<<"<meta charset=\"utf-8\">"<<endl;
	plik5<<"<head>"<<endl;
	plik5<<"<link rel=\"stylesheet\" href=\"http://openlayers.org/en/v3.0.0/css/ol.css\" type=\"text/css\">"<<endl;
	plik5<<"<link rel=\"stylesheet\" href=\"ter.css\" type=\"text/css\">"<<endl;
	plik5<<"<script src=\"http://openlayers.org/en/v3.0.0/build/ol.js\" type=\"text/javascript\"></script>"<<endl;
	plik5<<"</head>"<<endl;
	plik5<<"<body>"<<endl;
//	plik5<<"<div id=\"map\" class=\"map\"></div>"<<endl;
	plik5<<"<div id=\"hdd\">"<<endl;
}
void htmlTile(fstream& plik5)
{
	plik5<<"</div>"<<endl;
	plik5<<"<script src=\"nasz.js\" type=\"text/javascript\"></script>"<<endl;
	plik5<<"</body>"<<endl;
	plik5<<"</html>"<<endl;

}
