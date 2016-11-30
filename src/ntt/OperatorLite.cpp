#include "ScheduleReader/ScheduleReader.hpp"
#include "ScheduleReader/ScheduleReaderSzczecin.hpp"
#include "ScheduleReader/ScheduleReaderGdansk.hpp"
#include "sqlite3.h"
#include "sqlite_my.h"
using namespace std;

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
  sql << nowy.id << "', '%q', " << nowy.lon << ", " << nowy.lat << ", " << nowy.wsp_jakosc << ", '%q');";
  char *zSQL = sqlite3_mprintf(sql.str().c_str(), nowy.name.c_str(), nowy.stopinfo.c_str());
  string zSQL2 = zSQL;
  sqlite3_free(zSQL);

  sqlite_execute_easy(db, zSQL2, 4);
}

int main(int argc, char** argv)
{
  string cityName = argv[1];
  string operatorFile = argv[2];
  string databaseFile = argv[3];

  sqlite3 *db;
  int rc;
  rc = sqlite3_open(databaseFile.c_str(), &db);

  if(rc)
  {
    return 1;
  }

  create_tables_if_not_exist(db);
  string sql = "BEGIN; DELETE FROM OPERATOR_ROUTES; DELETE FROM OPERATOR_STOPS;";
  sqlite_execute_easy(db, sql, 2);

  MyHand hnd(db);
  if(cityName == "Warszawa")
  {
    ScheduleReaderWarszawa reader(operatorFile, &hnd);
    reader.run();
  }
  if(cityName == "Gdańsk")
  {
    ScheduleReaderGdansk reader(operatorFile, &hnd);
    reader.run();
  }
  if(cityName == "Szczecin")
  {
    ScheduleReaderSzczecin reader(operatorFile, &hnd);
    reader.run();
  }
  sqlite_execute_easy(db, "COMMIT;", 5);
  sqlite3_close(db);
  return 0;
}
