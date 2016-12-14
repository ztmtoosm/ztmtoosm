#include <vector>
#include <map>
#include <cstring>
#include <libpq-fe.h>
#include <iostream>
using namespace std;
class InsertionPreparator
{
  map <string, string> list;
  string stmt;
public:
  InsertionPreparator(string stmtx);
  void add(string key, string str);
  void add(string key, int val);
  void add(string key, long long val);
  void add(string key, double val);
  void addNull(string key);
  void doIt(PGconn* connection);
};

class StartStopPreparator
{
  vector <string> str;
public:
  string toString(string comma=",", string delim="")
  {
    string ret = "";
    int counter = 0;
    for(string it1 : str)
    {
      if(counter)
        ret += comma+" ";
      ret += it1;
      counter++;
    }
    string delim2 = delim;
    if(delim == "(")
      delim2 = ")";
    return delim + ret + delim2;
  }
  StartStopPreparator& add(string x)
  {
    str.push_back(x);
    return *this;
  }
  StartStopPreparator& add(StartStopPreparator& x, string comma=",", string delim="")
  {
    str.push_back(x.toString(comma, delim));
    return *this;
  }
  void doIt(PGconn* connection, string comma=",", string delim="")
  {
    string str = toString(comma, delim);
    cerr << str << endl;
    PGresult *res = PQexec(connection, str.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << PQresultErrorMessage(res) << endl;
    }
  }
};
