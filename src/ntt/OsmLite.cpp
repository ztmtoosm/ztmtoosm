#include "osmbase/osm_base.hpp"
#include "sqlite3.h"
#include "sqlite_my.hpp"
#include "RouteInfoGenerator/OsmStopData.hpp"
int main(int argc, char** argv)
{
  string sciezka = argv[1];

  osm_base osmData(sciezka);

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
               "STOP_POSITION_NAME CHAR(255)," \
               ");";
      sqlite_execute_easy(db, sql, 2);
  sqlite3_close(db);
}
