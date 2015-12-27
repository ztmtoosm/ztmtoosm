#include "WlasciwosciLokalne.cpp"

static WlasciwosciLokalne& WlasciwosciLokalne::getClassByName(string miasto)
{
	if(miasto=="Warszawa")
		return WlasciwosciLokalneWarszawa();
	if(miasto=="Szczecin")
		return WlasciwosciLokalneSzczecin();
	if(miasto=="Gdańsk")
		return WlasciwosciLokalneGdansk();
	if(miasto=="Łódź")
		return WlasciwosciLokalneLodz();
	return WlasciwosciLokalne();
}
