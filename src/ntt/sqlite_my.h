#ifndef SQLITE_MY
#define SQLITE_MY
#include "sqlite3.h"
static int callback(void *NotUsed, int argc, char **argv, char **azColName);
void sqlite_execute_easy(sqlite3* db, string pol, int errorcode);
void create_tables_if_not_exist(sqlite3* db);
string if0(int val);
#endif
