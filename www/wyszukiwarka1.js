var tribe = 0;
var palette = ['#ab0d15'
,'#F7931D'
/*,'#FFF200'*/
,'#0DB14B'
,'#0071BC'
,'#00B9F1'
,'#903F98'
,'#EE609D'];
function getpalette(poz, n)
{
	return palette[Math.floor(poz%7)];
}
function getpaletteid(poz, n)
{
	return Math.floor(poz%7);
}

function loadJSON(url) 
{
	var resp;
	var xmlHttp;
	resp  = '';
	xmlHttp = new XMLHttpRequest();

	if(xmlHttp != null)
	{
		xmlHttp.open( "GET", url, false );
		xmlHttp.send( null );
		resp = xmlHttp.responseText;
	}
	return JSON.parse(resp); 
}
function podrozSciezka (from, to, time)
{
	var sciezka="/hafas";
	sciezka+='\?from\='+from;
	sciezka+='\&to\='+to;
	sciezka+='\&time\='+time;
	console.log(sciezka);
	return sciezka;
}
function polaczeniaSciezka (from, delim, time)
{
	var sciezka="/hafas";
	sciezka+='\?from\='+from;
	sciezka+='\&delim\='+delim;
	sciezka+='\&time\='+time;
	return sciezka;
}
function kursSciezka (from, line, time)
{
	var sciezka="/hafas";
	sciezka+='\?from\='+from;
	sciezka+='\&line\='+line;
	sciezka+='\&time\='+time;
	return sciezka;
}
function addLineToSource (coordinates, colorLine, name, vectorSource1, vectorSource2)
{
	var warstwa1 = new ol.Feature({geometry: new ol.geom.LineString(coordinates)});
	var warstwaStroke1 = new ol.style.Stroke ({color: colorLine, width: 4});
	var warstwaStrokeText1 = new ol.style.Stroke ({color: '#fff', width: 6});
	var warstwaFont1 = '18px helvetica,sans-serif,bold';
	var warstwaFill1 = new ol.style.Fill({color: colorLine});
	var warstwaText1 = new ol.style.Text({text: name, font: warstwaFont1, fill: warstwaFill1, stroke: warstwaStrokeText1});
	var warstwaStyle1 =new ol.style.Style({stroke : warstwaStroke1, text : warstwaText1});
	warstwa1.setStyle(warstwaStyle1);
	var warstwa2 = new ol.Feature({geometry: new ol.geom.LineString(coordinates)});
	var warstwaStroke2 = new ol.style.Stroke ({color: '#fff', width: 7});
	var warstwaStyle2 = new ol.style.Style({stroke : warstwaStroke2});
	warstwa2.setStyle(warstwaStyle2);
	vectorSource1.addFeature(warstwa1);
	vectorSource2.addFeature(warstwa2);
}
var PrzystanekNaMapie = function (name, id, coordinates, vectorSource)
{
	this.name = name;
	this.coordinates = coordinates;
	this.propsedColor = null;
	this.id = id;
	this.minTime = null;
	this.minTimeLine = null;
	this.maxTime = null;
	this.maxTimeLine = null;
	this.feature = new ol.Feature({geometry: new ol.geom.Point(coordinates)});
	this.feature.setId(id);
	vectorSource.addFeature(this.feature);
};

PrzystanekNaMapie.prototype.addLine = function (time, line, color)
{
	console.log(time);
	if(this.propsedColor == null)
		this.propsedColor = color;
	else
		this.propsedColor = '#000';
	if(this.minTime == null || time < this.mintime)
	{
		this.minTime = time;
		this.minTimeLine = line;
	}
	if(this.maxTime == null || time < this.maxtime)
	{
		this.maxTime = time;
		this.maxTimeLine = line;
	}
};

PrzystanekNaMapie.prototype.setBasicStyle = function ()
{
	this.feature.setStyle(
		new ol.style.Style({
	    image: new ol.style.Circle({
	      fill: new ol.style.Fill({
		color: this.propsedColor
	      }),
	      radius: 8,
	      stroke: new ol.style.Stroke({
		color: '#fff',
		width: 2
	      })
	    })
	  }));
};

PrzystanekNaMapie.prototype.setExtraStyle = function ()
{
	var name = this.name;
	var warstwaStrokeText1 = new ol.style.Stroke ({color: '#fff', width: 6});
	var warstwaFont1 = '18px helvetica,sans-serif,bold';
	var warstwaFill1 = new ol.style.Fill({color: this.propsedColor});
	var txt = new ol.style.Text({text: name, font: warstwaFont1, fill: warstwaFill1, stroke: warstwaStrokeText1});
	this.feature.setStyle(
		new ol.style.Style({
	    image: new ol.style.Circle({
	      fill: new ol.style.Fill({
		color: this.propsedColor
	      }),
	      radius: 12,
	      stroke: new ol.style.Stroke({
		color: '#fff',
		width: 4
	      })
	    }), text : txt
	  }));

};

function stringTime(time)
{
				var tim=new Date(time*1000);
var mins=tim.getMinutes();
				var hours=tim.getHours();
				var timString="";
				if(hours<10)
					timString+="0";
				timString+=hours+":";
				if(mins<10)
					timString+="0";
				timString+=mins;
				return timString;
}

var WizualizacjaPodrozy = function (from, toOrLine, time, type)
{
	this.minLat=100.0;
	this.maxLat=-100.0;
	this.minLon=100.0;
	this.maxLon=-100.0;
	map.removeLayer(layerLines2);
	map.removeLayer(layerLines);
	this.warstwaVector1 = new ol.source.Vector({});
	this.warstwaVector2 = new ol.source.Vector({});
	
	layerLines = new ol.layer.Vector({
	source: this.warstwaVector2,
	style: function(feature, resolution)
	{
		return style[feature.getGeometry().getType()];
	}
	});
	layerLines2 = new ol.layer.Vector({
	source: this.warstwaVector1,
	style: function(feature, resolution) {
	return style2[feature.getGeometry().getType()];
	}
	});
 	map.addLayer(layerLines);
        map.addLayer(layerLines2);
	if(type==0)
	{
		this.bazaJSON = loadJSON(podrozSciezka(from, toOrLine, time));
	}
	else
	{
		this.bazaJSON = loadJSON(kursSciezka(from, toOrLine, time));
	}
	this.heart=document.getElementById("heart");
	heart.innerHTML="";
	var podstawa = this.bazaJSON;
	this.przystanki = [];
	for(var i=0; i < this.bazaJSON.length; i++)
	{
		var liczbaPrzystankow=0;
		var nowalinia=document.createElement("DIV");
		var nowalinia_kolorek=document.createElement("DIV");
		var nowaliniaid=document.createElement("DIV");
		var nowalinia_srodkowe=document.createElement("DIV");
		var nowaliniaid_number=document.createElement("DIV");
		var nowaliniaid_more=document.createElement("DIV");
		var nowaliniaid_morea=document.createElement("A");

		nowaliniaid_number.innerHTML=podstawa[i].line;
		nowaliniaid_morea.href="javascript:void(0)";
		nowaliniaid_morea.innerHTML=">>>";
		nowaliniaid.className="hafasliniaid";
		nowaliniaid_number.className="hafasliniaid_number";
		nowaliniaid_more.className="hafasliniaid_more";
		nowalinia_srodkowe.className="hafaslinia_srodek";
		nowaliniaid_morea.powiazany=nowalinia_srodkowe;
		if(type==0)
			nowalinia_srodkowe.style.display="none";
		nowaliniaid_morea.onclick=function(e)
		{
			console.log(this.powiazany);
			if(this.powiazany.style.display=="block")
			{
				this.innerHTML=">>>";
				this.powiazany.style.display="none";
			}
			else
			{
				this.innerHTML="^^^";
				this.powiazany.style.display="block";
			}
		};
		if(type==0)
		{
			nowaliniaid_number.onclick=function(e)
			{
				new WizualizacjaPodrozy(this.fooId, this.innerHTML, this.fooTime, 1);
			}
		}
		nowalinia.appendChild(nowaliniaid);
		nowaliniaid.appendChild(nowaliniaid_number);
		if(type==0)
			nowaliniaid_more.appendChild(nowaliniaid_morea);
		nowalinia.className="hafaslinia";
		nowalinia_kolorek.className="kolorek";
		var col=getpalette(i, this.bazaJSON.length);
		console.log(col);
		nowalinia_kolorek.style.backgroundColor=col;
		nowalinia.setAttribute("paletteid", getpaletteid(i, podstawa.length));
		heart.appendChild(nowalinia);
		nowalinia.appendChild(nowalinia_kolorek);
		var tablica1 = [];
		var n = this.bazaJSON[i].route.length;
		for(var j=0; j < n; j++)
		{
			var tmp=ol.proj.transform([podstawa[i].route[j].lon, podstawa[i].route[j].lat], 'EPSG:4326', 'EPSG:3857');
			this.updateMinimal(podstawa[i].route[j].lon, podstawa[i].route[j].lat);
			tablica1[tablica1.length] = tmp;
			if(podstawa[i].route[j].type=="stop")
			{
				liczbaPrzystankow++;
				if(this.przystanki[podstawa[i].route[j].id]==undefined)
				{
					this.przystanki[podstawa[i].route[j].id] = new PrzystanekNaMapie (podstawa[i].route[j].name, podstawa[i].route[j].id, tmp, this.warstwaVector1);
				}
				var tim0=podstawa[i].route[j].time;
				this.przystanki[podstawa[i].route[j].id].addLine(tim0, podstawa[i].line, getpalette(i, podstawa.length));
				var nowystop=document.createElement("DIV");
				var nowystopname=document.createElement("DIV");
				nowystopname.className="nowystopname";
				var nowystoptime=document.createElement("DIV");
				nowystoptime.className="nowystoptime";
				nowystoptime.innerHTML=stringTime(tim0);
				nowaliniaid_number.fooTime=tim0;
				nowaliniaid_number.fooId=podstawa[i].route[j].id;
				nowystopname.innerHTML=podstawa[i].route[j].name;
				nowystop.friend=this.przystanki[podstawa[i].route[j].id];
				nowystop.friend2=this;
				nowystop.onmouseover=function(e)
				{
					this.friend.setExtraStyle();
				};
				nowystop.onclick=function(e)
				{
					changesource2(this.friend.id, this.friend.minTime);
				};
				nowystop.onmouseout=function(e)
				{
					this.friend.setBasicStyle();
				};
				nowystop.appendChild(nowystopname);
				nowystop.appendChild(nowystoptime);
				nowystop.className="hafasstop";
				if(j==0 || j==n-1)
					nowalinia.appendChild(nowystop);
				else
					nowalinia_srodkowe.appendChild(nowystop);
				if(j==0) //TODO STOP_COUNT
				{
					nowalinia.appendChild(nowaliniaid_more);
					nowalinia.appendChild(nowalinia_srodkowe);
				}
			}
		}
		if(liczbaPrzystankow<2)
			nowaliniaid_more.style.display="none";
		addLineToSource(tablica1, getpalette(i, this.bazaJSON.length), podstawa[i].line, this.warstwaVector1, this.warstwaVector2);
		for (var item in this.przystanki)
		{
			this.przystanki[item].setBasicStyle();
		}
		/*var fea_tmp3 = new ol.Feature({geometry: new ol.geom.MultiPoint(tablica2)});
		fea_tmp3.setStyle(
		new ol.style.Style({
	    image: new ol.style.Circle({
	      fill: new ol.style.Fill({
		color: getpalette(i, podstawa.length)
	      }),
	      radius: 8,
	      stroke: new ol.style.Stroke({
		color: '#fff',
		width: 2
	      })
	    
	    })
	  }));
		this.warstwaVector1.addFeature(fea_tmp3);
	*/
	}
	var cenLon=(this.minLon+this.maxLon)/2;
	var cenLat=(this.minLat+this.maxLat)/2;
	var newCenter=ol.proj.transform([cenLat, cenLon], 'EPSG:4326', 'EPSG:3857');
 var pan = ol.animation.pan({
	     duration: 500,
	         source: /** @type {ol.Coordinate} */ (map.getView().getCenter())
		   });
  var pan2 = ol.animation.zoom({
	     duration: 500,
	         resolution: /** @type {ol.Coordinate} */ (map.getView().getResolution())
		   });
 
   map.beforeRender(pan);
   map.beforeRender(pan2);
   var extent = this.warstwaVector1.getExtent();
   map.getView().fitExtent(extent, [map.getSize()[0]-150, map.getSize()[1]-30]);
};
WizualizacjaPodrozy.prototype.updateMinimal = function(lat, lon)
{
	this.minLat=Math.min(this.minLat, lat);
	this.maxLat=Math.max(this.maxLat, lat);
	this.minLon=Math.min(this.minLon, lon);
	this.maxLon=Math.max(this.maxLon, lon);
}
var WizualizacjaDrzewa = function (from, time, linia, div, type)
{
	this.type=type;
	div.innerHTML="";
	this.from = from;
	this.time = time;
	this.linia = linia;
	this.div = div;
	if(type==1)
		this.tabela = loadJSON(kursSciezka(from, linia, time));
	else
		this.tabela = loadJSON(polaczeniaSciezka(from, 2, time));
	this.tabelaDIV = [];
	this.ladujDIV(0, this.tabela.length);
	if(type==0)
	{
		this.more1 = document.createElement("DIV");
		this.more1.implicit = this;
		this.more1.innerHTML="MORE...";
		this.div.appendChild(this.more1);
		this.more1.onclick=function(e)
		{
			this.implicit.rozszerzUp();
		};
	}
	for(var i=0; i<this.tabelaDIV.length; i++)
	{
		this.div.appendChild(this.tabelaDIV[i]);
	}
	if(type==0)
	{
		this.more2 = document.createElement("DIV");
		this.more2.implicit = this;
		this.more2.innerHTML="MORE...";
		this.div.appendChild(this.more2);
		this.more2.onclick=function(e)
		{
			this.implicit.rozszerzDown();
		};
	}
}
WizualizacjaDrzewa.prototype.rozszerzUp = function ()
{
	var tabela2 = loadJSON(polaczeniaSciezka(this.from, 1, this.tabela[0].time));
	var s1 = tabela2.length;
	var tabela3 = [];
	for(var i=0; i<tabela2.length; i++)
	{
		tabela3[i]=null;
	}
	for(var i=0; i<this.tabela.length; i++)
	{
		tabela2[tabela2.length]=this.tabela[i];
		tabela3[tabela3.length]=this.tabelaDIV[i];
	}
	this.tabela=tabela2;
	this.tabelaDIV=tabela3;
	this.ladujDIV(0, s1);
	this.div.innerHTML="";
	this.div.appendChild(this.more1);
	for(var i=0; i<this.tabelaDIV.length; i++)
	{
		this.div.appendChild(this.tabelaDIV[i]);
	}
	this.div.appendChild(this.more2);
}
WizualizacjaDrzewa.prototype.rozszerzDown = function ()
{
	var tabela2 = loadJSON(polaczeniaSciezka(this.from, 0, this.tabela[this.tabela.length-1].time));
	for(var i=0; i<tabela2.length; i++)
	{
		this.tabela[this.tabela.length]=tabela2[i];
	}
	this.ladujDIV(this.tabela.length-tabela2.length, this.tabela.length);
	this.div.innerHTML="";
	this.div.appendChild(this.more1);
	for(var i=0; i<this.tabelaDIV.length; i++)
	{
		this.div.appendChild(this.tabelaDIV[i]);
	}
	this.div.appendChild(this.more2);
}
WizualizacjaDrzewa.prototype.ladujDIV = function (from, to)
{
	for(var i=from; i<to; i++)
	{
		var nowy_postoj = document.createElement("DIV");
		var nowy_postoj_linia = document.createElement("div");
		var nowy_postoj_godzina = document.createElement("div");
		var nowy_postoj_link = document.createElement("a");
		nowy_postoj.className="nowy_postoj";
		if(i<this.center)
		{
			nowy_postoj.className="nowy_postoj malelitery";
		}
		nowy_postoj_linia.className="nowystopname";
		nowy_postoj_godzina.className="nowystoptime";
		nowy_postoj_link.className="nowy_postoj_link";
		nowy_postoj.appendChild(nowy_postoj_linia);
		nowy_postoj.appendChild(nowy_postoj_godzina);
		nowy_postoj_linia.appendChild(nowy_postoj_link);
		nowy_postoj.index=i;
		this.tabelaDIV[i]=nowy_postoj;
		nowy_postoj.implicit=this;
		nowy_postoj_link.implicit=nowy_postoj;
		var wpisz=nowy_postoj_link;
		if(this.type==0)
			wpisz.innerHTML=this.tabela[i].linia+" ("+this.tabela[i].idname+")";
		else
			wpisz.innerHTML=this.tabela[i].name;
		nowy_postoj_godzina.innerHTML=stringTime(this.tabela[i].time);
		nowy_postoj_link.onclick=function(e)
		{
			this.implicit.rozwin();
		}
		nowy_postoj.rozwin=function()
		{
			var id=this.implicit.from;
			var teraz=this.implicit.tabela[this.index];
			var extra = new WizualizacjaPodrozy(teraz.id, teraz.linia, teraz.time, 1);
		}
	}
}

/*
var style = {
  'MultiPoint': [new ol.style.Style({
    image: new ol.style.Circle({
      fill: new ol.style.Fill({
        color: 'rgba(255,255,0,255)',
      radius: 12,
      stroke: new ol.style.Stroke({
        color: '#ff0',
        width: 1
      })
    })
  })],
  'MultiLineString': [new ol.style.Style({
    stroke: new ol.style.Stroke({
      color: '#f0f',
      width: 12
    })
  })],
  'LineString': [new ol.style.Style({
    stroke: new ol.style.Stroke({
      color: '#fff',
      width: 5
    })
  })]
};
var style2 = {
  'MultiPoint': [new ol.style.Style({
    image: new ol.style.Circle({
      fill: new ol.style.Fill({
        color: 'rgba(255,255,0,255)'
      }),
      radius: 12,
      stroke: new ol.style.Stroke({
        color: '#ff0',
        width: 1
      })
    })
  })],
  'MultiLineString': [new ol.style.Style({
    stroke: new ol.style.Stroke({
      color: '#000',
      width: 1
    })
  })],
  'LineString': [new ol.style.Style({
    stroke: new ol.style.Stroke({
      color: '#f07',
      width: 2
    })
  })]
};
*/
function loadJSON(url) {
var resp ;
var xmlHttp ;

resp  = '' ;
xmlHttp = new XMLHttpRequest();

if(xmlHttp != null)
{
xmlHttp.open( "GET", url, false );
xmlHttp.send( null );
resp = xmlHttp.responseText;
}
       return JSON.parse(resp); 
}

var tabsource = null;

var basicVector = new ol.source.Vector({});
var projection = ol.proj.get('EPSG:3857');
var layerLines = new ol.layer.Vector({
source: basicVector,
    style: function(feature, resolution) {
    return style[feature.getGeometry().getType()];
  }
});
var layerLines2 = new ol.layer.Vector({
  source: basicVector,
    style: function(feature, resolution) {
    return style2[feature.getGeometry().getType()];
  }
});
      var map = new ol.Map({
        target: 'map',
        layers: [
          new ol.layer.Tile({
            source: new ol.source.OSM({url: " http://a.tiles.wmflabs.org/hikebike/{z}/{x}/{y}.png", crossOrigin: null})
          }), layerLines, layerLines2],
        view: new ol.View({
          center: ol.proj.transform([21.05, 52.23], 'EPSG:4326', 'EPSG:3857'),
          zoom: 12
        })});

    	document.getElementById("from").value="701301";
    	document.getElementById("to").value="122201";
	var today = new Date();
    	document.getElementById("time").value=parseInt((today.getTime()/1000), 10).toString();
	var aktfeature = null;
    	var mapcnt=document.getElementById("map");
	
mapcnt.onmousemove = function(e)
{
	var pos   = {X : mapcnt.offsetLeft, Y : mapcnt.offsetTop};
	var mPos  = {X : e.clientX - pos.X, Y : e.clientY - pos.Y};
	var newfeature = null;
	map.forEachFeatureAtPixel([mPos.X, mPos.Y], function(feature, layer){
	if(feature.getGeometry().getType()=='Point')
	{
		newfeature=feature.getId();
	}
	});
	if(newfeature!=aktfeature)
	{
		if(newfeature!=null)
		{
			tabsource.przystanki[newfeature].setExtraStyle();
		}
		if(aktfeature!=null)
		{
			tabsource.przystanki[aktfeature].setBasicStyle();
		}
		aktfeature=newfeature;
	}
}

function changesource()
{
	tribe=0;
    	var from = document.getElementById("from").value;
    	var to = document.getElementById("to").value;
    	var time = document.getElementById("time").value;
	tabsource = new WizualizacjaPodrozy(from, to, time, 0);
}

function changesource2(from, time)
{
    	var heart = document.getElementById("heart");
	var visu = new WizualizacjaDrzewa(from, time, null, heart, 0);
}

function zrownaj()
{
    	var heart = document.getElementById("heart");
    	var map0 = document.getElementById("map0");
	heart.style.maxHeight=(window.innerHeight-110)+"px";
}
zrownaj();
map.on('singleclick', function(evt){
	var extraCoord=ol.proj.transform(evt.coordinate, 'EPSG:3857', 'EPSG:4326');
	    console.log(evt);
	    console.log(evt.coordinate[0]);
	    console.log(evt.coordinate[1]);
	if(tribe==0)
	{
    		document.getElementById("from").value=extraCoord[0].toString()+"x"+extraCoord[1].toString();
	}
	if(tribe==1)
	{
    		document.getElementById("to").value=extraCoord[0].toString()+"x"+extraCoord[1].toString();
	}
	tribe+=1;
	if(tribe==2)
	{
		changesource();
		tribe=0;
	}
})
function showPosition(position) {
	 document.getElementById("from").value = position.coords.longitude + "x" + position.coords.latitude;
	 tribe=1;
}

if (navigator.geolocation) {
	        navigator.geolocation.getCurrentPosition(showPosition);
		    }
window.addEventListener("resize", zrownaj, false);
