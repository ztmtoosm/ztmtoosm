#include "osmbase/osm_base.hpp"
#include "sqlite3.h"
#include "sqlite_my.hpp"
#include "RouteInfoGenerator/OsmStopData.hpp"
int main(int argc, char** argv)
{
  string sciezka = argv[1];

  osm_base osmData(sciezka);
  map<string, OsmStopData> data = loadOsmStopData(&osmData);
  
cout<<data.size()<<endl;

  string tmp2 = argv[2];
  sqlite3 *db;
  int rc;
  rc = sqlite3_open(tmp2.c_str(), &db);
  if(rc)
  {
    return 1;
  }

  string sql = "CREATE TABLE IF NOT EXISTS OSM_STOPS("  \
               "NORMAL_STOP BIGINT," \
               "STOP_POSITION BIGINT," \
               "REF_ID CHAR(10) PRIMARY KEY," \
               "NORMAL_STOP_NAME CHAR(255)," \
               "STOP_POSITION_NAME CHAR(255)" \
               ");";
      sqlite_execute_easy(db, sql, 2);
      cout<<"OK1"<<endl;
        for(auto& it1 : data)
  {
	  string aktName = it1.first;
	  OsmStopData aktData = it1.second;
	  stringstream sql;
      sql << "INSERT INTO OSM_STOPS(NORMAL_STOP, STOP_POSITION, REF_ID, STOP_POSITION_NAME) VALUES(";
      sql << aktData.bus_stop << ", " << aktData.stop_position << ",'" << aktName << "', '%q');";
      char *zSQL = sqlite3_mprintf(sql.str().c_str(), aktData.name.c_str());
	  string zSQL2 = zSQL;
	  sqlite3_free(zSQL);
      sqlite_execute_easy(db, zSQL2, 3);
  	cout<<"OK2"<<" "<<it1.first<<endl;
	}
  
  sqlite3_close(db);
}
