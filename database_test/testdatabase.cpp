#include <iostream>
#include <pqxx/pqxx>
#include <sstream>
using namespace std;
int getDatabaseNodeId(long long foo, pqxx::work& txn)
{
	stringstream polecenie;
	polecenie<<"SELECT key_column FROM planet_osm_nodes WHERE id="<<foo<<";";
	pqxx::result r = txn.exec(polecenie.str());
	if(r.size()<=0)
		return -1;
	cout<<r[0][0].as<int>()<<endl;
	return r[0][0].as<int>();
}

int main(int, char *argv[])
{
	double id1, id2;
	cin>>id1>>id2;
	pqxx::connection c("dbname=gis user=root");
	pqxx::work txn(c);
	long long idd1=id1*10000000;
	long long idd2=id2*10000000;
	stringstream polecenie;
	polecenie<<"SELECT lat, lon, id FROM planet_osm_nodes WHERE lat>"<<idd1-1000000<<" AND lat<"<<idd1+1000000;
    	polecenie<<"ORDER BY geom  <-> ST_GeometryFromText('POINT("<<id1<<" "<<id2<<")',4326) LIMIT 1";
	pqxx::result r = txn.exec(polecenie.str());
	std::cout.precision(9);
	std::cout<<"[";
	for(int i=0; i<r.size(); i++)
	{
		double y = r[i][0].as<int>()/10000000.0;
		double x = r[i][1].as<int>()/10000000.0;
		if(i>0)
			std::cout<<",";
		std::cout<<"{ \"y\":"<<y<<", \"x\":"<<x<<", \"id\": "<<r[i][2]<<std::endl;
	}  
	std::cout<<"]";
}
