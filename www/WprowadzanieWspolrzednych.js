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
	iconFeature.interac=modifyInteraction;
	return iconFeature;
}
function usunZnacznik(znacznik)
{
	map.removeInteraction(znacznik.interac);
	basicZnaczniki.removeFeature(znacznik);
}

var WprowadzanieWspolrzednych = function(wpis, color, desc)
{
	console.log(wpis);
	console.log("xD");
	this.color = color;
	this.znacznik = null;
	this.show = 0;
	this.wspolrzedne = [0,0];
	this.wpis = wpis;
	this.wpis.blur();
	this.wpis.style.backgroundColor="white";
	wpis.style.color="#AAAAAA";
	wpis.value = desc;
	wpis.onfocus = function(){this.sterownik.onInput(); this.oldvalue=this.value;};
	wpis.sterownik = this;
	console.log(wpis);
}

WprowadzanieWspolrzednych.prototype.onSelectPhrase= function(wspolrzedne)
{
	this.wpis.blur();
	this.wpis.style.color="black";
	this.show = 2;
	this.wpis.style.backgroundColor="#99CCFF";
	this.wspolrzedne = wspolrzedne;
	this.znacznik = nowyZnacznik(this.wspolrzedne, this.color, this);
}

WprowadzanieWspolrzednych.prototype.onMovePointer = function(wspolrzedne)
{
	this.wpis.blur();
	this.wpis.style.color="black";
	console.log(wspolrzedne);
	this.wpis.style.backgroundColor="#99CCFF";
	this.wspolrzedne = wspolrzedne;
	if(this.show==0)
		this.znacznik = nowyZnacznik(this.wspolrzedne, this.color, this);
	this.show = 1;
	console.log(this.wspolrzedne);
	var lol=wspolrzedne[0]+"x"+wspolrzedne[1];
	console.log(lol);
	this.wpis.value=lol;
}

WprowadzanieWspolrzednych.prototype.onIndicatePointer = function(wspolrzedne)
{
	this.wpis.blur();
	this.wpis.style.color="black";
	console.log(wspolrzedne);
	this.wpis.style.backgroundColor="#99CCFF";
	this.wspolrzedne = wspolrzedne;
	if(this.znacznik!=null)
		usunZnacznik(this.znacznik);
	//TODO SKASOWANIE STAREGO
	this.znacznik = nowyZnacznik(this.wspolrzedne, this.color, this);
	this.show = 1;
	console.log(this.wspolrzedne);
	var lol=wspolrzedne[0]+"x"+wspolrzedne[1];
	console.log(lol);
	this.wpis.value=lol;

}

WprowadzanieWspolrzednych.prototype.instantCoordinates = function()
{
	return this.wspolrzedne[0]+"x"+this.wspolrzedne[1];
}

WprowadzanieWspolrzednych.prototype.onInput=function(wspolrzedne)
{
	this.wpis.value="";
	this.wpis.style.color="black";
	this.wpis.style.backgroundColor="white";
	this.show = 0;
	if(this.znacznik!=null)
	{
		usunZnacznik(this.znacznik);
		this.znacznik=null;
	}
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


