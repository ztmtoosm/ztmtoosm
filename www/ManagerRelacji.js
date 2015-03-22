var ManagerRelacji = function ()
{
	this.znaczek = null;
	this.znaczekWsp = [];
	var these = this;
	//new Relacja(this);
	this.basicVector = new ol.source.Vector({});
	this.basicZnaczniki = new ol.source.Vector({});
	this.projection = ol.proj.get('EPSG:3857');
	this.layerLines = new ol.layer.Vector({
	source: these.basicVector,
	});
	this.layerLines2 = new ol.layer.Vector({
	  source: these.basicVector,
	});
	this.layerLines3 = new ol.layer.Vector({
	  source: these.basicZnaczniki,
	});
	this.map = new ol.Map({
        target: 'map',
        layers: [
          new ol.layer.Tile({
            source: new ol.source.OSM({url: " http://a.tiles.wmflabs.org/hikebike/{z}/{x}/{y}.png", crossOrigin: null})
          }), these.layerLines, these.layerLines2, these.layerLines3],
        view: new ol.View({
          center: ol.proj.transform([21.05, 52.23], 'EPSG:4326', 'EPSG:3857'),
          zoom: 12
	})});
	this.mapcnt = document.getElementById("map");
	console.log(this.mapcnt);
	this.mapcnt.sterownik = this;
	this.mapcnt.onclick = function(e)
	{
		var that = this;
		var pos   = {X : that.offsetLeft, Y : that.offsetTop};
		var mPos  = {X : e.clientX - pos.X, Y : e.clientY - pos.Y};
		var newfeature = null;
		var block = false;
		this.sterownik.map.forEachFeatureAtPixel([mPos.X, mPos.Y], function(feature, layer){
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
			var wsp = ol.proj.transform(this.sterownik.map.getCoordinateFromPixel([mPos.X, mPos.Y]), 'EPSG:3857', 'EPSG:4326');
			if(this.sterownik.onClickLine!=undefined)
				this.sterownik.onClickLine(newfeature, wsp, this.sterownik.lineHandler);
		}
	}
	$("#map").on("mouseup", function(){if(this.sterownik.znaczek!=null){this.sterownik.zmienionyZnacznik(this.sterownik.znaczek, this.sterownik.znaczekWsp, this.sterownik.znacznikHandler);}this.sterownik.znaczek = null;});
	$("#map").on("mousedown", function(){this.sterownik.znaczek=null});

	document.managerRelacji = this;
	$(document).ready(function() {
	document.getElementById("nowy").manager = document.managerRelacji;
	document.getElementById("map").manager = document.managerRelacji;
	$('#map').on('contextmenu', '', function() {
	var pos   = {X : this.offsetLeft, Y : this.offsetTop};
	var mPos  = {X : event.clientX - pos.X, Y : event.clientY - pos.Y};
	var pix=[mPos.X, mPos.Y];
	this.wsp=ol.proj.transform(this.manager.map.getCoordinateFromPixel(pix), 'EPSG:3857', 'EPSG:4326');
	console.log(this.wsp);
	console.log("xDDDD");
	document.getElementById("nowy").wsp = this.wsp;
	document.getElementById("nowy").onclick = function()
	{
		this.manager.ppmClick(this.wsp);
	};
	document.getElementById("rmenu").className = "show";  
	document.getElementById("rmenu").style.top =  mouseY(event) + "px";
	document.getElementById("rmenu").style.left = mouseX(event) + "px";

	window.event.returnValue = false;


	});

	$(document).bind("click", function(event) {
	document.getElementById("rmenu").className = "hide";
});
});



}

ManagerRelacji.prototype.ppmClick = function (wspolrzedne)
{
	if(this.onPpm != undefined)
		this.onPpm(wspolrzedne, this.ppmHandler);
}

ManagerRelacji.prototype.nowyZnacznikFizycznie = function (wspolrzedne, id)
{
	var loc = ol.proj.transform(wspolrzedne, 'EPSG:4326', 'EPSG:3857');
	var iconFeature = new ol.Feature({
		geometry: new ol.geom.Point(loc)
	});
	var warstwaStrokeText1 = new ol.style.Stroke ({color: '#fff', width: 4});
	var warstwaFont1 = '18px helvetica,sans-serif,bold';
	var warstwaFill1 = new ol.style.Fill({color: '#00f'});
	var txt = new ol.style.Text({offsetY: -18, text: id, font: warstwaFont1, fill: warstwaFill1, stroke: warstwaStrokeText1});
	var style = new ol.style.Style({
		   image: new ol.style.Circle({
	      fill: new ol.style.Fill({
		color: '#00f'
	      }),
	      radius: 10,
	      stroke: new ol.style.Stroke({
		color: '#fff',
		width: 4
	      })}),
		text: txt
	});
	iconFeature.setStyle(style);
	this.basicZnaczniki.addFeature(iconFeature);
	var modifyInteraction = makeMovable(iconFeature, function(wsp){});
	this.map.addInteraction(modifyInteraction);
	iconFeature.sterownik = this;
	iconFeature.id = id;
	iconFeature.interac=modifyInteraction;
	return iconFeature;
}

ManagerRelacji.prototype.nowaLiniaFizycznie = function (preCoordinates, id)
{
	var coordinates = [];
	for(var i=0; i<preCoordinates.length; i++)
	{
		coordinates[i] = ol.proj.transform([preCoordinates[i].x, preCoordinates[i].y], 'EPSG:4326', 'EPSG:3857');
	}
	var colorLine = '#00f';
	var warstwa1 = new ol.Feature({geometry: new ol.geom.LineString(coordinates)});
	var warstwaStroke1 = new ol.style.Stroke ({color: colorLine, width: 4});
	var warstwaStyle1 =new ol.style.Style({stroke : warstwaStroke1});
	warstwa1.setStyle(warstwaStyle1);
	var warstwa2 = new ol.Feature({geometry: new ol.geom.LineString(coordinates)});
	var warstwaStroke2 = new ol.style.Stroke ({color: '#fff', width: 7});
	var warstwaStyle2 = new ol.style.Style({stroke : warstwaStroke2});
	warstwa2.setStyle(warstwaStyle2);
	warstwa1.id = id;
	warstwa2.id = id;
	console.log(warstwa1);
	this.basicZnaczniki.addFeature(warstwa1);
	this.basicVector.addFeature(warstwa2);
	return [warstwa1, warstwa2];
}

ManagerRelacji.prototype.znacznikZmienId = function (znacznik, id)
{
	znacznik.id = id;
	var warstwaStrokeText1 = new ol.style.Stroke ({color: '#fff', width: 4});
	var warstwaFont1 = '18px helvetica,sans-serif,bold';
	var warstwaFill1 = new ol.style.Fill({color: '#00f'});
	var txt = new ol.style.Text({offsetY: -18, text: id, font: warstwaFont1, fill: warstwaFill1, stroke: warstwaStrokeText1});
	
	var style = new ol.style.Style({
		   image: new ol.style.Circle({
	      fill: new ol.style.Fill({
		color: '#00f'
	      }),
	      radius: 10,
	      stroke: new ol.style.Stroke({
		color: '#fff',
		width: 4
	      })}),
		text: txt
	});
	znacznik.setStyle(style);
}

ManagerRelacji.prototype.liniaZmienId = function (paraLinii, id)
{
	paraLinii[0].id=id;
	paraLinii[1].id=id;
}

ManagerRelacji.prototype.usunLinie = function (paraLinii)
{
	this.basicZnaczniki.removeFeature(paraLinii[0]);
	this.basicVector.removeFeature(paraLinii[1]);
}

ManagerRelacji.prototype.usunPunkt = function (punkt)
{
	this.basicZnaczniki.removeFeature(punkt);
	this.map.removeInteraction(punkt.interac);
}



