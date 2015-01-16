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
	var sciezka="http://192.168.1.110/hafas";
	sciezka+='\?from\='+from;
	sciezka+='\&to\='+to;
	sciezka+='\&time\='+time;
	return sciezka;
}	
var WizualizacjaPodrozy = function (from, to, time)
{
	this.bazaJSON = loadJSON(podrozSciezka(from, to, time));
	this.heart=document.getElementById("heart");
	heart.innerHTML="";
	this.warstwaVector1 = new ol.source.Vector({});
	this.warstwaVector2 = new ol.source.Vector({});
	var podstawa=loadJSON(sciezka);
	for(var i=0; i < podstawa.length; i++)
	{
		var nowalinia=document.createElement("DIV");
		var nowaliniaid=document.createElement("DIV");
		nowaliniaid.innerHTML=podstawa[i].line;
		nowaliniaid.className="hafasliniaid";
		nowalinia.appendChild(nowaliniaid);
		nowalinia.className="hafaslinia";
		nowalinia.setAttribute("paletteid", getpaletteid(i, podstawa.length));
		heart.appendChild(nowalinia);
		var tablica1 = [];
		var tablica2 = [];
		for(var j=0; j < podstawa[i].route.length; j++)
		{
			var tmp=ol.proj.transform([podstawa[i].route[j].lon, podstawa[i].route[j].lat], 'EPSG:4326', 'EPSG:3857');
			tablica1[tablica1.length] = tmp;
			if(podstawa[i].route[j].type=="stop")
			{
				tablica2[tablica2.length] = tmp;
				var nowystop=document.createElement("DIV");
				var nowystopname=document.createElement("DIV");
				var nowystoptime=document.createElement("DIV");
				nowystopname.innerHTML=podstawa[i].route[j].name;
				var tim=podstawa[i].route[j].time;
				tim/=60;
				var mins=tim%60;
				var hours=Math.floor(tim/60);
				nowystoptime.innerHTML=hours+":"+mins;
				nowystop.appendChild(nowystopname);
				nowystop.appendChild(nowystoptime);
				nowystop.className="hafasstop";
				nowalinia.appendChild(nowystop);
			}
		}
		var fea_tmp1 = new ol.Feature({geometry: new ol.geom.LineString(tablica1)});
		fea_tmp1.setStyle(new ol.style.Style({
		stroke: new ol.style.Stroke({
		color: getpalette(i, podstawa.length),
		width: 4
		}), text: new ol.style.Text({text: podstawa[i].line,  font: '18px helvetica,sans-serif,bold', fill: new ol.style.Fill({
			                            color: getpalette(i, podstawa.length)
						                            }),
									                            stroke: new ol.style.Stroke({
													                                color: '#fff',
																	                            width: 6
																				                            })})
	}));
		var fea_tmp2 = new ol.Feature({name: 'My Polygon', geometry: new ol.geom.LineString(tablica1)});
		fea_tmp2.setStyle(new ol.style.Style({
		stroke: new ol.style.Stroke({
		color: '#fff',
		width: 7
		})}));
		var fea_tmp3 = new ol.Feature({geometry: new ol.geom.MultiPoint(tablica2)});
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
		bv1.addFeature(fea_tmp1);
		bv1.addFeature(fea_tmp3);
		bv2.addFeature(fea_tmp2);
	}
	return [bv1, bv2];
}

}

var palette = ['#ab0d15'
,'#F7931D'
,'#FFF200'
,'#0DB14B'
,'#0071BC'
,'#00B9F1'
,'#903F98'
,'#EE609D'];
function getpalette(poz, n)
{
	console.log(Math.floor(poz*8/n));
	return palette[Math.floor(poz*8/n)];
}
function getpaletteid(poz, n)
{
	return Math.floor(poz*8/n);
}
var style = {
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
function loadHafas(from, to, time)
{
	var heart=document.getElementById("heart");
	heart.innerHTML="";
	var bv1 = new ol.source.Vector({});
	var bv2 = new ol.source.Vector({});
	var sciezka="http://192.168.1.110/hafas";
	sciezka+='\?from\='+from;
	sciezka+='\&to\='+to;
	sciezka+='\&time\='+time;
	var podstawa=loadJSON(sciezka);
	for(var i=0; i < podstawa.length; i++)
	{
		var nowalinia=document.createElement("DIV");
		var nowaliniaid=document.createElement("DIV");
		nowaliniaid.innerHTML=podstawa[i].line;
		nowaliniaid.className="hafasliniaid";
		nowalinia.appendChild(nowaliniaid);
		nowalinia.className="hafaslinia";
		nowalinia.setAttribute("paletteid", getpaletteid(i, podstawa.length));
		heart.appendChild(nowalinia);
		var tablica1 = [];
		var tablica2 = [];
		for(var j=0; j < podstawa[i].route.length; j++)
		{
			var tmp=ol.proj.transform([podstawa[i].route[j].lon, podstawa[i].route[j].lat], 'EPSG:4326', 'EPSG:3857');
			tablica1[tablica1.length] = tmp;
			if(podstawa[i].route[j].type=="stop")
			{
				tablica2[tablica2.length] = tmp;
				var nowystop=document.createElement("DIV");
				var nowystopname=document.createElement("DIV");
				var nowystoptime=document.createElement("DIV");
				nowystopname.innerHTML=podstawa[i].route[j].name;
				var tim=podstawa[i].route[j].time;
				tim/=60;
				var mins=tim%60;
				var hours=Math.floor(tim/60);
				nowystoptime.innerHTML=hours+":"+mins;
				nowystop.appendChild(nowystopname);
				nowystop.appendChild(nowystoptime);
				nowystop.className="hafasstop";
				nowalinia.appendChild(nowystop);
			}
		}
		var fea_tmp1 = new ol.Feature({geometry: new ol.geom.LineString(tablica1)});
		fea_tmp1.setStyle(new ol.style.Style({
		stroke: new ol.style.Stroke({
		color: getpalette(i, podstawa.length),
		width: 4
		}), text: new ol.style.Text({text: podstawa[i].line,  font: '18px helvetica,sans-serif,bold', fill: new ol.style.Fill({
			                            color: getpalette(i, podstawa.length)
						                            }),
									                            stroke: new ol.style.Stroke({
													                                color: '#fff',
																	                            width: 6
																				                            })})
	}));
		var fea_tmp2 = new ol.Feature({name: 'My Polygon', geometry: new ol.geom.LineString(tablica1)});
		fea_tmp2.setStyle(new ol.style.Style({
		stroke: new ol.style.Stroke({
		color: '#fff',
		width: 7
		})}));
		var fea_tmp3 = new ol.Feature({geometry: new ol.geom.MultiPoint(tablica2)});
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
		bv1.addFeature(fea_tmp1);
		bv1.addFeature(fea_tmp3);
		bv2.addFeature(fea_tmp2);
	}
	return [bv1, bv2];
}
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
            source: new ol.source.OSM({url: "http://a.tiles.wmflabs.org/bw-mapnik/{z}/{x}/{y}.png", crossOrigin: null})
          }), layerLines, layerLines2],
        view: new ol.View({
          center: ol.proj.transform([37.41, 8.82], 'EPSG:4326', 'EPSG:3857'),
          zoom: 4
        })});

    	document.getElementById("from").value="701301";
    	document.getElementById("to").value="122201";
    	document.getElementById("time").value="36000";
	var aktfeature = null;
    	var mapcnt=document.getElementById("map");
	mapcnt.onmousemove=function(e)
	{
		 var pos   = {X:mapcnt.offsetLeft, Y:mapcnt.offsetTop};
		 var mPos  = { X:e.clientX-pos.X, Y:e.clientY-pos.Y };
		 var newfeature = null;
		 var feature = map.forEachFeatureAtPixel([mPos.X, mPos.Y], function(feature, layer){
			 if(feature.getGeometry().getType()=='MultiPoint')
			 {
				 newfeature=feature;
			 }
	});
	if(newfeature!=aktfeature)
	{
		if(newfeature!=null)
		{
newfeature.setStyle(
			    new ol.style.Style({
    image: new ol.style.Circle({
      fill: new ol.style.Fill({
        color: '#f00'
      }),
      radius: 8,
      stroke: new ol.style.Stroke({
        color: '#fff',
        width: 2
      })
			       })
    }));
		}
		if(aktfeature!=null)
		{
aktfeature.setStyle(
			    new ol.style.Style({
    image: new ol.style.Circle({
      fill: new ol.style.Fill({
        color: '#00f'
      }),
      radius: 8,
      stroke: new ol.style.Stroke({
        color: '#fff',
        width: 2
      })
			       })
    }));
	}
	aktfeature=newfeature;
	}
	}
function changesource()
{
    	var from = document.getElementById("from").value;
    	var to = document.getElementById("to").value;
    	var time = document.getElementById("time").value;
	map.removeLayer(layerLines2);
	map.removeLayer(layerLines);
	var tabsource = loadHafas(from, to, time);
	layerLines = new ol.layer.Vector({
	source: tabsource[1],
	style: function(feature, resolution)
	{
		return style[feature.getGeometry().getType()];
	}
	});
	layerLines2 = new ol.layer.Vector({
	source: tabsource[0],
	style: function(feature, resolution) {
	return style2[feature.getGeometry().getType()];
	}
	});
 	map.addLayer(layerLines);
        map.addLayer(layerLines2);
}
