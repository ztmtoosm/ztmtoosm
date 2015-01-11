#include "/home/marcin/programowanie/cpp/ProjektyWłasne/TOR-EDIT/geom.hpp"
#include "readztm.hpp"
#include <map>
#include <fstream>
#include <algorithm>
using namespace std;
fstream plik;
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
struct zespol_przystankowy;
struct odcinek
{
	string start, stop;
	vector <string> przystanki_posrednie;
	set <string> linie;

};
struct zespol_przystankowy
{
	double lat;
	double lon;
	string name;
	string id;
	map <string, odcinek*> odcinki_start;
	map <string, odcinek*> odcinki_stop;
};
class PorzadkowanieKolejnosciLinii
{
	public:
	map<odcinek*, vector <string> > uporzadkowaneLinie;
	PorzadkowanieKolejnosciLinii(map<string, zespol_przystankowy> przystanki) : bazaPrzystankow(przystanki)
	{
		odwiedzPrzystanekIWskazNaKolejne(przystanki.begin()->first);
	}
	private:
	map <string, zespol_przystankowy> bazaPrzystankow;
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
	vector <string> wspolneLinieDlaOdwiedzonych(odcinek* obslugiwany, odcinek* pomocniczy, string przystanekWezlowy)
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
	vector <string> wspolneLinieDlaNieodwiedzonych(odcinek* obslugiwany, odcinek* pomocniczy, string przystanekWezlowy)
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
		vector <odcinek*> posortowaneOdcinki = sortujOdcinki(nazwa);
		int s1 = posortowaneOdcinki.size();
		for(int i=0; i<s1; i++)
		{
			plik<<nazwa<<" xD "<<posortowaneOdcinki[i]->start<<" "<<posortowaneOdcinki[i]->stop;
			if(odwiedzoneOdcinki.find(posortowaneOdcinki[i])==odwiedzoneOdcinki.end())
			{
				kolejnePrzystankiDoOdwiedzenia.push_back(posortowaneOdcinki[i]->start);
				kolejnePrzystankiDoOdwiedzenia.push_back(posortowaneOdcinki[i]->stop);
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
};
class ztmread_netz : private ztmread
{
	map <string, zespol_przystankowy> przystanki;
	string nowy2(string id)
	{
		string id2;
		for(int i=0; i<id.length()-2; i++)
		{
			id2+=id[i];
		}
		return id2;
	}
	odcinek* znajdz_odcinek(string alfa, string beta)
	{
		if(przystanki.find(alfa)==przystanki.end())
			return NULL;
		if(przystanki.find(beta)==przystanki.end())
			return NULL;
		if(przystanki[alfa].odcinki_start.find(beta)!=przystanki[alfa].odcinki_start.end())
			return przystanki[alfa].odcinki_start[beta];
		if(przystanki[beta].odcinki_start.find(alfa)!=przystanki[beta].odcinki_start.end())
			return przystanki[beta].odcinki_start[alfa];
		return NULL;
	}
	void usun_odcinek(odcinek* teraz)
	{
		if(teraz==NULL)
			return;
		przystanki[teraz->start].odcinki_start.erase(teraz->stop);
		przystanki[teraz->stop].odcinki_stop.erase(teraz->start);
		delete teraz;
	}
	void dodaj_odcinek(odcinek* foo)
	{
		przystanki[foo->start].odcinki_start[foo->stop]=(foo);
		przystanki[foo->stop].odcinki_stop[foo->start]=(foo);
	}
	void add_posrednie(odcinek* nowy, odcinek* mag1, odcinek* mag2, int typ, string srodek)
	{
		if(typ==1)
		{
			nowy->przystanki_posrednie.insert(nowy->przystanki_posrednie.end(), mag1->przystanki_posrednie.begin(), mag1->przystanki_posrednie.end());
			nowy->przystanki_posrednie.push_back(srodek);
			nowy->przystanki_posrednie.insert(nowy->przystanki_posrednie.end(), mag2->przystanki_posrednie.begin(), mag2->przystanki_posrednie.end());
		}
		if(typ==2)
		{
			vector <string> ppr=mag1->przystanki_posrednie;
			reverse(ppr.begin(), ppr.end());
			nowy->przystanki_posrednie.insert(nowy->przystanki_posrednie.end(), ppr.begin(), ppr.end());
			nowy->przystanki_posrednie.push_back(srodek);
			nowy->przystanki_posrednie.insert(nowy->przystanki_posrednie.end(), mag2->przystanki_posrednie.begin(), mag2->przystanki_posrednie.end());
		}
		if(typ==3)
		{
			vector <string> ppr=mag2->przystanki_posrednie;
			reverse(ppr.begin(), ppr.end());
			nowy->przystanki_posrednie.insert(nowy->przystanki_posrednie.end(), mag1->przystanki_posrednie.begin(), mag1->przystanki_posrednie.end());
			nowy->przystanki_posrednie.push_back(srodek);
			nowy->przystanki_posrednie.insert(nowy->przystanki_posrednie.end(), ppr.begin(), ppr.end());
			
		}
	}
	bool reduct_zespol(string str)
	{
		bool wynik=0;
		zespol_przystankowy& teraz=przystanki[str];
		plik<<teraz.odcinki_start.size()<<" "<<teraz.odcinki_stop.size()<<endl;
		string start, stop;
		int ok=0;
		if(teraz.odcinki_start.size()==1 && teraz.odcinki_stop.size()==1)
		{
			start=teraz.odcinki_stop.begin()->first;
			stop=teraz.odcinki_start.begin()->first;
			ok=1;	
		}
		if(teraz.odcinki_start.size()==2 && teraz.odcinki_stop.size()==0)
		{
			map<string, odcinek*>::iterator it1=teraz.odcinki_start.begin();
			start=it1->first;
			it1++;
			stop=it1->first;
			ok=2;
		}
		if(teraz.odcinki_start.size()==0 && teraz.odcinki_stop.size()==2)
		{
			map<string, odcinek*>::iterator it1=teraz.odcinki_stop.begin();
			start=it1->first;
			it1++;
			stop=it1->first;
			ok=3;
		}
		if(start==stop)
			return 0;
		if(ok==0)
			return 0;
		if(znajdz_odcinek(start, stop)==NULL)
		{
			plik<<"t1"<<endl;
			odcinek* mag1=znajdz_odcinek(start, str);
			odcinek* mag2=znajdz_odcinek(stop, str);
			if(mag1!=NULL && mag2!=NULL && magluj_easy(mag1->linie)==magluj_easy(mag2->linie))
			{
				set<string> nacht=mag1->linie;
				odcinek* nowy=new odcinek;
				nowy->linie=nacht;
				nowy->start=start;
				nowy->stop=stop;
				add_posrednie(nowy, mag1, mag2, ok, str);	
				usun_odcinek(mag1);
				usun_odcinek(mag2);
				dodaj_odcinek(nowy);
				wynik=1;
			}
		}
		else
		{
			plik<<"t2"<<endl;
			odcinek* stary=znajdz_odcinek(start, stop);
			odcinek* mag1=znajdz_odcinek(start, str);
			odcinek* mag2=znajdz_odcinek(stop, str);
			if(stary->przystanki_posrednie.size()==0 && mag1!=NULL && mag2!=NULL && magluj_easy(mag1->linie)==magluj_easy(mag2->linie))
			{
				set<string> nacht=mag1->linie;
				stary->linie.insert(nacht.begin(), nacht.end());
				add_posrednie(stary, mag1, mag2, ok, str);
				if(stary->start!=start)
					reverse(stary->przystanki_posrednie.begin(), stary->przystanki_posrednie.end());
				usun_odcinek(mag1);
				usun_odcinek(mag2);
				wynik=1;
			}
		}
		return wynik;
	}
	void nowy_przystanek(przystanek nowy)
	{
		zespol_przystankowy foo;
		string id2=nowy2(nowy.id);
		foo.id=id2;
		foo.name=nowy.name;
		foo.lat=nowy.lat;
		foo.lon=nowy.lon;
		przystanki[id2]=foo;
	}
	void nowa_linia(string nazwa, vector <vector <string> > trasy)
	{
		if(nazwa[0]=='N')
			return;
		for(int i=0; i<trasy.size(); i++)
		{
			for(int j=0; j<trasy[i].size()-1; j++)
			{
				string id1=nowy2(trasy[i][j]);
				string id2=nowy2(trasy[i][j+1]);
			//	string id1=trasy[i][j];
			//	string id2=trasy[i][j+1];
			
				if(id1!=id2)
				{
					odcinek* foo;
					int type=0;
					if(przystanki[id1].odcinki_start.find(id2)!=przystanki[id1].odcinki_start.end())
					{
						foo=przystanki[id1].odcinki_start[id2];
						type=1;
					}
					else
					{
						if(przystanki[id2].odcinki_start.find(id1)!=przystanki[id2].odcinki_start.end())
						{
							foo=przystanki[id2].odcinki_start[id1];
							type=2;
						}
					}
					if(type==0)
					{
						foo=new odcinek;
						foo->start=id1;
						foo->stop=id2;
						dodaj_odcinek(foo);
					}
					foo->linie.insert(nazwa);
				}
			}
		}
	}
	public:
	ztmread_netz (string sciezka) : ztmread(sciezka)
	{
		run();
		
		map <string, zespol_przystankowy>::iterator it1=przystanki.begin();
		/*bool dalej=1;
		while(dalej)
		{
			dalej=0;
			it1=przystanki.begin();
			while(it1!=przystanki.end())
			{
				bool dalej_tmp2=reduct_zespol(it1->first);
				dalej|=dalej_tmp2;
				it1++;
			}
		}*/
		map <string, string> id_toredit;
		it1=przystanki.begin();
		while(it1!=przystanki.end())
		{
			zespol_przystankowy teraz=(it1->second);
			if((teraz.odcinki_start.size()>0 || teraz.odcinki_stop.size()>0) && teraz.lon>20 && teraz.lat>50)
			{
				cout<<"SPK "<<(teraz.lon-21)*6000<<" "<<(teraz.lat-52.5)*-10000<<endl;
				char data[10000];
				cin.getline(data,10000);
				vector <string> data1=rozdziel(data);
				id_toredit[it1->first]=data1[1];
				cout<<"SPP "<<data1[1]<<" name "<<teraz.name<<endl;
				cin.getline(data,10000);
				cout<<"SPP "<<data1[1]<<" id "<<teraz.id<<endl;
				cin.getline(data,10000);
			}
			it1++;
		}
		PorzadkowanieKolejnosciLinii porzadkowanie(przystanki);
		it1=przystanki.begin();
		while(it1!=przystanki.end())
		{
			map<string, odcinek*>::iterator it2=(it1->second).odcinki_start.begin();
			while(it2!=it1->second.odcinki_start.end())
			{
				if(id_toredit.find(it1->first)!=id_toredit.end())
					if(id_toredit.find(it2->first)!=id_toredit.end())
					{
						cout<<"SLN "<<id_toredit[it1->first]<<" "<<id_toredit[it2->first]<<endl;
						char data[10000];
						cin.getline(data,10000);
						vector <string> data1=rozdziel(data);
						string mag=magluj_easy((it2->second)->linie);
			//			cout<<"SPP "<<data1[1]<<" linie "<<wy_posr(porzadkowanie.uporzadkowaneLinie[it2->second])<<endl;
			//			cin.getline(data,10000);
						cout<<"SPP "<<data1[1]<<" posrednie "<<wy_posr(it2->second->przystanki_posrednie)<<endl;
						cin.getline(data,10000);
					
					}
				it2++;
			}
			it1++;
		}
	}
};

int main(int argc, char** argv)
{
	string arg1=argv[1];
	plik.open("huj.txt", ios::out | ios::trunc);
	ztmread_netz zrn(arg1);
	plik.close();
}
