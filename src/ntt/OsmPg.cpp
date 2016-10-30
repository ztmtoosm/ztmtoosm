#include "osmbase/osm_base.hpp"
#include "sqlite3.h"
#include "sqlite_my.h"
#include "RouteInfoGenerator/OsmStopData.hpp"
int main(int argc, char** argv)
{
  string OsmFile = argv[1];

  PGconn *conn = PQconnectdb("user=marcin dbname=nexty");

  if (PQstatus(conn) == CONNECTION_BAD) {
    fprintf(stderr, "Connection to database failed: %s\n",
    PQerrorMessage(conn));
  }

const string OSM_STOPS3 = "CREATE TABLE IF NOT EXISTS OSM_STOPS("  \
	"OSM_ID BIGINT," \
	"OSM_TYPE CHAR," \
	"REF_ID CHAR(10) NOT NULL," \
	"tag_key CHAR(255)," \
	"tag_value CHAR(255)" \
	");";


PGresult *res = PQexec(conn, OSM_STOPS3.c_str());
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            do_exit(conn, res);
                }
  
  osm_base osmData(OsmFile);
  map<string, OsmStopData> data = loadOsmStopData(&osmData);
  
  cout<<data.size()<<endl;

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
