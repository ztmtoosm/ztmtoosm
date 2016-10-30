#include "ScheduleReader/ScheduleReader.hpp"
#include "ScheduleReader/ScheduleReaderSzczecin.hpp"
#include "ScheduleReader/ScheduleReaderGdansk.hpp"
#include "sqlite_my.h"
#include <libpq-fe.h>
#include <PgSql.h>
using namespace std;


class MyHand : public ScheduleHandler
{
  PGconn* db;
public:
  MyHand(PGconn* db2) : db(db2) {}
  void nowa_linia(string nazwa, vector <vector <string> > trasy);
  void nowy_przystanek(przystanek nowy);
};

void MyHand::nowa_linia(string nazwa, vector <vector <string> > trasy)
{
  InsertionPreparator preparator("OPERATOR_ROUTES");
  for(int i=0; i<trasy.size(); i++)
    {
      for(int j=0; j<trasy[i].size(); j++)
        {
          preparator.add("ROUTE_ID", nazwa);
          preparator.add("DIRECTION", i+1);
          preparator.add("STOP_ON_DIRECTION_NUMBER", j+1);
          preparator.add("STOP_ID", trasy[i][j]);
          preparator.doIt(db);
          //stringstream sql;
          //sql << "INSERT INTO OPERATOR_ROUTES(ROUTE_ID, DIRECTION, STOP_ON_DIRECTION_NUMBER, STOP_ID) VALUES('";
          //sql << nazwa << "', " << i+1 << ", " << j+1 << ", '" << trasy[i][j] << "');";
          //PQexec(db, sql.str().c_str());
        }
    }
}

void MyHand::nowy_przystanek(przystanek nowy)
{
  InsertionPreparator preparator("OPERATOR_STOPS");
  preparator.add("STOP_ID", nowy.id);
  preparator.add("NAME", nowy.name.c_str());
  preparator.add("LON", nowy.lon);
  preparator.add("LAT", nowy.lat);
  preparator.add("COORDINATES_QUALITY", nowy.wsp_jakosc);
  preparator.add("MORE_INFO", nowy.stopinfo);
  preparator.doIt(db);
}

void do_exit(PGconn *conn, PGresult *res) {
      
          fprintf(stderr, "%s\n", PQerrorMessage(conn));    

              PQclear(res);
                  PQfinish(conn);    
                      
                          exit(1);
}




int main(int argc, char** argv)
{
  string cityName = argv[1];
  string operatorFile = argv[2];

  PGconn *conn = PQconnectdb("user=marcin dbname=nexty");

  if (PQstatus(conn) == CONNECTION_BAD) {
    fprintf(stderr, "Connection to database failed: %s\n",
    PQerrorMessage(conn));
  }

  StartStopPreparator prep2;
  prep2.add("ROUTE_ID varchar(10) NOT NULL")
       .add("DIRECTION integer NOT NULL")
       .add("STOP_ON_DIRECTION_NUMBER integer NOT NULL")
       .add("STOP_ID varchar(10)")
       .add("PRIMARY KEY(ROUTE_ID, DIRECTION, STOP_ON_DIRECTION_NUMBER)");
  StartStopPreparator().add("CREATE TABLE IF NOT EXISTS OPERATOR_ROUTES").add(prep2, ",", "(").doIt(conn, "");

  StartStopPreparator prep3;
  prep3.add("STOP_ID CHAR(10) NOT NULL PRIMARY KEY")
       .add("NAME CHAR(100)")
       .add("LON REAL")
       .add("LAT REAL")
       .add("COORDINATES_QUALITY INTEGER")
       .add("MORE_INFO TEXT");
  StartStopPreparator().add("CREATE TABLE IF NOT EXISTS OPERATOR_STOPS").add(prep3, ",", "(").doIt(conn, "");

  MyHand hnd(conn);
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

  PQfinish(conn);

  return 0;
}

/*
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
}*/
