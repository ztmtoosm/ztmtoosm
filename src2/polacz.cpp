#include "readztm.hpp"
#include <map>
#include <fstream>
#include <algorithm>
using namespace std;
const double ODL_ROWN=2;
vector <string> pomin_podobne(vector <string> przed)
{
	vector <string> wynik;
	for(int i=0; i<przed.size(); i++)
		if(wynik.size()==0 || wynik[wynik.size()-1]!=przed[i])
			wynik.push_back(przed[i]);
	return wynik;
}
fstream plik;
string nowy2(string id)
{
	string id2;
	for(int i=0; i<id.length()-2; i++)
	{
		id2+=id[i];
	}
	return id2;
}


string magluj_easy(set <string> lin)
{
	string wynik;
	set<string>::iterator it1=lin.begin();
	while(it1!=lin.end())
	{
		wynik+=*it1+";";
		it1++;
	}
	return wynik;
}
string wy_posr(vector <string> lin)
{
	string wynik;
	vector<string>::iterator it1=lin.begin();
	while(it1!=lin.end())
	{
		wynik+=*it1+";";
		it1++;
	}
	return wynik;
}
vector <string> rozdzielSredniki(string tekst)
{
	vector <string> wynik;
	string pusty;
	int s1=tekst.length();
	if(s1==0)
		return wynik;
	wynik.push_back(pusty);
	for(int i=0; i<s1-1; i++)
	{
		if(tekst[i]==';')
		{
			wynik.push_back(pusty);
		}
		else
		{
			wynik[wynik.size()-1]+=tekst[i];
		}
	}
	return wynik;
}
string readUstawienia(string nazwaZTorEdit, string key)
{
	string wynikPusty;
	cout<<"PUS "<<nazwaZTorEdit<<endl;
	char data[10000];
	cin.getline(data, 10000);
	vector <string> fro=rozdziel(data);
	for(int i=1; i<fro.size(); i+=2)
	{
		if(fro[i]==key)
			return fro[i+1];
	}
		return wynikPusty;
}


vector <string> get_posrednie(string name)
{
	return rozdzielSredniki(readUstawienia(name, "posrednie"));
}

void polacz(string name)
{
	char data[10000];
	cout<<"WYCH "<<name<<endl;
	cin.getline(data, 10000);
	vector <string> wych_lista=rozdziel(data);
	if(wych_lista.size()<3)
		return;
	string id1=wych_lista[1];
	string id2=wych_lista[2];
	cout<<"KSS "<<id1<<endl;
	cin.getline(data, 10000);
	vector <string> ss1=rozdziel(data);
	vector <string> posr1=get_posrednie(id1);
	cout<<"KSS "<<id2<<endl;
	cin.getline(data, 10000);
	vector <string> ss2=rozdziel(data);
	vector <string> posr2=get_posrednie(id2);
	string start=ss1[1];
	string stop=ss2[2];
	if(ss1[1]==name)
	{
		start=ss1[2];
		reverse(posr1.begin(), posr1.end());
	}
	if(ss2[2]==name)
	{
		stop=ss2[1];
		reverse(posr2.begin(), posr2.end());
	}
	string id=readUstawienia(name, "id");
	vector <string> posr_wynik;
	posr_wynik.insert(posr_wynik.end(), posr1.begin(), posr1.end());
	posr_wynik.push_back(id);
	posr_wynik.insert(posr_wynik.end(), posr2.begin(), posr2.end());
	cout<<"SLN "<<start<<" "<<stop<<endl;
	cin.getline(data, 10000);
	string nowaNazwa=rozdziel(data)[1];
	cout<<"DEL "<<id1<<endl;
	cin.getline(data, 10000);
	cout<<"DEL "<<id2<<endl;
	cin.getline(data, 10000);
	cout<<"SPP "<<nowaNazwa<<" posrednie "<<wy_posr(posr_wynik)<<endl;
	cin.getline(data, 10000);
	cout<<"DEL "<<name<<endl;
	cin.getline(data, 10000);
}

int main(int argc, char** argv)
{
	char data[30000];
	cout<<"PE3 X11_set yes"<<endl;
	cin.getline(data, 3000);
	vector <string> rzd=rozdziel(data);
	for(int i=1; i<rzd.size(); i++)
	{
		polacz(rzd[i]);
	}
}
