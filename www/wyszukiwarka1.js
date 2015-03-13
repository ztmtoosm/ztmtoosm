function makeMovable(feature, moveMove) 
{
	var modify = new ol.interaction.Modify({
		features: new ol.Collection([feature])
	});

  feature.on('change',function() {
	            this.sterownik.onMovePointer(ol.proj.transform(this.getGeometry().getCoordinates(), 'EPSG:3857', 'EPSG:4326'));
										    }, feature);
	return modify;
}

function nowyZnacznik(wspolrzedne, znak, sterownik)
{
	var moveMove = function(wsp){};
	var loc = ol.proj.transform(wspolrzedne, 'EPSG:4326', 'EPSG:3857');
	    var iconFeature = new ol.Feature({
		            geometry: new ol.geom.Point(loc)
			        });
	var style = new ol.style.Style({
	    image: new ol.style.Icon(/** @type {olx.style.IconOptions} */ ({
		            anchor: [0.25, 1],
			            anchorXUnits: 'fraction',
				            anchorYUnits: 'fraction',
					            src: znak,
						        }))
	});
	iconFeature.setStyle(style);
	basicZnaczniki.addFeature(iconFeature);
	console.log(moveMove);
	var modifyInteraction = makeMovable(iconFeature, moveMove);
	map.addInteraction(modifyInteraction);
	iconFeature.sterownik=sterownik;
	return iconFeature;
}

var WprowadzanieWspolrzednych = function(wpis, color)
{
	console.log(wpis);
	console.log("xD");
	this.color = color;
	this.znacznik = null;
	this.show = 0;
	this.wspolrzedne = [0,0];
	this.wpis = wpis;
	this.wpis.style.backgroundColor="white";
	wpis.onfocus = function(){console.log(this);this.value=""; this.sterownik.onClearSelector()};
	wpis.sterownik = this;
	console.log(wpis);
}

WprowadzanieWspolrzednych.prototype.onSelectPhrase= function(wspolrzedne)
{
	this.show = 2;
	this.wpis.style.backgroundColor="#99CCFF";
	this.wspolrzedne = wspolrzedne;
	this.znacznik = nowyZnacznik(this.wspolrzedne, this.color, this);
}

WprowadzanieWspolrzednych.prototype.onMovePointer = function(wspolrzedne)
{
	console.log(wspolrzedne);
	this.show = 1;
	this.wpis.style.backgroundColor="#99CCFF";
	this.wspolrzedne = wspolrzedne;
	console.log(this.wspolrzedne);
	var lol=wspolrzedne[0]+"x"+wspolrzedne[1];
	console.log(lol);
	this.wpis.value=lol;
}

WprowadzanieWspolrzednych.prototype.instantCoordinates = function()
{
	return this.wspolrzedne[0]+"x"+this.wspolrzedne[1];
}

WprowadzanieWspolrzednych.prototype.onClearSelector=function(wspolrzedne)
{
	this.wpis.style.backgroundColor="white";
	this.show = 0;
}

$(function() {
$( ".fromto" ).autocomplete({

source: function( request, response ) {
	var req = "Bus stop "+request.term;
	        $.ajax({
			url: "http://nominatim.openstreetmap.org/search",
					              data: {
							      q : req,
							      format: "json",
							      viewbox : "20.5225,51.9561,21.5250,52.4953",
							      bounded : "1"
									            },
										              success: function( data ) {
												      var data2 = [];
												      for(var i=0; i<data.length; i++)
												      {
													      console.log(data[i]);
													      data2[i]=
													      {label: data[i].display_name,
													      value: data[i].display_name,
													      bbox: data[i].boundingbox,
													      icon: data[i].icon};
												      }
												                  response( data2 );
														            }
															            });
		      },
	select: function( event, ui ) {
		var wspolrzedne = [parseFloat(ui.item.bbox[2]), parseFloat(ui.item.bbox[0])];
 		this.sterownik.onSelectPhrase(wspolrzedne);
}
}).data( "ui-autocomplete" )._renderItem = function( ul, item ) {
	console.log(item);
	        return $( "<li></li>" )
		            .data( "item.autocomplete", item )
			                .append( "<a>" + "<img src='" + item.icon + "' />" + item.label+ "</a>" )
					            .appendTo( ul );
						        };
});



/*
var tribe = 0;
function changeTribe(newTribe)
{
	tribe = newTribe;
}
*/
var palette = ['#ab0d15'
,'#F7931D'
/*,'#FFF200'*/
,'#0DB14B'
,'#0071BC'
,'#00B9F1'
,'#903F98'
,'#EE609D'];

var dni = [
"niedziela",
"poniedziałek",
"wtorek",
"środa",
"czwartek",
"piątek",
"sobota",
];

var miesiace = [
"stycznia",
"lutego",
"marca",
"kwietnia",
"maja",
"czerwca",
"lipca"];

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
var basicZnaczniki = new ol.source.Vector({});
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
var layerLinesZnaczniki = new ol.layer.Vector({
  source: basicZnaczniki,
    style: function(feature, resolution) {
    return style2[feature.getGeometry().getType()];
  }
});




      var map = new ol.Map({
        target: 'map',
        layers: [
          new ol.layer.Tile({
            source: new ol.source.OSM({url: " http://a.tiles.wmflabs.org/hikebike/{z}/{x}/{y}.png", crossOrigin: null})
          }), layerLines, layerLines2, layerLinesZnaczniki],
        view: new ol.View({
          center: ol.proj.transform([21.05, 52.23], 'EPSG:4326', 'EPSG:3857'),
          zoom: 12
	})});
//document.body.style.cursor = 'url("marker-icon.png"), auto';

//fett.style.display="none";
for(var i=0; i<24; i++)
{
	var option = document.createElement("option");
	option.text=i;
	option.value=i;
	var d=new Date();
	if(d.getHours()==i)
		option.selected="selected";
	document.getElementById("wyborGodziny").add(option);
}
for(var i=0; i<60; i++)
{
	var option = document.createElement("option");
	option.text=i;
	option.value=i;
	var d=new Date();
	if(d.getMinutes()==i)
		option.selected="selected";
	if(i<10)
	{
		option.text="0"+i;
	}
	document.getElementById("wyborMinuty").add(option);
}

function eleganckiCzas(czas)
{
	var wynik = "";
	wynik+=dni[czas.getDay()]+", ";
	wynik+=czas.getDate()+" ";
	wynik+=miesiace[czas.getMonth()]+" ";
	return wynik;
}


for(var i=0; i<3; i++)
{
	var d = new Date();
	var d_prim = new Date();
	d_prim.setTime(d.getTime()+i*24*60*60*1000);
	var option = document.createElement("option");
	option.text=eleganckiCzas(d_prim);
	option.value=i;
	document.getElementById("wyborDnia").add(option);
}

function getSettedTime()
{
	var d = new Date();
	var d1 = new Date();
	var wyborDnia=document.getElementById("wyborDnia");
	var wyborGodziny=document.getElementById("wyborGodziny");
	var wyborMinuty=document.getElementById("wyborMinuty");
	var wyb=parseInt(wyborDnia.options[wyborDnia.selectedIndex].value);
	d1.setTime(d.getTime()+wyb*24*60*60*1000);
	var wybMin=parseInt(wyborMinuty.options[wyborMinuty.selectedIndex].value);
	var wybGodz=parseInt(wyborGodziny.options[wyborGodziny.selectedIndex].value);
	
	
	var d2=new Date(d1.getFullYear(), d1.getMonth(), d1.getDate(), wybGodz, wybMin,0, 0);
	var wynik=d2.getTime();
	console.log(wynik);
	console.log(wybMin);
	console.log(wybGodz);
	return wynik;
}

var today = new Date();
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
    	var from = wpf.instantCoordinates();
    	var to = wpt.instantCoordinates();
	if(wpf.show==0 || wpt.show==0)
	{
		alert("Brak wprowadzonych danych");
	}
	else
	{
		var time = parseInt(getSettedTime()/1000);
		tabsource = new WizualizacjaPodrozy(from, to, time, 0);
	}
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

map.on('click', function(evt){
console.log("dupa");
});

/*
if (navigator.geolocation) {
	        navigator.geolocation.getCurrentPosition(showPosition);
		    }
*/

var wpf = new WprowadzanieWspolrzednych(document.getElementById("from"), "green.png");
var wpt = new WprowadzanieWspolrzednych(document.getElementById("to"), "red.png");
window.addEventListener("resize", zrownaj, false);
document.getElementById("overlay").style.display="none";
map.on('contextmenu', function(evt){
		var extraCoord=ol.proj.transform(evt.coordinate, 'EPSG:3857', 'EPSG:4326');
			    console.log(evt);
			    	    console.log(evt.coordinate[0]);
				    	    console.log(evt.coordinate[1]);});
