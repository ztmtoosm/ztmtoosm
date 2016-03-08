#include "ScheduleReader/ScheduleReader.hpp"
#include "ScheduleReader/ScheduleReaderSzczecin.hpp"
#include "ScheduleReader/ScheduleReaderGdansk.hpp"
#include "sqlite3.h"
using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   /*int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");*/
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

class MyHand : public ScheduleHandler
{
  sqlite3* db;
  public:
  MyHand(sqlite3* db2) : db(db2) {}
  void nowa_linia(string nazwa, vector <vector <string> > trasy);
  void nowy_przystanek(przystanek nowy);
};

void MyHand::nowa_linia(string nazwa, vector <vector <string> > trasy)
{
  for(int i=0; i<trasy.size(); i++)
  {
    for(int j=0; j<trasy[i].size(); j++)
    {
      stringstream sql;
      sql << "INSERT INTO OPERATOR_ROUTES(ROUTE_ID, DIRECTION, STOP_ON_DIRECTION_NUMBER, STOP_ID) VALUES('";
      sql << nazwa << "', " << i+1 << ", " << j+1 << ", '" << trasy[i][j] << "');";
      sqlite_execute_easy(db, sql.str(), 3);
    }
  }
}

void MyHand::nowy_przystanek(przystanek nowy)
{
  stringstream sql;
  sql << "INSERT INTO OPERATOR_STOPS(STOP_ID, NAME, LON, LAT, COORDINATES_QUALITY, MORE_INFO) VALUES('";
  sql << nowy.id << "', '" << nowy.name << "', " << nowy.lon << ", " << nowy.lat << ", " << nowy.wsp_jakosc << ", '" << nowy.stopinfo << "');";
  sqlite_execute_easy(db, sql.str(), 4);
}

int main(int argc, char** argv)
{
	string tmp0 = argv[1];
  string tmp1 = argv[2];
  string tmp2 = argv[3];

  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  rc = sqlite3_open(tmp2.c_str(), &db);

  if(rc)
  {
    return 1;
  }

  string sql = "CREATE TABLE IF NOT EXISTS OPERATOR_ROUTES("  \
               "ROUTE_ID CHAR(20)," \
               "DIRECTION INTEGER," \
               "STOP_ON_DIRECTION_NUMBER INTEGER," \
               "STOP_ID CHAR(10));";
      sqlite_execute_easy(db, sql, 2);

      sql = "CREATE TABLE IF NOT EXISTS OPERATOR_STOPS("  \
                     "STOP_ID CHAR(10) PRIMARY KEY," \
                     "NAME CHAR(100)," \
                     "LON REAL," \
                     "LAT REAL," \
                     "COORDINATES_QUALITY INTEGER," \
                     "MORE_INFO TEXT;";
            sqlite_execute_easy(db, sql, 2);

  MyHand hnd(db);
  if(tmp0 == "Warszawa")
  {
    ScheduleReaderWarszawa reader(tmp1, &hnd);
    reader.run();
  }
  if(tmp0 == "Gdańsk")
  {
    ScheduleReaderGdansk reader(tmp1, &hnd);
    reader.run();
  }
  if(tmp0 == "Szczecin")
  {
    ScheduleReaderSzczecin reader(tmp1, &hnd);
    reader.run();
  }
  sqlite3_close(db);
  return 0;
}
