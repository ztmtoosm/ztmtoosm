#include "/home/marcin/programowanie/cpp/ProjektyWłasne/TOR-EDIT/geom.hpp"
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
class OdcinekSchematowy
{
	public:
	pair<double, double> fromWspolrzedne;
	string fromId;
	pair<double, double> toWspolrzedne;
	string toId;
	string nameOsm;
	double dlugosc;
	static string readUstawienia(string nazwaZTorEdit, string key)
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
	void ladujDane(string nazwaZTorEdit)
	{
		char data[10000];
		cout<<"KSS "<<nazwaZTorEdit<<endl;
		cin.getline(data, 10000);
		string fromIdZTorEdit = rozdziel(data)[1];
		string toIdZTorEdit = rozdziel(data)[2];
		cout<<"GPK "<<fromIdZTorEdit<<endl;
		cin.getline(data, 10000);
		fromWspolrzedne=pair<double, double>(fromstring<double>(rozdziel(data)[1]), fromstring<double>(rozdziel(data)[2]));
		fromId=readUstawienia(fromIdZTorEdit, "id");
		cout<<"GPK "<<toIdZTorEdit<<endl;
		cin.getline(data, 10000);
		toWspolrzedne=pair<double, double>(fromstring<double>(rozdziel(data)[1]), fromstring<double>(rozdziel(data)[2]));
		toId=readUstawienia(toIdZTorEdit, "id");
		ladujLinie(readUstawienia(nazwaZTorEdit, "linie"));
		ladujPosrednie(readUstawienia(nazwaZTorEdit, "posrednie"));
		LINIA l1(fromWspolrzedne.first, fromWspolrzedne.second, toWspolrzedne.first, toWspolrzedne.second);
		dlugosc=l1.dlugosc();
	}
	vector <string> posrednieId;
	set <string> linie1;
	vector <string> linie;
	private:
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
	void ladujLinie(string tekst)
	{
		linie=rozdzielSredniki(tekst);
	}
	void ladujPosrednie(string tekst)
	{
		posrednieId=rozdzielSredniki(tekst);
	}
	
};
class GeneratorCzestotliwosci
{
	public:
	int liczbaKursow(OdcinekSchematowy* odcinek, string liniaId)
	{
		vector <string> puts=odcinek->posrednieId;
		puts.insert(puts.begin(), odcinek->fromId);
		puts.push_back(odcinek->toId);
		int wartosc=0;
		for(int i=0; i<puts.size()-1; i++)
		{
			int tempWartosc=max(kursy[liniaId][pair<string, string>(puts[i], puts[i+1])],kursy[liniaId][pair<string, string>(puts[i+1], puts[i])]);
			wartosc=max(tempWartosc, wartosc);
		}
		return wartosc;
	}
	int liczbaKursow(string from, string to, string liniaId)
	{
		return max(kursy[liniaId][pair<string, string>(from, to)],kursy[liniaId][pair<string, string>(to, from)]);
	}
	void transform(string liniaId, string fromId, string toId, vector <string> lista4)
	{
		plik<<"TRANFORM "<<fromId<<" "<<toId<<" "<<liniaId<<" : ";
		if(lista4.size()==0)
			return;
		int old1=kursy[liniaId][pair<string, string>(fromId, toId)];
		kursy[liniaId][pair<string, string>(fromId, toId)]=0;
		plik<<"GAL"<<endl;
		for(int i=0; i<lista4.size()-1; i++)
		{
			plik<<lista4[i]<<"-"<<lista4[i+1]<<" ";
			kursy[liniaId][pair<string, string>(lista4[i], lista4[i+1])]+=old1;
		}
		plik<<endl;
	}
	void dodajKurs(string liniaId, dni_kursowania dzien, int minuta, string fromIdPelne, string toIdPelne)
	{
		if(dzien==POWSZEDNI && minuta>60*60*6 && minuta<60*60*9)
		{
			string fromId=nowy2(fromIdPelne);
			string toId=nowy2(toIdPelne);
			kursy[liniaId][pair<string, string>(fromId, toId)]++;
		}
	}
	private:
	map<string, map <pair<string, string>, int> > kursy;
};
/*class PorzadkowanieKolejnosciLinii
{
	public:
	map<odcinek*, vector <string> > uporzadkowaneLinie;
	PorzadkowanieKolejnosciLinii(map<string, vector<OdcinekSchematowy*> > przystanki) : bazaPrzystankow(przystanki)
	{
		//TODO odwiedzPrzystanekIWskazNaKolejne(przystanki.begin()->first);
	}
	private:
	map <string, vector<OdcinekSchematowy*>  bazaPrzystankow;
	set <string> odwiedzonePrzystanki;
	set <odcinek*> odwiedzoneOdcinki;
	odcinek* znajdz_odcinek(string alfa, string beta)
	{
		if(bazaPrzystankow.find(alfa)==bazaPrzystankow.end())
			return NULL;
		if(bazaPrzystankow.find(beta)==bazaPrzystankow.end())
			return NULL;
		if(bazaPrzystankow[alfa].odcinki_start.find(beta)!=bazaPrzystankow[alfa].odcinki_start.end())
			return bazaPrzystankow[alfa].odcinki_start[beta];
		if(bazaPrzystankow[beta].odcinki_start.find(alfa)!=bazaPrzystankow[beta].odcinki_start.end())
			return bazaPrzystankow[beta].odcinki_start[alfa];
		return NULL;
	}
	void umiescPrzystanekWDoSortowania(vector <punkt_sort>& doSortowania, PUNKT baza, string nazwaPrzystanku)
	{
		punkt_sort nowy;
		nowy.baza=baza;
		nowy.pkt=wspolrzedneTorEdit(nazwaPrzystanku);
		nowy.name=(nazwaPrzystanku);
		doSortowania.push_back(nowy);
	}
	PUNKT wspolrzedneTorEdit(string nazwa)
	{
		zespol_przystankowy teraz=bazaPrzystankow[nazwa];
		return PUNKT((teraz.lon-21)*6000, (teraz.lat-52.5)*-10000);
	}
	vector <odcinek*> sortujOdcinki(string nazwaPrzystanku)
	{
		vector <punkt_sort> doSortowania;
		PUNKT baza=wspolrzedneTorEdit(nazwaPrzystanku);
		zespol_przystankowy teraz = bazaPrzystankow[nazwaPrzystanku];
		map<string, odcinek*>::iterator it1=teraz.odcinki_start.begin();
		while(it1!=teraz.odcinki_start.end())
		{
			umiescPrzystanekWDoSortowania(doSortowania, baza, it1->first);
			it1++;
		}
		it1=teraz.odcinki_stop.begin();
		while(it1!=teraz.odcinki_stop.end())
		{
			umiescPrzystanekWDoSortowania(doSortowania, baza, it1->first);
			it1++;
		}
		doSortowania=semi_otoczka(doSortowania);
		vector <odcinek*> wynik;
		for(int i=0; i<doSortowania.size(); i++)
		{
			wynik.push_back(znajdz_odcinek(nazwaPrzystanku, doSortowania[i].name));
		}
		return wynik;
	}
	bool czyOdcinekWychodzacy(odcinek* teraz, string nazwaPrzystanku)
	{
		if(teraz->start==nazwaPrzystanku)
			return true;
		return false;
	}
	bool czyReverse(odcinek* obslugiwany, odcinek* pomocniczy, string przystanekWezlowy)
	{
		bool czyWychO=czyOdcinekWychodzacy(obslugiwany, przystanekWezlowy);
		bool czyWychP=czyOdcinekWychodzacy(pomocniczy, przystanekWezlowy);
		//if(czyWychO==0 && czyWychP==0)
		//	return false;
		return true;
	}
	vector <string> wspolneLinieDlaOdwiedzonych(OdcinekPrzystankowy* obslugiwany, OdcinekPrzystnakowy* pomocniczy, string przystanekWezlowy)
	{
		vector <string> wynik;
		vector <string> doAnalizy = uporzadkowaneLinie[pomocniczy];
		if(czyReverse(obslugiwany, pomocniczy, przystanekWezlowy))
		{
			reverse(doAnalizy.begin(), doAnalizy.end());
		}
		for(int i=0; i<doAnalizy.size(); i++)
		{
			if(obslugiwany->linie.find(doAnalizy[i])!=obslugiwany->linie.end())
			{
				wynik.push_back(doAnalizy[i]);
			}
		}
		return wynik;
	}
	vector <string> wspolneLinieDlaNieodwiedzonych(OdcinekPrzystankowy* obslugiwany, OdcinekPrzystankowy* pomocniczy, string przystanekWezlowy)
	{
		plik<<pomocniczy<<endl;
		vector <string> wynik;
		vector <string> doAnalizy;
		set <string>::iterator it1=pomocniczy->linie.begin();
		while(it1!=pomocniczy->linie.end())
		{
			doAnalizy.push_back(*it1);
			it1++;
		}
		if(czyReverse(obslugiwany, pomocniczy, przystanekWezlowy))
		{
			reverse(doAnalizy.begin(), doAnalizy.end());
		}
		for(int i=0; i<doAnalizy.size(); i++)
		{
			if(obslugiwany->linie.find(doAnalizy[i])!=obslugiwany->linie.end())
			{
				wynik.push_back(doAnalizy[i]);
			}
		}
		return wynik;
	}
	void odwiedzPrzystanekIWskazNaKolejne (string nazwa)
	{
		vector <string> kolejnePrzystankiDoOdwiedzenia;
		odwiedzonePrzystanki.insert(nazwa);
		vector <OdcinekPrzystankowy*> posortowaneOdcinki = sortujOdcinki(nazwa);
		int s1 = posortowaneOdcinki.size();
		for(int i=0; i<s1; i++)
		{
			if(odwiedzoneOdcinki.find(posortowaneOdcinki[i])==odwiedzoneOdcinki.end())
			{
				kolejnePrzystankiDoOdwiedzenia.push_back(posortowaneOdcinki[i]->fromId);
				kolejnePrzystankiDoOdwiedzenia.push_back(posortowaneOdcinki[i]->toId);
				vector <string> posortowaneLinie;
				odwiedzoneOdcinki.insert(posortowaneOdcinki[i]);
				for(int j=i+1; (j%s1)!=i; j++)
				{
					j=j%s1;
					if(odwiedzoneOdcinki.find(posortowaneOdcinki[j])==odwiedzoneOdcinki.end())
					{
						vector <string> wynikPart = wspolneLinieDlaOdwiedzonych(posortowaneOdcinki[i], posortowaneOdcinki[j], nazwa);
						posortowaneLinie.insert(posortowaneLinie.end(), wynikPart.begin(), wynikPart.end());
					}
					else
					{
						vector <string> wynikPart = wspolneLinieDlaNieodwiedzonych(posortowaneOdcinki[i], posortowaneOdcinki[j], nazwa);
						posortowaneLinie.insert(posortowaneLinie.end(), wynikPart.begin(), wynikPart.end());
					}
				}
				set <string> noweLinie;
				vector <string> posortowaneLinie2;
				for(int j=0; j<posortowaneLinie.size(); j++)
				{
					if(noweLinie.find(posortowaneLinie[j])==noweLinie.end())
					{
						noweLinie.insert(posortowaneLinie[j]);
						posortowaneLinie2.push_back(posortowaneLinie[j]);
					}
				}
				posortowaneLinie=posortowaneLinie2;
				set <string> stareLinie=posortowaneOdcinki[i]->linie;
				set <string>::iterator it1=stareLinie.begin();
				while(it1!=stareLinie.end())
				{
					if(noweLinie.find(*it1)==noweLinie.end())
					{
						posortowaneLinie.push_back(*it1);
						noweLinie.insert(*it1);
					}
					it1++;
				}
				if(czyOdcinekWychodzacy(posortowaneOdcinki[i], nazwa))
					reverse(posortowaneLinie.begin(), posortowaneLinie.end());
				uporzadkowaneLinie[posortowaneOdcinki[i]]=posortowaneLinie;
			}
		}
		for(int i=0; i<kolejnePrzystankiDoOdwiedzenia.size(); i++)
		{
			if(odwiedzonePrzystanki.find(kolejnePrzystankiDoOdwiedzenia[i])==odwiedzonePrzystanki.end())
				odwiedzPrzystanekIWskazNaKolejne(kolejnePrzystankiDoOdwiedzenia[i]);
		}
	}
};*/


struct SciezkaWSchemacie
{
	string nazwa;
	double dlugosc;
	OdcinekSchematowy* odcinek;
	SciezkaWSchemacie(OdcinekSchematowy* foo) : odcinek(foo)
	{
	}
};
void set_color(string nazwa, vector <int> col)
{
	char data[500];
	cout<<"SPP "<<nazwa<<" X11_color_r "<<col[0]<<endl;
	cin.getline(data, 500);
	cout<<"SPP "<<nazwa<<" X11_color_g "<<col[1]<<endl;
	cin.getline(data, 500);
	cout<<"SPP "<<nazwa<<" X11_color_b "<<col[2]<<endl;
	cin.getline(data, 500);
}
class ztmread_netz : private ztmread
{
	GeneratorCzestotliwosci generatorCzestotliwosci;
	vector <OdcinekSchematowy*> odcinkiSchematowe;
	map <string, vector <int> > kolory;
	set<pair<string, string> > dodane_kr;
	map<pair<string, string>, double> kierunki;
	map<string, vector<SciezkaWSchemacie> > najkrotszeSciezkiWSchemacieBaza;
	void ladujOdcinkiSchematowe()
	{
		cout<<"KE1 "<<endl;
		char data[100000];
		cin.getline(data,100000);
		vector <string> nazwy=rozdziel(data);
		for(int i=1; i<nazwy.size(); i++)
		{
			OdcinekSchematowy* foo=new OdcinekSchematowy;
			foo->ladujDane(nazwy[i]);
			foo->nameOsm=nazwy[i];
			cout<<"SPP "<<nazwy[i]<<" X11_grubosc 0"<<endl;
			cin.getline(data,100000);
			odcinkiSchematowe.push_back(foo);
		}
		cout<<"PE1 "<<endl;
		cin.getline(data,100000);
		nazwy=rozdziel(data);
		for(int i=1; i<nazwy.size(); i++)
		{
			string u1=OdcinekSchematowy::readUstawienia(nazwy[i], "name");
			if(rand()%100==0 && u1.length()>0)
			{
				cout<<"SPP "<<nazwy[i]<<" X11_text "<<u1<<endl;
				cin.getline(data,100000);
			}
			cout<<"SPP "<<nazwy[i]<<" X11_grubosc 0"<<endl;
			cin.getline(data,100000);
		}
	}
	void nowy_kurs(kurs nowy)
	{
		for(int i=0; i<nowy.postoje.size()-1; i++)
		{
			generatorCzestotliwosci.dodajKurs(nowy.linia, nowy.dni, nowy.postoje[i].time, nowy.postoje[i].stop_id, nowy.postoje[i+1].stop_id);
		}
	}
	void dodaj_linke(string nazwa, vector <string> calat, map<string, string>& dane)
	{
		if(kolory.find(nazwa)==kolory.end())
		{
			vector <int> newkolors;
			for(int i=0; i<3; i++)
			{
				newkolors.push_back(rand()%255);
			}
			kolory[nazwa]=newkolors;
		}
		for(int i=0; i<calat.size()-1; i++)
		{
			string teraz, nast;
			teraz="Y"+nazwa+"Y"+calat[i];
			if(dane.find(teraz)==dane.end())
				teraz+="Y"+calat[i+1];
			nast="Y"+nazwa+"Y"+calat[i+1];
			if(dane.find(nast)==dane.end())
				nast+="Y"+calat[i];
			plik<<"tr: "<<teraz<<" "<<nast<<" "<<dane.size()<<endl;
			if(teraz!=nast)
			if(dane.find(teraz)!=dane.end())
			{
				if(dane.find(nast)!=dane.end())
				if(dodane_kr.find(pair<string, string>(dane[teraz], dane[nast]))==dodane_kr.end())
				if(dodane_kr.find(pair<string, string>(dane[nast], dane[teraz]))==dodane_kr.end())
				{
					cout<<"SLN "<<dane[teraz]<<" "<<dane[nast]<<endl;
					dodane_kr.insert(pair<string, string>(dane[nast], dane[teraz]));
					char data[133];
					cin.getline(data, 133);
					string nazwaTor=rozdziel(data)[1];
					cout<<"SPP "<<nazwaTor<<" X11_grubosc -"<<min(max(1, generatorCzestotliwosci.liczbaKursow(calat[i], calat[i+1], nazwa)/5), 8)<<endl;
					cin.getline(data, 133);
					set_color(nazwaTor, kolory[nazwa]);
				}
			}
		}
		for(int i=1; i<calat.size()-1; i++)
		{
			string teraz, nast;
			teraz="Y"+nazwa+"Y"+calat[i]+"Y"+calat[i-1];
			nast="Y"+nazwa+"Y"+calat[i]+"Y"+calat[i+1];
			if(teraz!=nast)
			if(dane.find(teraz)!=dane.end())
				if(dane.find(nast)!=dane.end())
				if(dodane_kr.find(pair<string, string>(dane[teraz], dane[nast]))==dodane_kr.end())
				if(dodane_kr.find(pair<string, string>(dane[nast], dane[teraz]))==dodane_kr.end())
				{
					double k1=kierunki[pair<string, string>(calat[i-1], calat[i])];
					double k2=kierunki[pair<string, string>(calat[i], calat[i+1])];
					char data[133];
					cout<<"GPK "<<dane[teraz]<<endl;
					cin.getline(data, 133);
					PUNKT p1(fromstring<double>(rozdziel(data)[1]), fromstring<double>(rozdziel(data)[2]));
					cout<<"GPK "<<dane[nast]<<endl;
					cin.getline(data, 133);
					PUNKT p2(fromstring<double>(rozdziel(data)[1]), fromstring<double>(rozdziel(data)[2]));
					LINIOLUK ll(p1, p2, k1, k2);
					LUK llu=ll.l2;
					int powazneObliczenia1=min(max(1, generatorCzestotliwosci.liczbaKursow(calat[i], calat[i+1], nazwa)/5), 8);
					int powazneObliczenia2=min(max(1, generatorCzestotliwosci.liczbaKursow(calat[i], calat[i-1], nazwa)/5), 8);
					if(abs(llu.kierunekwodl(llu.dlugosc())-k2)<0.1 && abs(llu.kierunekwodl(0)-k1)<0.1)
					{
						cout<<"SLL "<<dane[teraz]<<" "<<dane[nast]<<" "<<k1<<" "<<k2<<endl;
						cin.getline(data, 133);
						string nazwaTor=rozdziel(data)[1];
						cout<<"SPP "<<nazwaTor<<" X11_grubosc -"<<min(powazneObliczenia1, powazneObliczenia2)<<endl;
						cin.getline(data, 133);
						set_color(nazwaTor, kolory[nazwa]);
					}
					else
					{
						LINIA l3(p1, p2);
						LINIA l11(p1, k1, l3.dlugosc()/4);
						LINIA l12(p2, k2, l3.dlugosc()/4);
						LINIA l13(l11.stop(), l12.start());
						PUNKT p4=l3.punktwodl(l3.dlugosc()/2);
						double k3=l13.kierunek();
						cout<<"SPK "<<p4.x<<" "<<p4.y<<endl;
						cin.getline(data, 133);
						string roftl=rozdziel(data)[1];
						cout<<"SLL "<<dane[teraz]<<" "<<roftl<<" "<<k1<<" "<<k3<<endl;
						cin.getline(data, 133);
						string nazwaTor=rozdziel(data)[1];
						cout<<"SPP "<<nazwaTor<<" X11_grubosc -"<<min(powazneObliczenia1, powazneObliczenia2)<<endl;
						cin.getline(data, 133);
						set_color(nazwaTor, kolory[nazwa]);
						cout<<"SLL "<<roftl<<" "<<dane[nast]<<" "<<k3<<" "<<k2<<endl;
						cin.getline(data, 133);
						nazwaTor=rozdziel(data)[1];
						cout<<"SPP "<<nazwaTor<<" X11_grubosc -"<<min(powazneObliczenia1, powazneObliczenia2)<<endl;
						cin.getline(data, 133);
						set_color(nazwaTor, kolory[nazwa]);
					}
					dodane_kr.insert(pair<string, string>(dane[nast], dane[teraz]));
				}
		}
	}
	map <string, double> szerokosci;
	void wspol(OdcinekSchematowy* odcinek, map<string, string>& wynik, pair <double, double> sta, pair<double, double> sto, vector <string> odc, vector <string> lin)
	{
		PUNKT p1(sta.first, sta.second);
		PUNKT p2(sto.first, sto.second);
		LINIA l1(p1, p2);
		double odl_pocz_start=szerokosci[odc[0]];
		double odl_pocz_stop=szerokosci[odc[odc.size()-1]];
		vector <double> linie_czestotliwosci;
		double linie_czestotliwosci_sum=0;
		for(int i=0; i<lin.size(); i++)
		{
			int t1=(min(max(1, generatorCzestotliwosci.liczbaKursow(odcinek, lin[i])/5), 8));
			linie_czestotliwosci.push_back(t1);
			linie_czestotliwosci_sum+=t1;
			if(i>0)
				linie_czestotliwosci_sum+=0.5;
		}
		double czest_wyk=linie_czestotliwosci_sum;
		czest_wyk/=-2;
		for(int i=0; i<lin.size(); i++)
		{
			czest_wyk+=linie_czestotliwosci[i]/2.0;
			LINIA l2=l1.rownolegla(czest_wyk);
			double czest_wyk_tmp=czest_wyk;
			czest_wyk+=linie_czestotliwosci[i]/2.0;
			czest_wyk+=0.5;
			LINIA l3=LINIA(l2.odlodpocz(min(odl_pocz_start, l2.dlugosc()/2)-1), l2.odlodpocz(max(l2.dlugosc()-odl_pocz_stop, l2.dlugosc()/2+1)));
			plik<<l3.x1<<" "<<l3.y1<<" "<<l3.x2<<" "<<l3.y2<<endl;
			for(int j=0; j<odc.size(); j++)
			{
				PUNKT teraz=l3.odlodpocz(l3.dlugosc()/(odc.size()-1)*j);
				string nazwa="Y"+lin[i]+"Y"+odc[j];
				if(j==0)
				{
					nazwa+="Y"+odc[1];
				}
				if(j==odc.size()-1)
				{
					nazwa+="Y"+odc[odc.size()-2];
				}
				else
				{
					kierunki[pair<string, string>(odc[j], odc[j+1])]=l2.kierunek();
					kierunki[pair<string, string>(odc[j+1], odc[j])]=l2.reverse().kierunek();
				}
				cout<<"SPK "<<teraz.x<<" "<<teraz.y<<endl;
				char data[1000];
				cin.getline(data, 1000);
				wynik[nazwa]=rozdziel(data)[1];
				cout<<"SPP "<<wynik[nazwa]<<" nazwa "<<nazwa<<endl;
				cin.getline(data, 1000);
				cout<<"SPP "<<wynik[nazwa]<<" wyktmp "<<czest_wyk_tmp<<endl;
				cin.getline(data, 1000);
	
			}
		}
	}


	vector <pair<string, OdcinekSchematowy*> > bfs (string from, string to, map<string, vector <SciezkaWSchemacie> >& nodes)
	{
		plik<<"NOD SIZ"<<nodes.size()<<endl;
		vector <pair<string, OdcinekSchematowy*> > wynik;
		if(nodes.find(to)==nodes.end())
			return wynik;
		if(nodes.find(from)==nodes.end())
			return wynik;
		plik<<"NOD SIZ2"<<nodes.size()<<endl;
		set <pair<double, string> > kolejka1;
		map <string, double> kolejka2;
		set <string> odwiedzone;
		kolejka2[from]=0;
		kolejka1.insert(pair<double, string>(0, from));
		map<string, string> ojciec;
		map<string, OdcinekSchematowy*> ojciec2;
		bool ok=1;
		while(!kolejka1.empty() && ok)
		{
			plik<<"NOD X0 "<<endl;
			string akt_str=kolejka1.begin()->second;
			if(akt_str==to)
				ok=0;
			odwiedzone.insert(akt_str);
			plik<<"NOD X1 "<<endl;
			double akt_wart=kolejka1.begin()->first;
			plik<<"NOD X2 "<<akt_wart<<" "<<akt_str<<endl;
			set<pair<double, string> >::iterator it1=kolejka1.find(pair<double,string>(akt_wart, akt_str));
			plik<<"NOD X2.05 "<<endl;
			if(it1!=kolejka1.end())
				kolejka1.erase(it1);
			else
			{
				vector <pair<string, OdcinekSchematowy*> > pusty;
				plik<<"NOD X2.1 "<<endl;
				return pusty;
			}
			plik<<"NOD X2.5 "<<endl;
			if(kolejka2.find(akt_str)!=kolejka2.end())
				kolejka2.erase(kolejka2.find(akt_str));
			plik<<"NOD X3 "<<endl;
			vector <SciezkaWSchemacie> nod=nodes[akt_str];
			plik<<"NOD X "<<nod.size()<<endl;
			plik<<"AKT STR "<<akt_str<<endl;
			for(int i=0; i<nod.size() && ok; i++)
			{
				plik<<nod[i].odcinek<<" @ "<<kolejka1.size()<<endl;
				if(odwiedzone.find(nod[i].nazwa)==odwiedzone.end())
				{
					plik<<"NOD alfa1 "<<from<<" "<<to<<endl;
					double way_wart=nod[i].dlugosc;
					plik<<"NOD alfa1.1"<<endl;
					if(kolejka2.find(nod[i].nazwa)==kolejka2.end())
					{
						plik<<"NOD alfa2"<<endl;
						kolejka2[nod[i].nazwa]=akt_wart+way_wart;
						ojciec[nod[i].nazwa]=akt_str;
						plik<<"NOD alfa2.4"<<endl;
						ojciec2[nod[i].nazwa]=nod[i].odcinek;
						plik<<"NOD alfa2.5"<<endl;
						kolejka1.insert(pair<double, string>(akt_wart+way_wart, nod[i].nazwa));
						plik<<"NOD alfa2.6"<<endl;
					}
					else
					{
						plik<<"NOD alfa3-1"<<endl;
						if(akt_wart+way_wart<kolejka2[nod[i].nazwa])
						{
							plik<<"NOD alfa3"<<endl;
							kolejka1.erase(kolejka1.find(pair<double, string>(kolejka2[nod[i].nazwa], nod[i].nazwa)));
							kolejka2[nod[i].nazwa]=akt_wart+way_wart;
							
							plik<<"NOD alfa3.6"<<endl;
							ojciec[nod[i].nazwa]=akt_str;
							ojciec2[nod[i].nazwa]=nod[i].odcinek;
							kolejka1.insert(pair<double, string>(akt_wart+way_wart, nod[i].nazwa));
						}
					}
				}
			}
		}
		plik<<"NOD OUT "<<endl;
		string akt_str=to;
		wynik.push_back(pair<string, OdcinekSchematowy*>(akt_str, NULL));
		while(ojciec.find(akt_str)!=ojciec.end())
		{
			OdcinekSchematowy* akt_odc=ojciec2[akt_str];
			akt_str=ojciec[akt_str];
			wynik.push_back(pair<string, OdcinekSchematowy*>(akt_str, akt_odc));
		}
		reverse(wynik.begin(), wynik.end());
		plik<<"NOD OUT-real "<<endl;
		return wynik;
	}

	vector <string> cala_trasa(vector <string> lista1)
	{
		vector <string> lista2;
		for(int i=0; i<lista1.size(); i++)
		{
			lista2.push_back(nowy2(lista1[i]));
		}
		lista2=pomin_podobne(lista2);
		vector <string> lista3;
		for(int i=0; i<lista2.size()-1; i++)
		{
			vector <pair<string, OdcinekSchematowy*> > lista4=bfs(lista2[i], lista2[i+1], najkrotszeSciezkiWSchemacieBaza);
			plik<<lista4.size()<<endl;
			for(int j=0; j<lista4.size(); j++)
			{
				lista3.push_back(lista4[j].first);
			}
		}
		lista3=pomin_podobne(lista3);
		return lista3;
	}
	void popraw_czestotliwosci(string naz, vector <string> lista1)
	{
		vector <string> lista2;
		for(int i=0; i<lista1.size(); i++)
		{
			lista2.push_back(nowy2(lista1[i]));
		}
		lista2=pomin_podobne(lista2);
		for(int i=0; i<lista2.size()-1; i++)
		{
			vector <pair<string, OdcinekSchematowy*> > lista4=bfs(lista2[i], lista2[i+1], najkrotszeSciezkiWSchemacieBaza);
			vector <string> lista5;
			for(int j=0; j<lista4.size(); j++)
			{
				lista5.push_back(lista4[j].first);
			}
			plik<<"TRAN rBEGIN"<<endl;
			lista5=pomin_podobne(lista5);
			plik<<"TRAN BEGIN "<<lista2[i]<<" "<<lista2[i+1]<<" "<<lista5.size()<<endl;
			generatorCzestotliwosci.transform(naz, lista2[i], lista2[i+1], lista5);
		}
	}
	void dodajLinieDoSchematu(string nazwa, vector <string> lista1)
	{
		set <string> wynik;
		vector <string> lista2;
		for(int i=0; i<lista1.size(); i++)
		{
			lista2.push_back(nowy2(lista1[i]));
		}
		lista2=pomin_podobne(lista2);
		for(int i=0; i<lista2.size()-1; i++)
		{
			vector <pair<string, OdcinekSchematowy*> > lista4=bfs(lista2[i], lista2[i+1], najkrotszeSciezkiWSchemacieBaza);
			plik<<lista4.size()<<" LIS4"<<endl;
			for(int j=0; j<lista4.size(); j++)
			{
				if(lista4[j].second!=NULL)
					(lista4[j].second)->linie1.insert(nazwa);
			}
		}
	}
	map<string, vector <vector <string> > > trasyt;
	void nowa_linia(string nazwa, vector <vector <string> > trasy)
	{
		if(nazwa[0]=='N')
			return;
		trasyt[nazwa]=trasy;
	}
	public:
	ztmread_netz (string sciezka) : ztmread(sciezka)
	{
		run();
		ladujOdcinkiSchematowe();
		
		for(int j=0; j<odcinkiSchematowe.size(); j++)
		{
			vector <string> puts;
			puts=odcinkiSchematowe[j]->posrednieId;
			puts.insert(puts.begin(), odcinkiSchematowe[j]->fromId);
			puts.push_back( odcinkiSchematowe[j]->toId);
			for(int i=0; i<puts.size()-1; i++)
			{
				SciezkaWSchemacie nowa1(odcinkiSchematowe[j]);
				SciezkaWSchemacie nowa2(odcinkiSchematowe[j]);
				nowa1.nazwa=puts[i+1];
				nowa1.dlugosc=odcinkiSchematowe[j]->dlugosc/(puts.size()-1);
				nowa2.nazwa=puts[i];
				nowa2.dlugosc=odcinkiSchematowe[j]->dlugosc/(puts.size()-1);
				najkrotszeSciezkiWSchemacieBaza[puts[i]].push_back(nowa1);
				najkrotszeSciezkiWSchemacieBaza[puts[i+1]].push_back(nowa2);
			}
		}
		map<string, vector <vector <string> > >::iterator it3=trasyt.begin();
		while(it3!=trasyt.end())
		{
			vector <vector <string> > trasyt2=it3->second;
			for(int i=0; i<trasyt2.size(); i++)
			{
				popraw_czestotliwosci(it3->first, trasyt2[i]);
			}
			it3++;
		}
		it3=trasyt.begin();
		while(it3!=trasyt.end())
		{
			vector <vector <string> > trasyt2=it3->second;
			for(int i=0; i<trasyt2.size(); i++)
			{
				dodajLinieDoSchematu(it3->first, trasyt2[i]);
			}
			it3++;
		}
		for(int j=0; j<odcinkiSchematowe.size(); j++)
		{
			set<string>::iterator it1=odcinkiSchematowe[j]->linie1.begin();
			plik<<odcinkiSchematowe[j]->linie1.size()<<"lin"<<endl;
			while(it1!=odcinkiSchematowe[j]->linie1.end())
			{
				odcinkiSchematowe[j]->linie.push_back(*it1);
				it1++;
			}
		}
		map <string, string> id_toredit;
		plik<<odcinkiSchematowe.size()<<" SCHEMATOWE "<<endl;
		for(int j=0; j<odcinkiSchematowe.size(); j++)
		{
			string from=odcinkiSchematowe[j]->fromId;
			string to=odcinkiSchematowe[j]->toId;
			szerokosci[from]=max(szerokosci[from], (odcinkiSchematowe[j]->linie.size()+1)*ODL_ROWN);
			szerokosci[to]=max(szerokosci[to], (odcinkiSchematowe[j]->linie.size()+1)*ODL_ROWN);
		}
		
		for(int j=0; j<odcinkiSchematowe.size(); j++)
		{
			vector <string> lint=odcinkiSchematowe[j]->linie;
			vector <string> puts;
			puts=odcinkiSchematowe[j]->posrednieId;
			puts.insert(puts.begin(), odcinkiSchematowe[j]->fromId);
			puts.push_back(odcinkiSchematowe[j]->toId);
			wspol(odcinkiSchematowe[j], id_toredit, odcinkiSchematowe[j]->fromWspolrzedne, odcinkiSchematowe[j]->toWspolrzedne, puts, lint); 
			if(odcinkiSchematowe[j]->posrednieId.size()>0 && odcinkiSchematowe[j]->dlugosc>20 && ((odcinkiSchematowe[j]->dlugosc-20)/(odcinkiSchematowe[j]->linie.size()))>6)
			{
				cout<<"SPP "<<odcinkiSchematowe[j]->nameOsm<<" X11_text "<<wy_posr(odcinkiSchematowe[j]->linie)<<endl;
				char data[500];
				cin.getline(data, 500);
			}
				cout<<"SPP "<<odcinkiSchematowe[j]->nameOsm<<" dlg "<<(odcinkiSchematowe[j]->dlugosc)<<endl;
				char data[500];
				cin.getline(data, 500);
		
		}
		it3=trasyt.begin();
		while(it3!=trasyt.end())
		{
			vector <vector <string> > trasyt2=it3->second;
			for(int i=0; i<trasyt2.size(); i++)
			{
				dodaj_linke(it3->first, cala_trasa(trasyt2[i]), id_toredit);
			}
			it3++;
		}
	}
};

int main(int argc, char** argv)
{
	srand(time(NULL));
	string arg1=argv[1];
	plik.open("huj.txt", ios::out | ios::trunc);
	ztmread_netz zrn(arg1);
	plik.close();
}
