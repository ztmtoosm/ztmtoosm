#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include "ztmosmpor.hpp"
using namespace std;

class WlasciwosciLokalneWarszawa;
class WlasciwosciLokalneGdansk;
class WlasciwosciLokalneSzczecin;
class WlasciwosciLokalneLodz;

class WlasciwosciLokalne
{
	public:
	virtual string nazwaMala(string lineName) = 0;
	virtual string nazwaDuza(string lineName) = 0;
	virtual long long getParentRelation (string name) = 0;
	virtual set <string> wszystkieLinie(ScheduleHandlerInternal* bazaZtm) = 0;
	virtual long long getRootRelation() = 0;
	virtual string getNazwaMiasta() = 0;
	virtual string getRefKey() = 0;
	virtual string getNetworkName() = 0;
	virtual string getUrlLine(string line) = 0;
	virtual string substituteWhiteCharsBySpace(string nazwa)
	{
		if(nazwa=="")
			return nazwa;
		string wynik;
		stringstream plt;
		plt<<nazwa;
		string tmp1;
		vector <string> tmp;
		while(plt>>tmp1)
			tmp.push_back(tmp1);
		for(int i=0; i<tmp.size()-1; i++)
		{
			wynik+=tmp[i];
			if(i<tmp.size()-2)
				wynik+=" ";
		}
		return wynik;
	}
	virtual string getLongNameOfStop(string shortName, string id)
	{
		return shortName;
	}
	static WlasciwosciLokalne& getClassByName(string miasto);
};

class WlasciwosciLokalneWarszawa : public WlasciwosciLokalne
{
	public:
	string nazwaMala(string lineName)
	{
		if(lineName.length()<3)
			return "tram";
		return "bus";
	}
	string nazwaDuza(string lineName)
	{
		if(lineName.length()<3)
			return "Tram";
		return "Bus";
	}
	string getRefKey()
	{
		return "ref";
	}
	long long getParentRelation (string name)
	{
		if(name.length()<=2)
			return 3651333;
		if(name[0]=='1' || name[0]=='2')
			return 3651331;
		if(name[0]=='3')
			return 3651332;
		if(name[0]=='4')
			return 3651328;
		if(name[0]=='5' || name[0]=='E')
			return 3651327;
		if(name[0]=='7' || name[0]=='8')
			return 3651329;
		if(name[0]=='N')
			return 3651326;
		return 4656333;
	}

	set <string> wszystkieLinie(ScheduleHandlerInternal* bazaZtm)
	{
		set <string> wynik;
		auto it1=bazaZtm->dane_linia.begin();
		while(it1!=bazaZtm->dane_linia.end())
		{
			string kand=it1->first;
			if(kand[0]!='K')
			if(kand[0]!='S')
			if(kand[0]!='W')
			{
				wynik.insert(kand);
			}
			it1++;
		}
		return wynik;
	}
	long long getRootRelation()
	{
		return 3651336;
	}
	string getNazwaMiasta()
	{
		return "Warszawa";
	}
	string getNetworkName()
	{
		return "ZTM Warszawa";
	}
	string getUrlLine(string line)
	{
		return "http://ztm.waw.pl/rozklad_nowy.php?c=182&l=1&q="+line;
	}

	string getLongNameOfStop(string shortName, string id)
	{
		if(id.length()==6)
			return shortName+" "+id[4]+id[5];
		return shortName;
	}
};

class WlasciwosciLokalneSzczecin : public WlasciwosciLokalne
{
	public:
	string nazwaMala(string lineName)
	{
			return "tram";
	}
	string nazwaDuza(string lineName)
	{
			return "Tram";
	}
string getRefKey()
	{
		return "ref";
	}
	long long getParentRelation (string name)
	{
		return 5291742;
	}

	set <string> wszystkieLinie(ScheduleHandlerInternal* bazaZtm)
	{
		set <string> wynik;
		auto it1=bazaZtm->dane_linia.begin();
		while(it1!=bazaZtm->dane_linia.end())
		{
			string kand=it1->first;
			wynik.insert(kand);
			it1++;
		}
		return wynik;
	}
	long long getRootRelation()
	{
		return 5291742;
	}
	string getNazwaMiasta()
	{
		return "Szczecin";
	}
	string substituteWhiteCharsBySpace(string nazwa)
	{
		cout<<"KUKU!!!!!!!!!!"<<endl;
		return nazwa;
	}
	string getNetworkName()
	{
		return "ZDiTM Szczecin";
	}
	string getUrlLine(string line)
	{
		return "";
	}
};

class WlasciwosciLokalneGdansk : public WlasciwosciLokalne
{
	public:
	string nazwaMala(string lineName)
	{
			if(lineName.length()==2 && isdigit(lineName[0]))
				return "tram";
			if(lineName.length()==1 && isdigit(lineName[0]))
				return "tram";
			if(lineName=="N0")
				return "tram";
			return "bus";
	}
	string nazwaDuza(string lineName)
	{
			if(lineName.length()==1 && isdigit(lineName[0]))
				return "Tram";
			if(lineName.length()==2 && isdigit(lineName[0]))
				return "Tram";
			if(lineName=="N0")
				return "Tram";
			return "Bus";
	}
	string getRefKey()
	{
		return "ref:ztm";
	}
	long long getParentRelation (string name)
	{
		return 5411513;
	}

	set <string> wszystkieLinie(ScheduleHandlerInternal* bazaZtm)
	{
		set <string> wynik;
		auto it1=bazaZtm->dane_linia.begin();
		while(it1!=bazaZtm->dane_linia.end())
		{
			string kand=it1->first;
			wynik.insert(kand);
			it1++;
		}
		return wynik;
	}
	long long getRootRelation()
	{
		return 5411513;
	}
	string getNazwaMiasta()
	{
		return "Gdańsk";
	}
	string substituteWhiteCharsBySpace(string nazwa)
	{
		return nazwa;
	}
	string getNetworkName()
	{
		return "ZTM Gdańsk";
	}
	string getUrlLine(string line)
	{
		return "http://www.ztm.gda.pl/rozklady/linia-"+line+".html";
	}
};


class WlasciwosciLokalneLodz : public WlasciwosciLokalne
{
	public:
	string nazwaMala(string lineName)
	{
		return "tram";
	}
	string nazwaDuza(string lineName)
	{
		return "Tram";
	}
	string getRefKey()
	{
		return "ref";
	}
	long long getParentRelation (string name)
	{
		return 5467192;
	}

	set <string> wszystkieLinie(ScheduleHandlerInternal* bazaZtm)
	{
		set <string> wynik;
		auto it1=bazaZtm->dane_linia.begin();
		while(it1!=bazaZtm->dane_linia.end())
		{
			string kand=it1->first;
			wynik.insert(kand);
			it1++;
		}
		return wynik;
	}
	long long getRootRelation()
	{
		return 5467192;
	}
	string getNazwaMiasta()
	{
		return "Łódź";
	}
	string substituteWhiteCharsBySpace(string nazwa)
	{
		return nazwa;
	}
	string getNetworkName()
	{
		return "MPK Łódź";
	}
	string getUrlLine(string line)
	{
		return "http://www.mpk.lodz.pl/";
	}
};
