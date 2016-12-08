#include "osmbase/osm_base.hpp"
#include "RouteInfoGenerator/PrzegladanieCzyPrawidloweStareLinie.hpp"
#include "RouteInfoGenerator/RouteCohesion.hpp"
#include "RouteInfoGenerator/WypisywanieWspolrzednychTras.hpp"
#include "sqlite3.h"
#include "sqlite_my.h"
#include <libpq-fe.h>
#include <PgSql.h>

vector <long long> rels;

void addExtractedRef(osm_base* base, PGconn* db, long long current)
{
  set<string> list = extract_ref(base, current, "ref");
  for(string it1 : list)
  {
    InsertionPreparator prep("OSM_RELATIONS");
    prep.add("RELATION_ID", current);
    prep.add("REF_ID", it1);
    prep.doIt(db);
  }
}

void browseTree (osm_base* base, PGconn* db, long long parent, long long current)
{
  if(base->relations.find(current) == base->relations.end())
    return;
  relation akt = base->relations[current];
  map <string, string> tags = akt.getTags();
  string type = "a";
  InsertionPreparator prep("OSM_TREE");
  string cohesionType = "X";
  if(tags["type"] == "route")
  {
    rels.push_back(current);
    if(!relationCohesion(current, base))
      cohesionType = "N";
    else
      cohesionType = "Y";

    type = "r";

  }
  if(tags["type"] == "route_master")
  {
    type = "m";
  }
  string aktName = tags["name"];
  string aktRouteId = tags["ref"];
  prep.add("COHESION", cohesionType);
  prep.add("RELATION_ID", current);
  (parent != 0 ? prep.add("RELATION_PARENT", parent) : prep.addNull("RELATION_PARENT"));
  (aktRouteId.length() > 0 ? prep.add("NAME", aktName) : prep.addNull("NAME"));
  prep.add("TYPE", type);
  (aktRouteId.length() > 0 ? prep.add("ROUTE_ID", aktRouteId) : prep.addNull("ROUTE_ID"));
  prep.doIt(db);

  if(tags["type"] == "route")
  {
    addExtractedRef(base, db, current);
    return;
  }
  int s1=akt.members.size();
  for(int i=0; i<s1; i++)
  {
    if(akt.members[i].member_type == RELATION)
    {
      long long teraz_id = akt.members[i].member_id;
      browseTree(base,db, current, teraz_id);
    }
  }
}


int main(int argc, char** argv)
{
  string OsmFile = argv[1];

  PGconn *conn = PQconnectdb("host=localhost user=marcin dbname=nexty");

  if (PQstatus(conn) == CONNECTION_BAD) {
    fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
  }
  StartStopPreparator().add("DELETE FROM OSM_STOPS").doIt(conn, "");
  StartStopPreparator().add("DELETE FROM OSM_TREE").doIt(conn, "");
  StartStopPreparator().add("DELETE FROM OSM_RELATIONS").doIt(conn, "");

  StartStopPreparator prep2;
  prep2.add("NORMAL_STOP BIGINT")
      .add("STOP_POSITION BIGINT")
      .add("REF_ID VARCHAR(10) PRIMARY KEY NOT NULL")
      .add("NORMAL_STOP_NAME VARCHAR(255)")
      .add("STOP_POSITION_NAME VARCHAR(255)");
  StartStopPreparator().add("CREATE TABLE IF NOT EXISTS OSM_STOPS").add(prep2, ",", "(").doIt(conn, "");
  cerr << OsmFile << endl;
  osm_base osmData(OsmFile);
  map<string, OsmStopData> data = loadOsmStopData(&osmData);
  StartStopPreparator prep4;

  prep4.add("RELATION_ID BIGINT PRIMARY KEY NOT NULL")
      .add("RELATION_PARENT BIGINT")
      .add("NAME VARCHAR(255)")
      .add("TYPE VARCHAR(1)")
      .add("COHESION VARCHAR(1)")
      .add("ROUTE_ID VARCHAR(20)")
      .add("FOREIGN KEY(RELATION_PARENT) REFERENCES OSM_TREE(RELATION_ID)");
  StartStopPreparator().add("CREATE TABLE IF NOT EXISTS OSM_TREE").add(prep4, ",", "(").doIt(conn, "");
  StartStopPreparator prep3;
  prep3.add("RELATION_ID BIGINT NOT NULL")
      .add("REF_ID VARCHAR(10) NOT NULL, PRIMARY KEY(RELATION_ID, REF_ID)");
  StartStopPreparator().add("CREATE TABLE IF NOT EXISTS OSM_RELATIONS").add(prep3, ",", "(").doIt(conn, "");

  //StartStopPreparator().add("BEGIN").doIt(conn, "");

  for(auto& it1 : data)
  {
    string aktName = it1.first;
    OsmStopData aktData = it1.second;
    InsertionPreparator prep("OSM_STOPS");
    prep.add("NORMAL_STOP", aktData.bus_stop);
    prep.add("STOP_POSITION", aktData.stop_position);
    prep.add("REF_ID", aktName);
    (aktData.name.length() > 0 ? prep.add("STOP_POSITION_NAME", aktData.name) : prep.addNull("STOP_POSITION_NAME"));
    prep.doIt(conn);
  }
  browseTree(&osmData, conn, 0, atoi(argv[3]));
  //WypisywanieWspolrzednychTras(rels, &osmData, "foo.xxx");
  //StartStopPreparator().add("COMMIT").doIt(conn, "");
}
