#include "readztm.hpp"
#include "md5.h"
#include "HafasBaza.hpp"
using namespace std;

struct foo : public ScheduleHandler
{
	fstream stop_times;
	fstream trips;
	fstream shapes;
	sql_routes rou;
	set <string> odwiedzone;
	map<pair<string,string>, double> odleglosci; 
	void nowy_kurs (kurs nowy)
	{
		string scisnij;
		for(int i=0; i<nowy.postoje.size(); i++)
		{
			scisnij+=nowy.postoje[i].stop_id;
		}
		bool ok=1;
		vector <pair<double, double> > combo;
		double przebyta=0;
		string skrot=md5(scisnij);
		if(odwiedzone.find(skrot)==odwiedzone.end())
		{
			odwiedzone.insert(skrot);
			for(int i=0; i<nowy.postoje.size()-1; i++)
			{
				odleglosci[make_pair(skrot,nowy.postoje[i].stop_id)]=przebyta;
				vector <pair <double, double> > tmp1 = rou.get_route(nowy.postoje[i].stop_id, nowy.postoje[i+1].stop_id);
				for(int j=1; j<tmp1.size(); j++)
				{
					przebyta+=calcDistanceInMeters(tmp1[j-1].first, tmp1[j-1].second, tmp1[j].first, tmp1[j].second);
				}
				if(tmp1.size()==0)
					ok=0;
				combo.insert(combo.end(), tmp1.begin(), tmp1.end());
				scisnij+=nowy.postoje[i].stop_id;
			}
			odleglosci[make_pair(skrot,nowy.postoje[nowy.postoje.size()-1].stop_id)]=przebyta;
		}
		else
			ok=0;
		if(ok)
		{
			for(int i=0; i<combo.size(); i++)
			{
				shapes<<skrot<<","<<combo[i].first<<","<<combo[i].second<<","<<i<<endl;
			}
		}
		trips<<nowy.linia<<"/"<<nowy.id<<","<<nowy.linia<<","<<nowy.dni<<","<<skrot<<endl;
		for(int i=0; i<nowy.postoje.size(); i++)
		{
			stop_times<<nowy.linia<<"/"<<nowy.id<<","<<godz(nowy.postoje[i].time)<<":00,"<<godz(nowy.postoje[i].time)<<":00,"<<nowy.postoje[i].stop_id<<","<<i<<",,,,";
			stop_times<<odleglosci[make_pair(skrot, nowy.postoje[i].stop_id)]<<endl;
		}
	}
	foo(string sciezka) : rou(), stop_times("stop_times.txt", ios::out | ios::trunc), trips("trips.txt", ios::out | ios::trunc), shapes("shapes.txt", ios::out | ios::trunc)
	{
		shapes<<"shape_id,shape_pt_lat,shape_pt_lon,shape_pt_sequence"<<endl;
		trips<<"trip_id,route_id,service_id,shape_id"<<endl;
		stop_times<<"trip_id,arrival_time,departure_time,stop_id,stop_sequence,stop_headsign,pickup_type,drop_off_type,shape_dist_traveled"<<endl;
		shapes.precision(9);
		ScheduleReaderZtm reader(sciezka, this);
		reader.run();
	}
};

int main(int argc, char** argv)
{
	string argvv=argv[1];
	foo abc(argvv);
}
