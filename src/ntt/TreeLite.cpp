#include "osmbase/osm_base.hpp"
#include "RouteInfoGenerator/PrzegladanieCzyPrawidloweStareLinie.hpp"
#include "sqlite3.h"
#include "sqlite_my.hpp"
using namespace std;

void addExtractedRef(osm_base* base, sqlite3* db, long long current)
{
	set<string> list = extract_ref(base, current, "ref");
	for(string it1 : list)
	{
		stringstream sql;
		sql << "INSERT INTO OSM_RELATIONS(RELATION_ID, REF_ID) VALUES (" << current << ", %Q)";
		char *zSQL = sqlite3_mprintf(sql.str().c_str(), it1.c_str());
	  string zSQL2 = zSQL;
	  sqlite3_free(zSQL);
    sqlite_execute_easy(db, zSQL2, 3);
	}
}

void browseTree (osm_base* base, sqlite3* db, long long parent, long long current)
{
	if(base->relations.find(current) == base->relations.end())
		return;
	relation akt = base->relations[current];
	map <string, string> tags = akt.getTags();
	string type = "a";
	if(tags["type"] == "route")
	{
		type = "r";
	}
	if(tags["type"] == "route_master")
	{
		type = "m";
	}
	stringstream sql;
      sql << "INSERT INTO OSM_TREE(RELATION_ID, RELATION_PARENT, NAME, TYPE, ROUTE_ID) VALUES(";
      sql << current << ", ";
			if(parent != 0)
				sql << parent;
			else
				sql << "NULL";
			sql << ",%Q, '" << type << "',%Q);";
		string aktName = tags["name"];
		string aktRouteId = tags["ref"];
		char *zSQL = sqlite3_mprintf(sql.str().c_str(), (aktName.length() > 0 ? aktName.c_str() : NULL), (aktRouteId.length() > 0 ? aktRouteId.c_str() : NULL));
	  string zSQL2 = zSQL;
	  sqlite3_free(zSQL);
      sqlite_execute_easy(db, zSQL2, 3);


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
	string osmFile = argv[1];
  string databaseFile = argv[2];
  osm_base osmData(osmFile);
  sqlite3 *db;
  int rc;
  rc = sqlite3_open(databaseFile.c_str(), &db);

  if(rc)
  {
    return 1;
  }

  string sql = "BEGIN; CREATE TABLE IF NOT EXISTS OSM_TREE("  \
               "RELATION_ID BIGINT," \
               "RELATION_PARENT BIGINT," \
               "NAME CHAR(255)," \
               "TYPE CHAR(1)," \
               "ROUTE_ID CHAR(20)); DELETE FROM OSM_TREE;";
      sqlite_execute_easy(db, sql, 2);
  sql = "CREATE TABLE IF NOT EXISTS OSM_RELATIONS("  \
               "RELATION_ID BIGINT," \
               "REF_ID CHAR(10)); DELETE FROM OSM_RELATIONS;";
      sqlite_execute_easy(db, sql, 2);
  
	browseTree(&osmData, db, 0, atoi(argv[3]));
	sqlite_execute_easy(db, "COMMIT;", 5);
  sqlite3_close(db);
  return 0;
}
