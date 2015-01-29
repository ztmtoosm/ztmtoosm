#include <stdlib.h>
#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

struct sql_routes
{
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
	sql_routes()
	{
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "");
		con->setSchema("osm");
		stmt = con->createStatement();
	}
	vector <pair <double, double> > get_route (string from, string to)
	{
		vector <pair <double, double> > wynik;
		try {
		res = stmt->executeQuery("SELECT * FROM relacje WHERE fromid=\""+from+"\" AND toid=\""+to+"\"");
		while (res->next()) 
		{
			wynik.push_back(pair<double,double>(res->getDouble(4), res->getDouble(5)));
		}
		}
		catch (sql::SQLException &e) {}
		return wynik;
	}
};
