#ifndef RAPORTPRZYSTANKI
#define RAPORTPRZYSTANKI
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
public:
	RaportPrzystanki(fstream& json2Stream, map <string, OsmStopData>& osmStopDataW, ScheduleHandlerInternal* bazaZtmW, string miastoW, osm_base* bazaOsmW)
		: osmStopData(osmStopDataW), bazaZtm(bazaZtmW), miasto(miastoW), bazaOsm(bazaOsmW)
	{
		int jsonTableRowCount = 0;
		json2Stream.precision(9);
		json2Stream<<"[";
		for(auto& it1 : osmStopData)
		{
			if(bazaZtm->przystanki.find(it1.first)!=bazaZtm->przystanki.end())
			{
				int powod = 1000;
				if(it1.second.stop_position==0)
					powod = 1;
				auto& it2 = bazaZtm->przystanki[it1.first];
				stringstream line;
				line.precision(9);
				if(jsonTableRowCount>0)
					line<<",";
				line<<"{";
				line<<"\"id\":\""<<it1.first<<"\"";
				line<<",\"name\":\""<<escapeJsonString(it2.name)<<"\"";
				line<<",\"lon\": "<<it2.lon<<" ";
				line<<",\"lat\": "<<it2.lat<<" ";
				line<<",\"latlon_jakosc\": "<<it2.wsp_jakosc<<" ";
				double lat2, lon2;
				bool okWsp2 = getWsp(lat2, lon2, 'N', it1.second.stop_position, 'N', it1.second.bus_stop);
				if(okWsp2)
				{
					line<<",\"lon2\": "<<lon2<<" ";
					line<<",\"lat2\": "<<lat2<<" ";
				}
				else
				{
				}
				line<<",\"kierunki\":[";
				vector <string> kierunki=przystanekKierunki(it1.first);
				for(int i=0; i<kierunki.size(); i++)
				{
					if(i>0)
						line<<",";
					line<<"\""<<escapeJsonString(kierunki[i])<<"\"";
				}
				line<<"]";
				line<<",\"bus_stop\":"<<it1.second.bus_stop<<" ";
				line<<",\"bus_stop_name\":\""<<escapeJsonString(wyszName('N', it1.second.bus_stop))<<"\"";
				line<<",\"stop_position\": "<<it1.second.stop_position<<" ";
				line<<",\"stop_position_name\":\""<<escapeJsonString(wyszName('N', it1.second.stop_position))<<"\"";
				//if(wyszName('N', it1.second.stop_position).length()>0)
				//	wspStream<<"	"<<wyszName('N', it1.second.stop_position)<<endl;
				//else
				//	wspStream<<"	"<<it2.name<<" "<<it1.first[4]<<it1.first[5]<<endl;
				line<<",\"platform\": "<<it1.second.platform<<" ";
				line<<",\"platform_name\":\""<<escapeJsonString(wyszName(it1.second.platform_type, it1.second.platform))<<"\"";
				line<<",\"additional\":\""<<escapeJsonString(it2.stopinfo+" ; "+it2.miejscowosc)<<"\"";
				line<<",\"BS_SP\": "<<getDistance('N', it1.second.bus_stop, 'N', it1.second.stop_position);
				if(it1.second.bus_stop!=0 && powod==1000 && getDistance('N', it1.second.bus_stop, 'N', it1.second.stop_position)>150)
					powod = 3;
				line<<",\"SP_PL\": "<<getDistance(it1.second.platform_type, it1.second.platform, 'N', it1.second.stop_position);
				line<<",\"PL_BS\": "<<getDistance('N', it1.second.bus_stop, it1.second.platform_type, it1.second.platform);
				line<<",\"powod\": "<<powod;
				if(it1.second.platform_type=='N' || it1.second.platform_type=='W' || it1.second.platform_type=='R')
					line<<",\"platform_type\":\""<<it1.second.platform_type<<"\"";

				/*
				line<<"<tr id=\""<<it1.first<<"\">";
				line<<"<td>"<<it1.first<<"</td>";
				line<<"<td>"<<it2.name<<"</td>";
				line<<"<td>"<<it2.lon<<"</td>";
				line<<"<td>"<<it2.lat<<"</td>";
				line<<"<td>"<<it2.stopinfo<<"</td>";
				line<<"<td>"<<divOsmLink(it1.second.bus_stop, 'N')<<"</td>";
				line<<"<td>"<<divOsmLink(it1.second.stop_position, 'N')<<"</td>";
				line<<"<td>"<<divOsmLink(it1.second.platform, it1.second.platform_type)<<"</td>";
				vector <string> kierunki=przystanekKierunki(it1.first);
				line<<"<td>"<<kierunki[0]<<"</td>";
				line<<"<td>"<<kierunki[1]<<"</td>";
				line<<"<td>"<<kierunki[2]<<"</td>";*/
				/*
				string refDiv = htmlgen::div("komorka", "", it1.first);
				string refName = htmlgen::div("komorka", "", it1.second.name);
				string k1 = htmlgen::div("komorka", "", kierunki[0]);
				string k2 = htmlgen::div("komorka", "", kierunki[1]);
				string k3 = htmlgen::div("komorka", "", kierunki[2]);
				string row[] = {refDiv, refName, divOsmLink(it1.second.bus_stop, 'N'), divOsmLink(it1.second.stop_position, 'N'), divOsmLink(it1.second.platform, it1.second.platform_type), k1, k2, k3};
				*/
				line<<"}"<<endl;
				json2Stream<<line.str();
				jsonTableRowCount++;
			}
		}
		for(auto& it2 : bazaZtm->przystanki)
		{
			if(osmStopData.find(it2.first)==osmStopData.end())
			{
				int powod = 1000;
				stringstream line;
				if(jsonTableRowCount>0)
					line<<",";
				line<<"{";
				line<<"\"id\":\""<<it2.first<<"\"";
				line<<",\"name\":\""<<escapeJsonString(it2.second.name)<<"\"";
				line<<",\"lon\": "<<it2.second.lon<<" ";
				line<<",\"lat\": "<<it2.second.lat<<" ";
				line<<",\"latlon_jakosc\": "<<it2.second.wsp_jakosc<<" ";
				line<<",\"powod\": "<<powod;
				line<<",\"kierunki\":[";
				//wspStream<<it2.first<<"	"<<it2.second.lon<<"	"<<it2.second.lat<<"	"<<it2.second.name<<endl;
				vector <string> kierunki=przystanekKierunki(it2.first);
				for(int i=0; i<kierunki.size(); i++)
				{
					if(i>0)
						line<<",";
					line<<"\""<<escapeJsonString(kierunki[i])<<"\"";
				}
				line<<"]";
				line<<"}"<<endl;
				json2Stream<<line.str();
				jsonTableRowCount++;
			}
		}

		//przystankiHTMLStream<<divOsmTable(tabela)<<endl;
		json2Stream<<"]";
		json2Stream.close();
	}
};
#endif
