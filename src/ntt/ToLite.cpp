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

class MyHand : public ScheduleHandler
{
  sqlite3* db;
  public:
  MyHand(sqlite3* db2) : db(db2) {}
  void nowa_linia(string nazwa, vector <vector <string> > trasy);
};

void MyHand::nowa_linia(string nazwa, vector <vector <string> > trasy)
{
  for(int i=0; i<trasy.size(); i++)
  {
    for(int j=0; j<trasy[i].size(); j++)
    {
      char *zErrMsg = 0;
      int rc;
      stringstream sql;
      sql << "INSERT INTO OPERATOR_ROUTES(ROUTE_ID, DIRECTION, STOP_ON_DIRECTION_NUMBER, STOP_ID) VALUES('";
      sql << nazwa << "', " << i+1 << ", " << j+1 << ", '" << trasy[i][j] << "');";
      rc = sqlite3_exec(db, sql.str().c_str(), callback, 0, &zErrMsg);
      if(rc != SQLITE_OK)
      {
        //cerr << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
        exit(3);
      }
    }
  }
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
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  if(rc != SQLITE_OK)
  {
    //cerr << "SQL error: " << zErrMsg << endl;
    sqlite3_free(zErrMsg);
    return 2;
  }

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
