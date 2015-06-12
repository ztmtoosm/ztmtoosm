#ifndef SCHEDULEREADER
#define SCHEDULEREADER
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include "stringspecial.hpp"
using namespace std;
enum typ_postoju
{
	ZWYKLY, ZJAZDOWY, KONCOWY
};
struct postoj
{
	string stop_id;
	int time;
	typ_postoju typ;
};
struct kurs
{
	string id;
	string linia;
	vector <postoj> postoje;
	string dni;
};
struct przystanek
{
	string name;
	string id;
	string miejscowosc;
	double lon;
	double lat;
	string stopinfo;
};

class ScheduleHandler
{
	public:
	virtual void nowy_kurs(kurs nowy) {}
	virtual void nowy_przystanek(przystanek nowy) {}
	virtual void nowa_linia(string nazwa, vector <vector <string> > trasy) {}
	virtual void newkalendar(string date, vector<string>caltypes) {}
};

class ScheduleReader
{
	protected:
	string sciez;
	ScheduleHandler* hand;
	public:
	virtual void run() {}
	ScheduleReader (string sciezka, ScheduleHandler* handler) : hand(handler), sciez(sciezka) {}
};

class ScheduleReaderZtm : public ScheduleReader
{
	public:
	ScheduleReaderZtm (string sciezka, ScheduleHandler* handler) : ScheduleReader(sciezka, handler) {}
	void run();
	private:
	string doprzecinka(stringstream& ttt);
	bool porpolskie(string tekst, int i, string znak);
	bool polskiznak(string tekst, int i);	string znakmale(string tekst, int i);
	string duzemale(string skrot);
	string zaprzecinek(stringstream& ttt);
	void readobszar(string nazwa, fstream& plik);
	void readka(string nazwa, fstream& plik);
	void readwk(string nazwa, fstream& plik, string nazwa2);

	void pominlinie(int liczba, fstream& plik);
	string podkr(char* data);
	void readll(string nazwa, fstream& plik);
	string searchdigits(string data2);
	vector <string> readlw(string nazwa, fstream& plik);
	void readpr(string nazwa, fstream& plik, string akt, string aktmiasto);
	void readzp(string nazwa, fstream& plik);
};


class ScheduleReaderMetro : public ScheduleReader
{
	public:
	ScheduleReaderMetro(string sciezka, ScheduleHandler* handler) : ScheduleReader (sciezka, handler)
	{
	}
	string readForDelimeter(fstream& alpha, string& out);
	void run();
};


#endif
