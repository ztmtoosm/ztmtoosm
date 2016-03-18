#include "osmbase/osm_base.hpp"
#include "sqlite3.h"
#include "sqlite_my.h"
#include "RouteInfoGenerator/OsmStopData.hpp"
int main(int argc, char** argv)
{
  string OsmFile = argv[1];

  osm_base osmData(OsmFile);
  map<string, OsmStopData> data = loadOsmStopData(&osmData);
  
  cout<<data.size()<<endl;

  string databaseFile = argv[2];
  sqlite3 *db;
  int rc;
  rc = sqlite3_open(databaseFile.c_str(), &db);
  if(rc)
  {
    return 1;
  }

  create_tables_if_not_exist(db);
  string sql = "BEGIN; DELETE FROM OSM_STOPS;";
  sqlite_execute_easy(db, sql, 2);
  cout<<"OK1"<<endl;
  for(auto& it1 : data)
  {
    string aktName = it1.first;
    OsmStopData aktData = it1.second;
    stringstream sql;
    sql << "INSERT INTO OSM_STOPS(NORMAL_STOP, STOP_POSITION, REF_ID, STOP_POSITION_NAME) VALUES(";
    sql << if0(aktData.bus_stop) << ", " << if0(aktData.stop_position) << ",'" << aktName << "', %Q);";
    char *zSQL = sqlite3_mprintf(sql.str().c_str(), (aktData.name.length() > 0 ? aktData.name.c_str() : NULL));
    string zSQL2 = zSQL;
    sqlite3_free(zSQL);
    sqlite_execute_easy(db, zSQL2, 3);
  }
  sqlite_execute_easy(db, "COMMIT;", 5);
  sqlite3_close(db);
}
