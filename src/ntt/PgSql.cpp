#include <libpq-fe.h>
#include "PgSql.h"
#include <map>
#include <cstring>
#include <iostream>
using namespace std;
InsertionPreparator::InsertionPreparator(string stmtx) : stmt(stmtx) {}
void InsertionPreparator::add(string key, string str)
{
  list[key] = str;
}
void InsertionPreparator::add(string key, int val)
{
  list[key] = to_string(val);
}
void InsertionPreparator::add(string key, long long val)
{
  list[key] = to_string(val);
}
void InsertionPreparator::add(string key, double val)
{
  list[key] = to_string(val);
}
void InsertionPreparator::addNull(string key)
{
  list[key] = "$#$#$";
}
void InsertionPreparator::doIt(PGconn* connection)
{
  string str1 = "INSERT INTO "+stmt;
  string str2 = "(";
  bool ok = false;
  for(auto& it1 : list)
  {
    if(ok)
      str2 += ", ";
    str2 += it1.first;
    ok = true;
  }
  str2 += ")";
  str1 += str2 + " VALUES(";
  int lcz = 0;
  for(auto& it1 : list)
  {
    if(lcz > 0)
      str1 += ", ";
    str1 += "$"+to_string(lcz+1);
    lcz++;
  }
  str1 += ")";
  const char* abc[list.size()];
  int licz = 0;
  for(auto& it2 : list)
  {
    if(it2.second == "$#$#$")
      abc[licz] = NULL;
    else
      abc[licz] = it2.second.c_str();
    licz++;
  }
  PGresult *res = PQexecParams(connection, str1.c_str(), list.size(), NULL, abc, NULL, NULL, 0);
  list.clear();
}
