var WizualizacjaPodrozy = function (from, toOrLine, time, type)
{
	document.getElementById("overlay").style.display="block";
	if(type==0)
	{
		this.bazaJSON = loadJSON(podrozSciezka(from, toOrLine, time));
	}
	else
	{
		this.bazaJSON = loadJSON(kursSciezka(from, toOrLine, time));
	}
	document.getElementById("overlay").style.display="none";
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
	console.log("TOTUTUTUT");
	
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
//   document.getElementById("overlay").style.display="none";
};
WizualizacjaPodrozy.prototype.updateMinimal = function(lat, lon)
{
	this.minLat=Math.min(this.minLat, lat);
	this.maxLat=Math.max(this.maxLat, lat);
	this.minLon=Math.min(this.minLon, lon);
	this.maxLon=Math.max(this.maxLon, lon);
}

