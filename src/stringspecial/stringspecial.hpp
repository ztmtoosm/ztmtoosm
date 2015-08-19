#ifndef STRINGSPECIAL_HPP
#define STRINGSPECIAL_HPP
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <fstream>
#include <sstream>
#include <map>
#include <cstring>
#include <vector>
using namespace std;
string avoid_cudzyslow(string slowo);
int toint(string dat);
string tostring(int alfa);
double distance(double long1, double lat1, double long2, double lat2);
double distance(long long laong1, long long laat1, long long laong2, long long laat2);
int stringtoint(string abc);
double stringtodouble(string abc);
string inttostring (int abc);
string doubletostring (double abc);
vector <string> rozdziel(char* slowo);
double todouble(string data);
int get_times (string time);
string godz(int godzina);
template <typename T1> T1 fromstring(string alfa)
{
	stringstream foo;
	foo<<alfa;
	T1 foo2;
	foo>>foo2;
	return foo2;
}
template <typename T1> string toXstring(T1 alfa)
{
	stringstream foo;
	foo<<alfa;
	string foo2;
	foo>>foo2;
	return foo2;
}
string substituteWhiteCharsBySpace(string nazwa);
struct dat
{
	vector <string> dd;
	string get(int id)
	{
		return dd[id];
	}
	dat()
	{
		char slowo[200000];
		cin.getline(slowo, 200000);
		dd=rozdziel(slowo);
	}
	int size()
	{
		return dd.size();
	}
};
bool isStringOnlyWithDigits(string nastepne);
#endif
