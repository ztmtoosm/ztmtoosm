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
function lineRou(id)
{
	var sciezka="/wyszuk/js";
	sciezka+=id+".json";
	console.log(sciezka);
	return sciezka;
}
function punktId (id)
{
	var sciezka="/dijkstra";
	sciezka+='\?val\='+id;
	return sciezka;
}
function punktySciezka (from, to)
{
	var sciezka="/dijkstra";
	sciezka+='\?from\='+from;
	sciezka+='\&to\='+to;
	return sciezka;
}

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

function makeMovable(feature, moveMove) 
{
	var modify = new ol.interaction.Modify({
		features: new ol.Collection([feature])
	});

  feature.on('change',function(ev) {
	  this.sterownik.znaczek = this;
	  this.sterownik.znaczekWsp = ol.proj.transform(this.getGeometry().getCoordinates(), 'EPSG:3857', 'EPSG:4326')}, feature);
	return modify;
}


/*
Relacja.prototype.dodajLacznikCzesciowo = function (from, to, col)
{
	var lacznik = loadJSON(punktySciezka(from.osmid, to.osmid));
	var tablica1 = [];
	for(var i=0; i<lacznik.length; i++)
	{
		var tmp =ol.proj.transform([lacznik[i].x, lacznik[i].y], 'EPSG:4326', 'EPSG:3857');
		tablica1[tablica1.length]=tmp;
	}
	var l2=addLineToSource (tablica1, from.id, col, "xxx", this.manager.basicZnaczniki, this.manager.basicVector);
	return [lacznik, l2];
}








function zrownaj()
{
    	var heart = document.getElementById("heart");
    	var map0 = document.getElementById("map0");
	heart.style.maxHeight=(window.innerHeight-110)+"px";
}
zrownaj();


window.addEventListener("resize", zrownaj, false);
}
var znaczek = null;
$("#map").on("mouseup", function(){if(znaczek!=null){zmienionyZnacznik(znaczek);}znaczek = null;});
*/
function getQueryParams(qs) {
	    qs = qs.split("+").join(" ");

	        var params = {}, tokens,
		        re = /[?&]?([^=]+)=([^&]*)/g;

			    while (tokens = re.exec(qs)) {
				            params[decodeURIComponent(tokens[1])]
					                = decodeURIComponent(tokens[2]);
							    }

							        return params;
}

var managerRelacji = new ManagerRelacji();

var parFff = getQueryParams(document.location.search);
console.log(parFff.linia);
var tablica;
if(parFff.linia!=undefined)
{
	console.log(punktId(parFff.linia));
	tablica = loadJSON(lineRou(parFff.linia));
	//rrr = new Relacja(managerRelacji, tablica);
}
else
{
//	rrr = new Relacja(managerRelacji);
}

console.log(tablica);
var KeyValue = function (root, kv)
{
	this.div = document.createElement("DIV");
	var key = document.createElement("INPUT");
	key.type="text";
	var value = document.createElement("INPUT");
	value.type="text";
	this.div.appendChild(key);
	this.div.appendChild(value);
	key.value = kv.key;
	value.value = kv.value;
	root.div.appendChild(this.div);
}
KeyValue.prototype.getPair = function()
{
	return {key: "xxx", value : "yyy"};
}

var RelationMember = function (root, memberData)
{
	this.div = document.createElement("DIV");
	var category = document.createElement("INPUT");
	category.type="text";
	var osmId = document.createElement("INPUT");
	osmId.type="text";
	var role = document.createElement("INPUT");
	role.type="text";
	this.div.appendChild(category);
	this.div.appendChild(osmId);
	this.div.appendChild(role);
	category.value = memberData.category;
	osmId.value = memberData.id;
	role.value = memberData.role;
	root.div.appendChild(this.div);
}
KeyValue.prototype.getPair = function()
{
	return {role: "stop", id : "5", category: "W"};
}

var Tags = function (root, tags)
{
	this.div = document.createElement("DIV");
	root.div.appendChild(this.div);
	var addTag = document.createElement("INPUT");
	addTag.type="submit";
	addTag.id="nowytag";
	addTag.value="nowy tag";
	addTag.root=this;
	addTag.onclick=function()
	{
		this.root.keyValues[this.root.keyValues.length] = new KeyValue(this.root, {key:"", value:""});
	}
	this.keyValues = [];
	this.div.appendChild(addTag);
	for(var i=0; i<tags.length; i++)
	{
		var tmp = new KeyValue(this, tags[i]);
		this.keyValues[i] = tmp;
	}
}
Tags.prototype.getTags = function()
{
	var wynik = [];
	for(var i=0; i<this.keyValues.length; i++)
	{
		wynik[i] = this.keyValues[i].getPair();
	}
	return wynik;
}


var Members = function (root, members)
{
	this.div = document.createElement("DIV");
	root.div.appendChild(this.div);
	var addMember = document.createElement("INPUT");
	addMember.type="submit";
	addMember.id="nowytag";
	addMember.value="nowy tag";
	addMember.root=this;
	addMember.onclick=function()
	{
		this.root.memberValues[this.root.memberValues.length] = new RelationMember(this.root, {role:"", category:"", id:99});
	}
	this.memberValues = [];
	this.div.appendChild(addMember);
	for(var i=0; i<members.length; i++)
	{
		var tmp = new RelationMember(this, members[i]);
		this.memberValues[i] = tmp;
	}
}
Members.prototype.getTags = function()
{
	var wynik = [];
	for(var i=0; i<this.memberValues.length; i++)
	{
		wynik[i] = this.memberValues[i].getPair();
	}
	return wynik;
}

var Relation = function(root, relation)
{
	this.div = document.createElement("DIV");
	root.div.appendChild(this.div);
	this.tags = new Tags(this, relation.tags);
	this.members = new Members(this, relation.members);
}
var rel = new Relacja(managerRelacji, tablica[0].track);
var gowno = new Relation({div : document.body}, tablica[0]);
