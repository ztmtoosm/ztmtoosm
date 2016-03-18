#include <cstring>
#include <sstream>
#include "sqlite3.h"
using namespace std;
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  return 0;
}

void sqlite_execute_easy(sqlite3* db, string pol, int errorcode)
{
  char *zErrMsg = 0;
  int rc;
  rc = sqlite3_exec(db, pol.c_str(), callback, 0, &zErrMsg);
  if(rc != SQLITE_OK)
  {
    //cerr << "SQL error: " << zErrMsg << endl;
    sqlite3_free(zErrMsg);
    exit(errorcode);
  }
}
void create_tables_if_not_exist(sqlite3* db)
{
  string sql = "CREATE TABLE IF NOT EXISTS OSM_TREE("  \
               "RELATION_ID BIGINT PRIMARY KEY NOT NULL," \
               "RELATION_PARENT BIGINT," \
               "NAME CHAR(255)," \
               "TYPE CHAR(1)," \
               "ROUTE_ID CHAR(20), FOREIGN KEY(RELATION_PARENT) REFERENCES OSM_TREE(RELATION_ID));";
  sqlite_execute_easy(db, sql, 2);

  sql = "CREATE TABLE IF NOT EXISTS OSM_RELATIONS("  \
        "RELATION_ID BIGINT NOT NULL," \
        "REF_ID CHAR(10) NOT NULL, PRIMARY KEY(RELATION_ID, REF_ID));";
  sqlite_execute_easy(db, sql, 2);

  sql = "CREATE TABLE IF NOT EXISTS OSM_STOPS("  \
        "NORMAL_STOP BIGINT," \
        "STOP_POSITION BIGINT," \
        "REF_ID CHAR(10) PRIMARY KEY NOT NULL," \
        "NORMAL_STOP_NAME CHAR(255)," \
        "STOP_POSITION_NAME CHAR(255)" \
        ");";
  sqlite_execute_easy(db, sql, 2);

  sql = "CREATE TABLE IF NOT EXISTS OPERATOR_ROUTES("  \
        "ROUTE_ID CHAR(20) NOT NULL," \
        "DIRECTION INTEGER NOT NULL," \
        "STOP_ON_DIRECTION_NUMBER INTEGER NOT NULL," \
        "STOP_ID CHAR(10), PRIMARY KEY(ROUTE_ID, DIRECTION, STOP_ON_DIRECTION_NUMBER));";
  sqlite_execute_easy(db, sql, 2);

  sql = "CREATE TABLE IF NOT EXISTS OPERATOR_STOPS("  \
        "STOP_ID CHAR(10) NOT NULL PRIMARY KEY," \
        "NAME CHAR(100)," \
        "LON REAL," \
        "LAT REAL," \
        "COORDINATES_QUALITY INTEGER," \
        "MORE_INFO TEXT);";
  sqlite_execute_easy(db, sql, 2);
}

string if0(int val)
{
  if(val == 0)
    return "NULL";
  stringstream foo;
  foo<<val;
  return foo.str();
}
