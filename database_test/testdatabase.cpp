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
	long long id1, id2;
	cin>>id1>>id2;
	pqxx::connection c("dbname=gis user=root");
	pqxx::work txn(c);
	int idd1=getDatabaseNodeId(id1, txn);
	int idd2=getDatabaseNodeId(id2, txn);
	stringstream polecenie;
	polecenie<<"SELECT lat,lon, b.id FROM pgr_dijkstra(' \
	SELECT key_column AS id,\
	source::int,\
	target::int,\
	vals::double precision AS cost\
	FROM ways2',"<<idd1<<", "<<idd2<<", false, false) a JOIN planet_osm_nodes b ON id1=key_column ORDER BY seq";
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
