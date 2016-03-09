#include "ztmosmpor.hpp"

ScheduleHandlerInternal::ScheduleHandlerInternal (string sciez, string type="Warszawa")
{
	cout<<type<<endl;
	ScheduleReader* nowo;
	if(type=="Warszawa")
		nowo = new ScheduleReaderWarszawa(sciez, this);
	if(type=="Gdańsk")
	{
		cout<<type<<endl;
		nowo = new ScheduleReaderGdansk (sciez, this);
	}
	if(type=="Szczecin")
	{
		nowo = new ScheduleReaderSzczecin (sciez, this);
	}
	if(type=="Łódź")
	{
		nowo = new ScheduleReaderLodz (sciez, this);
	}
	nowo->run();
}
void ScheduleHandlerInternal::nowy_przystanek(przystanek nowy)
{
	przystanki[nowy.id]=nowy;
}
void ScheduleHandlerInternal::nowa_linia(string nazwa, vector <vector <string> > trasy)
{
	dane_linia[nazwa]=trasy;
}


bool todigits(int nasze, string nastepne)
{
	stringstream zuo;
	zuo<<nastepne;
	int dati;
	zuo>>dati;
	if(nasze%100==dati)
		return true;
	return false;
}
