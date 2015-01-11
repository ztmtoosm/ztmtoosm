struct cos2
{
	dijkstra* dij;
	string nazwa;
	int wariantId;
	vector <long long> stopPositions;
	vector <long long> stopSigns;
	osm_base* bazaOsm;
	void init1()
	{
		<vector <przystanek_big> przystanki=bazaZtm->daneLinia[nazwa][wariantId];
		for(int i=0; i<przystanki.size(); i++)
		{
			stopPostitions.push_back(przystanki[i].stop_position);
			if(przystanki[i].bus_stop!=0)
			{
				stopSigns.push_back(przystanki[i].bus_stop);
			}
			else
			{
				stopSigns.push_back(przystanki[i].stop_position);
			}
		}
	}
	void init2()
	{
		dij_data d0(stopPostitions, dij);
		if(!d0.ok)
		{
			map <long long, set<long long> >::iterator it1=d0.split_data.begin();
			while(it1!=d0.split_data.end())
			{
				vector <pair<long long, vector <long long> > > rozd=bazaOsm->rozdziel_way(it1->first, it1->second);
				for(int i=0; i<rozd.size(); i++)
				{
					for(int j=0; j<rozd[i].second.size()-1; j++)
					{
						dij->dij_podmien(it1->first, rozd[i].first, rozd[i].second[j], rozd[i].second[j+1]);
						dij->dij_podmien(it1->first, rozd[i].first, rozd[i].second[j+1], rozd[i].second[j]);
					}
				}
				it1++;
			}
		}

	}
	cos2(osm_base* bazaOsmW, dijkstra* dijW, ztmread_for_html* bazaZtm, string nazwaW, int wariantIdW)
	{
		bazaOsm=bazaOsmW;
		dij=dijW;
		nazwa=nazwaW;
		wariantIdW=wariantId;
		init1();
		init2();	
	}
};
