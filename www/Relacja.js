var Relacja = function (manager, podstawa, track_type)
{
	this.track_type_id = 0;
	if(track_type == 'tram')
		this.track_type_id = 1;
	this.manager = manager;
	this.tablicaZnacznikow = [];
	this.tablicaZnacznikowFeatures = [];
	this.tablicaLacznikow = [];
	this.tablicaLacznikowFeatures = [];
	this.zmienna = function(fet, wsp, these)
	{
		these.zmienionyZnacznik(wsp, fet);
	};
	manager.znacznikHandler = this;
	manager.lineHandler = this;
	manager.ppmHandler = this;
	manager.zmienionyZnacznik = this.zmienna;
	manager.onClickLine = function(fet, wsp, these) {
		for(var i=0; i<these.tablicaLacznikowFeatures.length; i++)
		{
			if(these.tablicaLacznikowFeatures[i][0]==fet || these.tablicaLacznikowFeatures[i][1]==fet)
			{
				these.znacznikWPolowie(i, wsp);
			}
		}
	};
	manager.onPpm = function(wsp, these) {
		these.nowyZnacznikNaKoncu(wsp);
	};
	console.log(manager);
	if(podstawa!=undefined)
	{
		for(var i=0; i<podstawa.length; i++)
		{
			var xxx = loadJSON(punktId(podstawa[i]));
			this.nowyZnacznikNaKoncu2(xxx);
		}
	}
}

Relacja.prototype.reopen = function() {
	for(var i=0; i<this.tablicaZnacznikow.length; i++)
	{
		var zzz = this.tablicaZnacznikow[i];
		this.tablicaZnacznikowFeatures[i]=this.manager.nowyZnacznikFizycznie([zzz.x, zzz.y], i);	
	}
	for(var i=0; i<this.tablicaLacznikow.length; i++)
	{
		var wspolrzedne = this.tablicaLacznikow[i];
		this.tablicaLacznikowFeatures[i] = this.manager.nowaLiniaFizycznie(wspolrzedne, i);
	}
	this.zmienna = function(fet, wsp, these)
	{
		these.zmienionyZnacznik(wsp, fet);
	};
	this.manager.znacznikHandler = this;
	this.manager.lineHandler = this;
	this.manager.ppmHandler = this;
	this.manager.zmienionyZnacznik = this.zmienna;
	this.manager.onClickLine = function(fet, wsp, these) {
		for(var i=0; i<these.tablicaLacznikowFeatures.length; i++)
		{
			if(these.tablicaLacznikowFeatures[i][0]==fet || these.tablicaLacznikowFeatures[i][1]==fet)
			{
				these.znacznikWPolowie(i, wsp);
			}
		}
	};
	this.manager.onPpm = function(wsp, these) {
		these.nowyZnacznikNaKoncu(wsp);
	};
	if(this.onReopen!=undefined)
	{
		this.onReopen(this.handler);
	}
}

Relacja.prototype.wezPolaczenia = function (osmId1, osmId2) {
	var lacznik = loadJSON(punktySciezka(osmId1, osmId2, this.track_type_id));
	return lacznik;
}

Relacja.prototype.nowyZnacznikNaKoncu = function (wsp)
{
	var zzz=loadJSON(punktSciezka(wsp[1], wsp[0]));
	console.log(zzz);
	this.nowyZnacznikNaKoncu2(zzz);
}

Relacja.prototype.nowyZnacznikNaKoncu2 = function (zzz)
{
	var zn = this.manager.nowyZnacznikFizycznie([zzz.x, zzz.y], this.tablicaZnacznikow.length);
	this.tablicaZnacznikow[this.tablicaZnacznikow.length] = zzz;
	console.log(zzz);
	this.tablicaZnacznikowFeatures[this.tablicaZnacznikowFeatures.length] = zn;
	if(this.tablicaZnacznikow.length>1)
	{
		var wspolrzedne = this.wezPolaczenia(this.tablicaZnacznikow[this.tablicaZnacznikow.length-2].id, this.tablicaZnacznikow[this.tablicaZnacznikow.length-1].id);
		var features = this.manager.nowaLiniaFizycznie(wspolrzedne, this.tablicaZnacznikow.length-1);
		this.tablicaLacznikow[this.tablicaLacznikow.length] = wspolrzedne;
		this.tablicaLacznikowFeatures[this.tablicaLacznikowFeatures.length] = features;
	}
}

Relacja.prototype.znacznikWPolowie = function (id, wsp)
{
	console.log("...");
	console.log(id);
	console.log(wsp);
	var distance = 1000.0;
	var id_tmp = -1;
	for(var i=0; i<this.tablicaLacznikow[id].length; i++)
	{
	             var line = new ol.geom.LineString([[this.tablicaLacznikow[id][i].x, this.tablicaLacznikow[id][i].y], wsp]);
		          var tmp_distance= line.getLength();
			  if(tmp_distance<distance)
			  {
				  distance = tmp_distance;
				  id_tmp = i;
			  }
	}
	if(id_tmp>0 && id_tmp<this.tablicaLacznikow[id].length-1)
	{
		var zntmp = this.tablicaLacznikow[id][id_tmp];
		console.log(zntmp);
		console.log("hhhhh");
		for (var i=this.tablicaZnacznikow.length-1; i>id; i--)
		{
			console.log(this.tablicaZnacznikowFeatures);
			this.tablicaZnacznikow[i+1]=this.tablicaZnacznikow[i];
			this.tablicaZnacznikowFeatures[i+1]=this.tablicaZnacznikowFeatures[i];
			this.manager.znacznikZmienId(this.tablicaZnacznikowFeatures[i+1], i+1);
		}
		for (var i=this.tablicaLacznikow.length-1; i>id; i--)
		{
			this.tablicaLacznikow[i+1]=this.tablicaLacznikow[i];
			this.tablicaLacznikowFeatures[i+1]=this.tablicaLacznikowFeatures[i];
			this.manager.liniaZmienId(this.tablicaLacznikowFeatures[i+1]);
		}
		this.manager.usunLinie(this.tablicaLacznikowFeatures[id]);
		this.tablicaZnacznikowFeatures[id+1]=this.manager.nowyZnacznikFizycznie([zntmp.x, zntmp.y], id+1);
		this.tablicaZnacznikow[id+1]=zntmp;
		var ws = this.wezPolaczenia(this.tablicaZnacznikow[id].id,this.tablicaZnacznikow[id+1].id);
		var nl = this.manager.nowaLiniaFizycznie(ws, id);
		this.tablicaLacznikow[id]=ws;
		this.tablicaLacznikowFeatures[id]=nl;
		var ws2 = this.wezPolaczenia(this.tablicaZnacznikow[id+1].id,this.tablicaZnacznikow[id+2].id);
		var nl2 = this.manager.nowaLiniaFizycznie(ws2, id+1);
		this.tablicaLacznikow[id+1]=ws2;
		this.tablicaLacznikowFeatures[id+1]=nl2;
	}
	if(this.onChange!=undefined)
	{
		this.onChange(this.handler);
	}
}
Relacja.prototype.zmienionyZnacznik = function (wsp, ref)
{
	var id = ref.id;
	var zzz=loadJSON(punktSciezka(wsp[1], wsp[0]));
	this.manager.usunPunkt(ref);
	var zn = this.manager.nowyZnacznikFizycznie([zzz.x, zzz.y], id);
	this.tablicaZnacznikow[id] = zzz;
	this.tablicaZnacznikowFeatures[id] = zn;
	var n = this.tablicaZnacznikow.length;
	if(zn.id<n-1)
	{
		this.manager.usunLinie(this.tablicaLacznikowFeatures[zn.id]);
		var wspolrzedne = this.wezPolaczenia(this.tablicaZnacznikow[id].id, this.tablicaZnacznikow[id+1].id);
		var nl=this.manager.nowaLiniaFizycznie(wspolrzedne, id);
		this.tablicaLacznikow[zn.id]=wspolrzedne;
		this.tablicaLacznikowFeatures[zn.id]=nl;
	}
	if(zn.id>0)
	{
		this.manager.usunLinie(this.tablicaLacznikowFeatures[zn.id-1]);
		var wspolrzedne = this.wezPolaczenia(this.tablicaZnacznikow[id-1].id, this.tablicaZnacznikow[id].id);
		var nl=this.manager.nowaLiniaFizycznie(wspolrzedne, id-1);
		this.tablicaLacznikow[zn.id-1]=wspolrzedne;
		this.tablicaLacznikowFeatures[zn.id-1]=nl;
	}
	if(this.onChange!=undefined)
	{
		this.onChange(this.handler);
	}
}
Relacja.prototype.clear = function()
{
	for(var i=0; i<this.tablicaZnacznikowFeatures.length; i++)
	{
		this.manager.usunPunkt(this.tablicaZnacznikowFeatures[i]);
	}
	for(var i=0; i<this.tablicaLacznikowFeatures.length; i++)
	{
		this.manager.usunLinie(this.tablicaLacznikowFeatures[i]);
	}
	if(this.onClear!=undefined)
	{
		this.onClear(this.handler);
	}
}
Relacja.prototype.getTrackPoints = function()
{
	var akt = -1;
	var wynik =[];
	var wynik_tmp =[];
	for (var i=0; i<this.tablicaLacznikow.length; i++)
	{
		for (var j=0; j<this.tablicaLacznikow[i].length; j++)
		{
			if(wynik_tmp.length==0 || wynik_tmp[wynik_tmp.length-1]!=this.tablicaLacznikow[i][j].id)	
				wynik_tmp[wynik_tmp.length]=this.tablicaLacznikow[i][j].id;
		}
	}
	wynik[0]=wynik_tmp;
	return wynik;
}
