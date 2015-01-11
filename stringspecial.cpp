#include "stringspecial.hpp"
string avoid_cudzyslow(string slowo)
	{
		string wynik;
		int s1=slowo.length();
		for(int i=0; i<s1; i++)
		{
			if(slowo[i]!='"' && slowo[i]!='&' && slowo[i]!='<' && slowo[i]!='>' && slowo[i]!='\'')
			{
				wynik+=slowo[i];
			}
			else
			{
				if(slowo[i]=='"')
					wynik+="&quot;";
				if(slowo[i]=='&')
					wynik+="&amp;";
				if(slowo[i]=='<')
					wynik+="&lt;";
				if(slowo[i]=='>')
					wynik+="&gt;";
				if(slowo[i]=='\'')
					wynik+="&apos;";
		
			}
		}
		return wynik;
	}
string substituteWhiteCharsBySpace(string nazwa)
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

int toint(string dat)
{
	stringstream dd;
	dd<<dat;
	int lol;
	dd>>lol;
	return lol;
}
string tostring(int alfa)
{
	stringstream str;
	str<<alfa;
	string wynik;
	str>>wynik;
	return wynik;
}
double distance(double long1, double lat1, double long2, double lat2)
{
	double d2r = (M_PI / 180.0);
	double dlong = (long2 - long1) * d2r;
	double dlat = (lat2 - lat1) * d2r;
	double a = pow(sin(dlat/2.0), 2) + cos(lat1*d2r) * cos(lat2*d2r) * pow(sin(dlong/2.0), 2);
	double c = 2 * atan2(sqrt(a), sqrt(1-a));
	double d = 6367 * c;
	return d;
}

double distance(long long laong1, long long laat1, long long laong2, long long laat2)
{
	double long1=laong1/1000000.0;
	double lat1=laat1/1000000.0;
	double long2=laong2/1000000.0;
	double lat2=laat2/1000000.0;
	double d2r = (M_PI / 180.0);
	double dlong = (long2 - long1) * d2r;
	double dlat = (lat2 - lat1) * d2r;
	double a = pow(sin(dlat/2.0), 2) + cos(lat1*d2r) * cos(lat2*d2r) * pow(sin(dlong/2.0), 2);
	double c = 2 * atan2(sqrt(a), sqrt(1-a));
	double d = 6367 * c;
	d*=1000;
	return d;
}

int stringtoint(string abc)
{
	stringstream foo;
	foo<<abc;
	int wynik;
	foo>>wynik;
	return wynik;
}

double stringtodouble(string abc)
{
	stringstream foo;
	foo<<abc;
	double wynik;
	foo>>wynik;
	return wynik;
}
string inttostring (int abc)
{
	stringstream foo;
	foo<<abc;
	string wynik;
	foo>>wynik;
	return wynik;
}
string doubletostring (double abc)
{
	stringstream foo;
	foo<<abc;
	string wynik;
	foo>>wynik;
	return wynik;
}
vector <string> rozdziel(char* slowo)
{
	stringstream tmp;
	tmp<<slowo;
	string skldowa;
	vector <string> wynik;
	while(tmp>>skldowa)
	{
		wynik.push_back(skldowa);
	}
	return wynik;
}
double todouble(string data)
{
	stringstream ss;
	ss<<data;
	double wyn;
	ss>>wyn;
	return wyn;
}

int get_times (string time)
{
	string godz;
	string min;
	int i=0;
	while(time[i]!=':' && time[i]!='.')
	{
		godz+=time[i];
		i++;
	}
	i++;
	for(;i<time.length(); i++)
	{
		min+=time[i];
	}
	return stringtoint(godz)*60+stringtoint(min);
}
string godz(int godzina)
{
	godzina/=60;
	stringstream alfa;
	int minuta=godzina%60;
	godzina/=60;
	if(godzina<10)
		alfa<<"0";
	alfa<<godzina;
	alfa<<":";
	if(minuta<10)
		alfa<<"0";
	alfa<<minuta;
	string wynik;
	alfa>>wynik;
	return wynik;
}

