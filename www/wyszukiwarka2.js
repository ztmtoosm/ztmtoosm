function makeMovable(feature, moveMove) 
{
	var modify = new ol.interaction.Modify({
		features: new ol.Collection([feature])
	});

  feature.on('change',function(ev) {
	  znaczek=this;
	//	    this.sterownik.onMovePointer(ol.proj.transform(this.getGeometry().getCoordinates(), 'EPSG:3857', 'EPSG:4326'));
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
	iconFeature.interac=modifyInteraction;
	return iconFeature;
}
/*function usunZnacznik(znacznik)
{
}
function usunZnacznik(znacznik)
{
	map.removeInteraction(znacznik.interac);
	basicZnaczniki.removeFeature(znacznik);
}*/




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
function punktSciezka (y, x)
{
	var sciezka="/dijkstra";
	sciezka+='\?y\='+y;
	sciezka+='\&x\='+x;
	console.log(sciezka);
	return sciezka;
}
function punktySciezka (from, to)
{
	var sciezka="/dijkstra";
	sciezka+='\?from\='+from;
	sciezka+='\&to\='+to;
	return sciezka;
}
/*
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
var layerLines3 = new ol.layer.Vector({
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
          }), layerLines, layerLines2, layerLines3],
        view: new ol.View({
          center: ol.proj.transform([21.05, 52.23], 'EPSG:4326', 'EPSG:3857'),
          zoom: 12
	})});
//document.body.style.cursor = 'url("marker-icon.png"), auto';

var aktfeature = null;
var mapcnt=document.getElementById("map");


function addLineToSource (coordinates, id, colorLine, name, vectorSource1, vectorSource2)
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
	warstwa1.id = id;
	warstwa2.id = id;
	vectorSource1.addFeature(warstwa1);
	vectorSource2.addFeature(warstwa2);
	return [warstwa1, warstwa2];
}


mapcnt.onclick = function(e)
{
	var pos   = {X : mapcnt.offsetLeft, Y : mapcnt.offsetTop};
	var mPos  = {X : e.clientX - pos.X, Y : e.clientY - pos.Y};
	var newfeature = null;
	var block = false;
	map.forEachFeatureAtPixel([mPos.X, mPos.Y], function(feature, layer){
	console.log(feature.getGeometry().getType());
	if(feature.getGeometry().getType()=='LineString')
	{
		newfeature=feature;
	}
	if(feature.getGeometry().getType()=='Point')
	{
		block=true;
	}
	});
	if(block)
		newfeature = null;
	if(newfeature !=null)
	{
		var wsp = ol.proj.transform(map.getCoordinateFromPixel([mPos.X, mPos.Y]), 'EPSG:3857', 'EPSG:4326');
		znacznikWPolowie(newfeature.id, wsp);
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

/*map.on('click', function(evt){
console.log("dupa");
});*/

/*
if (navigator.geolocation) {
	        navigator.geolocation.getCurrentPosition(showPosition);
		    }
*/

window.addEventListener("resize", zrownaj, false);


var tablicaZnacznikow = [];
var tablicaLacznikow1 = [];
var tablicaLacznikow2 = [];

function dodajLacznik(from, to, col)
{
	var lacznik = loadJSON(punktySciezka(from.osmid, to.osmid));
	var tablica1 = [];
	console.log(lacznik);
	//tablicaLacznikow2[from.id]=lacznik;
	for(var i=0; i<lacznik.length; i++)
	{
		var tmp =ol.proj.transform([lacznik[i].x, lacznik[i].y], 'EPSG:4326', 'EPSG:3857');
		tablica1[tablica1.length]=tmp;
	}
	var l2=addLineToSource (tablica1, from.id, col, "xxx", basicZnaczniki, basicVector);
	return [lacznik, l2];
}

function nowyZnacznikNaKoncu(wsp)
{
	var zzz=loadJSON(punktSciezka(wsp[1], wsp[0]));
	console.log(zzz);
	var zn = nowyZnacznik([zzz.x, zzz.y], 'green.png', null);
	zn.id=tablicaZnacznikow.length;
	zn.osmid=zzz.id;
	if(tablicaZnacznikow.length>0)
	{
		var nl=dodajLacznik(tablicaZnacznikow[tablicaZnacznikow.length-1], zn, "#00ff00");
		tablicaLacznikow1[tablicaLacznikow1.length]=nl[0];
		tablicaLacznikow2[tablicaLacznikow2.length]=nl[1];
	}
	tablicaZnacznikow[tablicaZnacznikow.length]=zn;
}

function znacznikWPolowie(id, wsp)
{
	console.log(ol.Sphere);
	var distance = 1000.0;
	var id_tmp = -1;
	for(var i=0; i<tablicaLacznikow1[id].length; i++)
	{
	             var line = new ol.geom.LineString([[tablicaLacznikow1[id][i].x, tablicaLacznikow1[id][i].y], wsp]);
		          var tmp_distance= line.getLength();
			  if(tmp_distance<distance)
			  {
				  distance = tmp_distance;
				  id_tmp = i;
			  }
	}
	if(id_tmp>0 && id_tmp<tablicaLacznikow1[id].length-1)
	{
		var zntmp = tablicaLacznikow1[id][id_tmp];
		for (var i=tablicaZnacznikow.length-1; i>id; i--)
		{
			tablicaZnacznikow[i+1]=tablicaZnacznikow[i];
			tablicaZnacznikow[i+1].id=i+1;
		}
		for (var i=tablicaLacznikow1.length-1; i>id; i--)
		{
			tablicaLacznikow1[i+1]=tablicaLacznikow1[i];
			tablicaLacznikow2[i+1]=tablicaLacznikow2[i];
			tablicaLacznikow2[i+1][0].id=i+1;
			tablicaLacznikow2[i+1][1].id=i+1;
		}
		basicZnaczniki.removeFeature(tablicaLacznikow2[id][0]);
		basicVector.removeFeature(tablicaLacznikow2[id][1]);
		tablicaZnacznikow[id+1] = nowyZnacznik([zntmp.x, zntmp.y], 'red.png', null);
		tablicaZnacznikow[id+1].id=id+1;
		tablicaZnacznikow[id+1].osmid=zntmp.id;
		var nl=dodajLacznik(tablicaZnacznikow[id], tablicaZnacznikow[id+1], "#00ff00");
		tablicaLacznikow1[id]=nl[0];
		tablicaLacznikow2[id]=nl[1];
		var nl2=dodajLacznik(tablicaZnacznikow[id+1], tablicaZnacznikow[id+2], "#00ff00");
		tablicaLacznikow1[id+1]=nl2[0];
		tablicaLacznikow2[id+1]=nl2[1];
	}
}

function zmienionyZnacznik(ref)
{
	var wsp=ol.proj.transform(ref.getGeometry().getCoordinates(), 'EPSG:3857', 'EPSG:4326');
	var zzz=loadJSON(punktSciezka(wsp[1], wsp[0]));
	var zn = nowyZnacznik([zzz.x, zzz.y], 'green.png', null);
	zn.id=ref.id;
	zn.osmid=zzz.id;
	var n = tablicaZnacznikow.length;
	map.removeInteraction(ref.interac);
	basicZnaczniki.removeFeature(ref);
	if(zn.id<n-1)
	{
		basicZnaczniki.removeFeature(tablicaLacznikow2[zn.id][0]);
		basicVector.removeFeature(tablicaLacznikow2[zn.id][1]);
		var nl=dodajLacznik(zn, tablicaZnacznikow[zn.id+1], "#00ff00");
		tablicaLacznikow1[zn.id]=nl[0];
		tablicaLacznikow2[zn.id]=nl[1];
	}
	if(zn.id>0)
	{
		basicZnaczniki.removeFeature(tablicaLacznikow2[zn.id-1][0]);
		basicVector.removeFeature(tablicaLacznikow2[zn.id-1][1]);
		var nl=dodajLacznik(tablicaZnacznikow[zn.id-1], zn, "#00ff00");
		tablicaLacznikow1[zn.id-1]=nl[0];
		tablicaLacznikow2[zn.id-1]=nl[1];
	}
	/*if(tablicaZnacznikow.length>0)
	{
		var nl=dodajLacznikNaKoncu(tablicaZnacznikow[tablicaZnacznikow.length-1], zn);
		tablicaLacznikow1[tablicaLacznikow1.length]=nl[0];
		tablicaLacznikow2[tablicaLacznikow2.length]=nl[1];
	}*/
	tablicaZnacznikow[zn.id]=zn;
}

$(document).ready(function() {

console.log("ready");
//if ($("#map0").addEventListener) {
//$("#map0").addEventListener('contextmenu', function(e) {
//al/ert("You've tried to open context menu"); //here you draw your own menu
//e.preventDefault();
//}, false);
//}
// else {
//document.getElementById("test").attachEvent('oncontextmenu', function() {
//$(".test").bind('contextmenu', function() {
$('#map0').on('contextmenu', '', function() {
console.log(map);
var pix=[mouseX(event), mouseY(event)];
this.wsp=ol.proj.transform(map.getCoordinateFromPixel(pix), 'EPSG:3857', 'EPSG:4326');
console.log("xDDDD");
document.getElementById("nowy").wsp = this.wsp;
document.getElementById("nowy").onclick = function()
{
	nowyZnacznikNaKoncu(this.wsp);
};
document.getElementById("rmenu").className = "show";  
document.getElementById("rmenu").style.top =  mouseY(event) + "px";
document.getElementById("rmenu").style.left = mouseX(event) + "px";

window.event.returnValue = false;


});
//}
});

// this is from another SO post...  
$(document).bind("click", function(event) {
document.getElementById("rmenu").className = "hide";
});



function mouseX(evt) {
if (evt.pageX) {
return evt.pageX;
} else if (evt.clientX) {
return evt.clientX + (document.documentElement.scrollLeft ?
document.documentElement.scrollLeft :
document.body.scrollLeft);
} else {
return null;
}
}

function mouseY(evt) {
if (evt.pageY) {
return evt.pageY;
} else if (evt.clientY) {
return evt.clientY + (document.documentElement.scrollTop ?
document.documentElement.scrollTop :
document.body.scrollTop);
} else {
return null;
}
}
var znaczek = null;
$("#map").on("mouseup", function(){if(znaczek!=null){zmienionyZnacznik(znaczek);}znaczek = null;});
