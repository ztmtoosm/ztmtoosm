#include "ScheduleReader/ScheduleReader.hpp"
#include "ScheduleReader/ScheduleReaderSzczecin.hpp"
#include "ScheduleReader/ScheduleReaderGdansk.hpp"
#include "sqlite_my.h"
#include <libpq-fe.h>
#include <PgSql.h>
using namespace std;


class MyHand : public ScheduleHandler
{
  int kk = 0;
  PGconn* db;
public:
  MyHand(PGconn* db2) : db(db2) {}
  void nowa_linia(string nazwa, vector <vector <string> > trasy);
  void nowy_przystanek(przystanek nowy);
  void newkalendar(string date, vector<string> caltypes);
  void nowy_kurs(kurs p);
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
        }
    }
}

void MyHand::nowy_przystanek(przystanek nowy)
{
  InsertionPreparator preparator("OPERATOR_STOPS");
  preparator.add("STOP_ID", nowy.id);
  preparator.add("NAME", nowy.name);
  preparator.add("LON", nowy.lon);
  preparator.add("LAT", nowy.lat);
  preparator.add("COORDINATES_QUALITY", nowy.wsp_jakosc);
  preparator.add("MORE_INFO", nowy.stopinfo);
  preparator.doIt(db);
}

void MyHand::newkalendar(string date, vector<string> caltypes)
{
  for(string type : caltypes)
  {
    InsertionPreparator preparator("OPERATOR_CALENDAR");
    preparator.add("DATE", date);
    preparator.add("DAY_TYPE", type);
    preparator.doIt(db);
  }
}


void MyHand::nowy_kurs(kurs p)
{
  int i = 0;
  for(postoj k : p.postoje)
  {
    postoj lat = p.postoje[p.postoje.size()-1];
    InsertionPreparator preparator("SCHEDULE");
    preparator.add("LINE", p.linia);
    preparator.add("TRIP", kk);
    preparator.add("NEXT_STOP_TRIP", i);
    preparator.add("TIME_SECONDS", k.time);
    preparator.add("DAY_TYPE", p.dni);
    preparator.add("STOP_ID", k.stop_id);
    preparator.add("DIRECTION", lat.stop_id);
    preparator.doIt(db);
    i++;
  }
  kk++;
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

  PGconn *conn = PQconnectdb("host=db user=marcin password=marcin dbname=nexty");

  if (PQstatus(conn) == CONNECTION_BAD) {
    fprintf(stderr, "Connection to database failed: %s\n",
    PQerrorMessage(conn));
  }
  StartStopPreparator().add("DELETE FROM OPERATOR_ROUTES").doIt(conn, "");
  StartStopPreparator().add("DELETE FROM OPERATOR_STOPS").doIt(conn, "");
    StartStopPreparator().add("DELETE FROM OPERATOR_CALENDAR").doIt(conn, "");
      StartStopPreparator().add("DELETE FROM SCHEDULE").doIt(conn, "");
  StartStopPreparator().add("BEGIN").doIt(conn, "");
  StartStopPreparator prep2;
  prep2.add("ROUTE_ID varchar(10) NOT NULL")
       .add("DIRECTION integer NOT NULL")
       .add("STOP_ON_DIRECTION_NUMBER integer NOT NULL")
       .add("STOP_ID varchar(10)")
       .add("PRIMARY KEY(ROUTE_ID, DIRECTION, STOP_ON_DIRECTION_NUMBER)");
  StartStopPreparator().add("CREATE TABLE IF NOT EXISTS OPERATOR_ROUTES").add(prep2, ",", "(").doIt(conn, "");

  StartStopPreparator prep3;
  prep3.add("STOP_ID VARCHAR(10) NOT NULL PRIMARY KEY")
       .add("NAME VARCHAR(100)")
       .add("LON REAL")
       .add("LAT REAL")
       .add("COORDINATES_QUALITY INTEGER")
       .add("MORE_INFO TEXT");
  StartStopPreparator().add("CREATE TABLE IF NOT EXISTS OPERATOR_STOPS").add(prep3, ",", "(").doIt(conn, "");

  StartStopPreparator prep4;
  prep4.add("LINE varchar(10)");
  prep4.add("TRIP integer");
  prep4.add("NEXT_STOP_TRIP integer");
  prep4.add("TIME_SECONDS integer");
  prep4.add("DAY_TYPE varchar(3)");
  prep4.add("STOP_ID varchar(6)");
  prep4.add("DIRECTION varchar(6)");
  StartStopPreparator().add("CREATE TABLE IF NOT EXISTS SCHEDULE").add(prep4, ",", "(").doIt(conn, "");
  StartStopPreparator().add("CREATE INDEX ON SCHEDULE(TRIP, NEXT_STOP_TRIP)").doIt(conn, "");
    StartStopPreparator().add("CREATE INDEX ON SCHEDULE(LINE)").doIt(conn, "");
      StartStopPreparator().add("CREATE INDEX ON SCHEDULE(STOP_ID, TIME_SECONDS)").doIt(conn, "");
  StartStopPreparator prep5;
  prep5.add("DATE varchar(20)");
  prep5.add("DAY_TYPE varchar(3)");
  prep5.add("UNIQUE(DATE, DAY_TYPE)");
  StartStopPreparator().add("CREATE TABLE IF NOT EXISTS OPERATOR_CALENDAR").add(prep5, ",", "(").doIt(conn, "");

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
  StartStopPreparator().add("COMMIT").doIt(conn, "");
  PQfinish(conn);

  return 0;
}
