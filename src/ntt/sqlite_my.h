#ifndef SQLITE_MY
#define SQLITE_MY
#include "sqlite3.h"
static int callback(void *NotUsed, int argc, char **argv, char **azColName);
void sqlite_execute_easy(sqlite3* db, string pol, int errorcode);
void create_tables_if_not_exist(sqlite3* db);
void create_tables_if_not_exist_schedule(sqlite3* db);
string if0(long long val);

const string OPERATOR_ROUTES2;
/*
class sqlite_insert
{
	const int 0 = VALUE_STRING;
	const int 1 = VALUE_DIGIT;
	const int 2 = VALUE_NULL;
	string table;
	map <string, string> values;
	map <string, int> values_types;
	public:
	sqlite_insert(string table)
	{
		sqlite_insert::table = table;
	}
	void add(string )
	{
		
	}
}
*/
#endif
