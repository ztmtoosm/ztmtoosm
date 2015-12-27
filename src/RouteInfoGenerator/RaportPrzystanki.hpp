#ifndef RAPORTPRZYSTANKI
#define RAPORTPRZYSTANKI
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
using namespace rapidjson;

/*
 *
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

using namespace rapidjson;
using namespace std;
.
int main() {
		StringBuffer s;
		Writer<StringBuffer> writer(s);

		writer.StartObject();
		writer.String("hello");
		writer.String("world");
		writer.String("t");
		writer.Bool(true);
		writer.String("f");
		writer.Bool(false);
		writer.String("n");
		writer.Null();
		writer.String("i");
		writer.Uint(123);
		writer.String("pi");
		writer.Double(3.1416);
		writer.String("a");
		writer.StartArray();
		for (unsigned i = 0; i < 4; i++)
				writer.Uint(i);
		writer.EndArray();
		writer.EndObject();

		cout << s.GetString() << endl;

		return 0;
}*/

class RaportPrzystanki
{
	string miasto;
	map <string, OsmStopData>& osmStopData;
	ScheduleHandlerInternal* bazaZtm;
	osm_base* bazaOsm;
	string dokladnePrzystanki(string idPrzystanek, string idLinia, int idWariantu, int idKol)
	{
		string poprzedni = "POCZĄTKOWY";
		string kolejny = "KOŃCOWY";
		string poprzedniId, kolejnyId;
		if(idKol>0)
		{
			poprzedniId = bazaZtm->dane_linia[idLinia][idWariantu][idKol-1];
			poprzedni = bazaZtm->przystanki[poprzedniId].name;
		}
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+1)
		{
			kolejnyId = bazaZtm->dane_linia[idLinia][idWariantu][idKol+1];
			kolejny = bazaZtm->przystanki[kolejnyId].name;
		}
		string aktualny = bazaZtm->przystanki[idPrzystanek].name;;
		string ostatniId = bazaZtm->dane_linia[idLinia][idWariantu][bazaZtm->dane_linia[idLinia][idWariantu].size()-1];
		string ostatni = bazaZtm->przystanki[ostatniId].name;
		string pierwszyId = bazaZtm->dane_linia[idLinia][idWariantu][0];
		string pierwszy = bazaZtm->przystanki[pierwszyId].name;
		string info=htmlgen::link("Pelne"+miasto+".html#pelne"+idLinia, idLinia);
		info+=": ";
		if(idKol>1)
		{
			info+=htmlgen::link("#"+pierwszyId, pierwszy+" ("+pierwszyId+")");
			info+=" - ";
		}
		if(idKol>2)
		{
			info+="... - ";
		}
		if(idKol>0)
		{
			info+=htmlgen::link("#"+poprzedniId, poprzedni+" ("+poprzedniId+")");
			info+=" - ";
		}
		info+=aktualny+" ("+idPrzystanek+")";
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+1)
		{
			info+=" - ";
			info+=htmlgen::link("#"+kolejnyId, kolejny+" ("+kolejnyId+")");
		}
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+3)
		{
			info+=" - ...";
		}
		if(bazaZtm->dane_linia[idLinia][idWariantu].size()>idKol+2)
		{
			info+=" - ";
			info+=htmlgen::link("#"+ostatniId, ostatni+" ("+ostatniId+")");
		}
		return info;
	}
	vector <string> przystanekKierunki(string p)
	{
		vector <string> wynik;
		for(auto& it2 : bazaZtm->dane_linia)
		{
			for(int i=0; i<it2.second.size(); i++)
			{
				for(int j=0; j<it2.second[i].size(); j++)
				{
					if(it2.second[i][j]==p)
						wynik.push_back(dokladnePrzystanki(p, it2.first, i, j));
				}
			}
		}
		return wynik;
	}



	string wyszName(char type, long long id)
	{
		if(type=='N' && bazaOsm->nodes.find(id)!=bazaOsm->nodes.end())
		{
			return bazaOsm->nodes[id].getTags()["name"];
		}
		if(type=='W' && bazaOsm->nodes.find(id)!=bazaOsm->nodes.end())
		{
			return bazaOsm->ways[id].getTags()["name"];
		}
		if(type=='R' && bazaOsm->nodes.find(id)!=bazaOsm->nodes.end())
		{
			return bazaOsm->relations[id].getTags()["name"];
		}
		return "";
	}

	bool getLatLon(char type, long long id, double& lat, double& lon)
	{
		if(type=='R' && bazaOsm->relations.find(id)!=bazaOsm->relations.end())
		{
			relation& r = bazaOsm->relations[id];
			for(int i=0; i<r.members.size(); i++)
			{
				if(r.members[i].member_type==WAY)
				{
					id=r.members[i].member_id;
					type='W';
				}
			}
		}

		if(type=='W' && bazaOsm->ways.find(id)!=bazaOsm->ways.end())
		{
			way& r = bazaOsm->ways[id];
			for(int i=0; i<r.nodes.size(); i++)
			{
				id=r.nodes[i];
				type='N';
			}
		}

		if(type=='N' && bazaOsm->nodes.find(id)!=bazaOsm->nodes.end())
		{
			lat = bazaOsm->nodes[id].lat;
			lon = bazaOsm->nodes[id].lon;
			return true;
		}
		return false;
	}

	double getDistance(char type1, long long id1, char type2, long long id2)
	{
		double lat1, lon1;
		double lat2, lon2;
		bool t1 = getLatLon(type1, id1, lat1, lon1);
		bool t2 = getLatLon(type2, id2, lat2, lon2);
		if(t1 && t2)
		{
			return distance(lon1, lat1, lon2, lat2)*1000.0;
		}
		return -1.0;
	}

	bool getWsp(double& lat, double& lon, char type1, long long id1, char type2, long long id2)
	{
		bool t1 = getLatLon(type1, id1, lat, lon);
		if(t1)
			return true;
		bool t2 = getLatLon(type2, id2, lat, lon);
		if(t2)
			return true;
		return false;
	}

	std::string escapeJsonString(const std::string& input) {
		std::ostringstream ss;
		for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
		//C++98/03:
		//for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
			switch (*iter) {
				case '\\': ss << "\\\\"; break;
				case '"': ss << "\\\""; break;
				case '/': ss << "\\/"; break;
				case '\b': ss << "\\b"; break;
				case '\f': ss << "\\f"; break;
				case '\n': ss << "\\n"; break;
				case '\r': ss << "\\r"; break;
				case '\t': ss << "\\t"; break;
				default: ss << *iter; break;
			}
		}
		return ss.str();
	}

	int printStop(pair<string, OsmStopData> it1, Writer<StringBuffer>& writer)
	{
		int powod = 1000;
		if(it1.second.stop_position==0)
			powod = 1;
		bool found = bazaZtm->przystanki.find(it1.first)!=bazaZtm->przystanki.end();
		writer.String("id"); writer.String(it1.first.c_str());
		writer.StartObject();

		if(found)
		{
			auto& it2 = bazaZtm->przystanki[it1.first];
			writer.String("name"); writer.String(it2.name.c_str());
			writer.String("lon"); writer.Double(it2.lon);
			writer.String("lat"); writer.Double(it2.lat);
			writer.String("latlon_jakosc"); writer.Int(it2.wsp_jakosc);
			writer.String("additional"); writer.String((it2.stopinfo+" ; "+it2.miejscowosc).c_str());
			writer.String("kierunki");
			writer.StartArray();
			vector <string> kierunki=przystanekKierunki(it1.first);
			for(int i=0; i<kierunki.size(); i++)
			{
				writer.String(kierunki[i].c_str());
			}
			writer.EndArray();
		}
		else
			powod = 10;
		double lat2, lon2;
		bool okWsp2 = getWsp(lat2, lon2, 'N', it1.second.stop_position, 'N', it1.second.bus_stop);
		if(okWsp2)
		{
			writer.String("lon2"); writer.Double(lon2);
			writer.String("lat2"); writer.Double(lat2);
		}

		writer.String("bus_stop"); writer.Int64(it1.second.bus_stop);
		writer.String("bus_stop_name"); writer.String(wyszName('N', it1.second.bus_stop).c_str());
		writer.String("stop_position"); writer.Int64(it1.second.stop_position);
		writer.String("stop_position_name"); writer.String(wyszName('N', it1.second.stop_position).c_str());
		writer.String("platform"); writer.Int64(it1.second.platform);
		writer.String("platform_name"); writer.String(wyszName(it1.second.platform_type, it1.second.platform).c_str());

		writer.String("BS_SP"); writer.Double(getDistance('N', it1.second.bus_stop, 'N', it1.second.stop_position));
		if(it1.second.bus_stop!=0 && powod==1000 && getDistance('N', it1.second.bus_stop, 'N', it1.second.stop_position)>150)
			powod = 3;
		writer.String("SP_PL"); writer.Double(getDistance(it1.second.platform_type, it1.second.platform, 'N', it1.second.stop_position));
		writer.String("PL_BS"); writer.Double(getDistance('N', it1.second.bus_stop, it1.second.platform_type, it1.second.platform));
		writer.String("powod"); writer.Int(powod);
		if(it1.second.platform_type=='N' || it1.second.platform_type=='W' || it1.second.platform_type=='R')
		{
			writer.String("platform_type"); writer.String(string(""+it1.second.platform_type).c_str());
		}
		writer.EndObject();
		return powod;
	}

public:
	RaportPrzystanki(fstream& json2Stream, map <string, OsmStopData>& osmStopDataW, ScheduleHandlerInternal* bazaZtmW, string miastoW, osm_base* bazaOsmW)
		: osmStopData(osmStopDataW), bazaZtm(bazaZtmW), miasto(miastoW), bazaOsm(bazaOsmW)
	{
		json2Stream.precision(9);

		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartArray();

		for(auto& it1 : osmStopData)
		{
			if(bazaZtm->przystanki.find(it1.first)!=bazaZtm->przystanki.end())
			{
				printStop(make_pair(it1.first, it1.second), writer);
			}
		}
		for(auto& it2 : bazaZtm->przystanki)
		{
			if(osmStopData.find(it2.first)==osmStopData.end())
			{
				OsmStopData puste;
				printStop(make_pair(it2.first, puste), writer);
			}
		}
		writer.EndArray();
		json2Stream<<s.GetString();
		json2Stream.close();
	}
};
#endif
