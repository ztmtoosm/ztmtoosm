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
function punktySciezka (from, to, type)
{
	var sciezka="/dijkstra";
	sciezka+='\?from\='+from;
	sciezka+='\&to\='+to;
	sciezka+='\&przelicznik\='+type;
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
var tablica = [];
if(parFff.linia!=undefined)
{
	var tablicatmp = loadJSON(lineRou(parFff.linia));
	for(var i=0; i<tablicatmp.length; i++)
		tablica[tablica.length]=tablicatmp[i];
}
if(parFff.linia1!=undefined)
{
	var tablicatmp = loadJSON(lineRou(parFff.linia1));
	for(var i=0; i<tablicatmp.length; i++)
		tablica[tablica.length]=tablicatmp[i];
}
if(parFff.linia2!=undefined)
{
	var tablicatmp = loadJSON(lineRou(parFff.linia2));
	for(var i=0; i<tablicatmp.length; i++)
		tablica[tablica.length]=tablicatmp[i];
}

console.log(tablica);
var addOption = function (root, value, selected)
{
	var option = document.createElement("OPTION");
	root.appendChild(option);
	option.id = value;
	option.innerHTML = value;
	if(selected)
	{
		option.selected="selected";
	}
}

var addSelect = function (root, values, selected)
{
	var cat = document.createElement("SELECT");
	root.appendChild(cat);
	for(var i=0; i<values.length; i++)
	{
		addOption(cat, values[i], (values[i]==selected));
	}
	cat.getVal=function()
	{
		return this.options[this.selectedIndex].value;
	}
	return cat;
}
function typeOf (obj) {
  return {}.toString.call(obj).split(' ')[1].slice(0, -1).toLowerCase();
}
var KeyValue = function (root, kv, types)
{
	this.div = document.createElement("DIV");
	this.div.className="keyvalue";
	this.vals = [];
	for(var i=0; i<kv.length; i++)
	{
		if(typeOf(types[i])=="array")
		{
			this.vals[i]=addSelect(this.div, types[i], kv[i]);
		}
		else
		{
			var nowy = document.createElement("INPUT");
			nowy.type="text";
			nowy.getVal=function(){return this.value;};
			this.div.appendChild(nowy);
			nowy.value = kv[i];
			this.vals[i]=nowy;
		}
	}
	root.div.appendChild(this.div);
}
KeyValue.prototype.getPair = function()
{
	var wyniki = [];
	for(var i=0; i<this.vals.length; i++)
	{
		wyniki[i]=this.vals[i].getVal();
	}
	return wyniki;
}

/*
var RelationMember = function (root, memberData)
{
	this.div = document.createElement("DIV");
	this.div.className="relationmember";
	this.category = addSelect(this.div, ["N", "W", "R"], memberData.category);
	this.category.className="membercategory";
	//addOption(this.category, "N", ("N"==memberData.category));
	//addOption(this.category, "W", ("W"==memberData.category));
//	addOption(this.category, "R", ("R"==memberData.category));
	this.osmId = document.createElement("INPUT");
	this.osmId.type="text";
	this.role = document.createElement("INPUT");
	this.role.type="text";
	this.div.appendChild(this.osmId);
	this.div.appendChild(this.role);
	this.category.value = memberData.category;
	this.osmId.value = memberData.id;
	this.role.value = memberData.role;
	root.div.appendChild(this.div);
}
RelationMember.prototype.getPair = function()
{
	return {category: this.category.options[this.category.selectedIndex].value, id: this.osmId.value, role: this.role.value};
}
*/

var Tags = function (root, tags)
{
	this.div = document.createElement("DIV");
	this.div.className="tags";
	root.div.appendChild(this.div);
	var addTag = document.createElement("INPUT");
	addTag.type="submit";
	addTag.id="nowytag";
	addTag.value="nowy tag";
	addTag.root=this;
	addTag.onclick=function()
	{
		this.root.keyValues[this.root.keyValues.length] = new KeyValue(this.root, ["", ""], ["", ""]);
	}
	this.keyValues = [];
	this.div.appendChild(addTag);
	for(var i=0; i<tags.length; i++)
	{
		var tmp = new KeyValue(this, [tags[i].key, tags[i].value], ["", ""]);
		this.keyValues[i] = tmp;
	}
}
Tags.prototype.getTags = function()
{
	var wynik = [];
	for(var i=0; i<this.keyValues.length; i++)
	{
		var w1=this.keyValues[i].getPair();
		wynik[i]={key: w1[0], value: w1[1]};
	}
	return wynik;
}
/*
var ParentRelations = function (root, rels)
{
	this.div = document.createElement("DIV");
	root.div.appendChild(this.div);
	this.parentRel = [];
	for(var i=0; i<rels.length; i++)
	{
		var addTag = document.createElement("INPUT");
		addTag.type="text";
		addTag.value=rels[i];
		this.div.appendChild(addTag);
		this.parentRel[i]=addTag;
	}
	this.but = document.createElement("INPUT");
	this.but.type = "submit";
	this.but.root=this;
	this.but.onclick=function()
	{
		var addTag = document.createElement("INPUT");
		addTag.type="text";
		addTag.value=rels[i];
		this.div.appendChild(addTag);
		this.root.parentRel[this.root.parentRel.length]=addTag;
	}
	this.keyValues = [];
	this.div.appendChild(addTag);
	for(var i=0; i<tags.length; i++)
	{
		var tmp = new KeyValue(this, tags[i]);
		this.keyValues[i] = tmp;
	}
}
ParentRelations.prototype.getTags = function()
{
	var wynik = [];
	for(var i=0; i<this.keyValues.length; i++)
	{
		wynik[i] = this.keyValues[i].getPair();
	}
	return wynik;
}
*/
var Members = function (root, text, text2, show, headers)
{
	this.text = text;
	this.text2 = text2;
	this.divCore = document.createElement("DIV");
	this.showMore = document.createElement("A");
	if(show)
		this.showMore.innerHTML = text+" - hide";
	else
		this.showMore.innerHTML = text+" - show";
	this.showMore.showed = show;
	this.showMore.href="javascript:void(0)";
	this.showMore.root=this;
	this.divDoUkrycia = document.createElement("DIV");
	this.div = document.createElement("DIV");
	var header = document.createElement("DIV");
	this.div.className="members";
	root.div.appendChild(this.divCore);
	this.divCore.appendChild(this.showMore);
	this.divCore.appendChild(this.divDoUkrycia);
	this.div.appendChild(header);
	this.divDoUkrycia.appendChild(this.div);
	if(!show)
		$(this.divDoUkrycia).hide();
	var addMember = document.createElement("INPUT");
	addMember.type="submit";
	addMember.id="nowytag";
	addMember.value=text2;
	addMember.root=this;
	
	if(show)
	{
		addMember.onclick=function()
		{
			this.root.memberValues[this.root.memberValues.length] = new KeyValue(this.root,["",""], ["", ""]);
		}
	}
	else
	{
		addMember.onclick=function()
		{
			this.root.memberValues[this.root.memberValues.length] = new KeyValue(this.root,["N","",""], [["N", "W", "R"],"", ""]);
		}
	}
	this.showMore.onclick=function()
	{
		if(this.showed)
		{
			this.showed = false;
			this.innerHTML = this.root.text+" - show";
			$(this.root.divDoUkrycia).hide(1000);
		}
		else
		{
			this.showed = true;
			this.innerHTML = this.root.text+" - hide";
			$(this.root.divDoUkrycia).show(1000);
		}
	}
	for(var i=0; i<headers.length; i++)
	{
		var headercategory = document.createElement("DIV");
		header.appendChild(headercategory);
		headercategory.innerHTML=headers[i];
	}
	this.memberValues = [];
	this.divDoUkrycia.appendChild(addMember);
}
Members.prototype.addMember = function(member, val)
{
	this.memberValues[this.memberValues.length] = new KeyValue(this, member, val);
}
Members.prototype.getTags = function(fun)
{
	var wynik = [];
	for(var i=0; i<this.memberValues.length; i++)
	{
		var ww = this.memberValues[i].getPair();
		wynik[i] = fun(ww);
	}
	return wynik;
}

var aktRelacja = null;

var Relation = function(root, relation)
{
	this.relation = relation;
	this.div = document.createElement("DIV");
	this.div.className="relation";
	this.track_type = addSelect(this.div, ["bus", 'tram'], relation.track_type);
	this.relId = document.createElement("DIV");
	this.posrednie = document.createElement("DIV");
	$(this.posrednie).hide();
	this.posrednieHeader = document.createElement("A");
	this.posrednieHeader.href = "javascript:void(0)";
	this.posrednieHeader.root = this;
	this.posrednieHeader.showed = false;
	this.posrednieHeader.onclick = function()
	{
		if(this.showed)
		{
			this.showed = false;
			$(this.root.posrednie).hide(1000);
		}
		else
		{
			this.showed = true;
			$(this.root.posrednie).show(1000);
		}
	}
	this.posrednieHeader.innerHTML = "XXX";
	this.posrednie.innerHTML = "";
	var textDe ="";
	if(relation.todelete==true)
	{
		textDe = "TO DELETE ";
	}
	this.relId.innerHTML = "Relation "+textDe+"id: "+relation.id;
	this.div.appendChild(this.relId);
	//this.div.appendChild(this.posrednieHeader);
	this.div.appendChild(this.posrednie);
	this.relacja = null;
	this.relLink = document.createElement("A");
	this.relLink.root = this;
	if(relation.todelete!=true)
	{
		this.relLink.innerHTML="show route on map";
		this.relLink.href="javascript:void(0)";
		this.relLink.onclick = function()
		{	
			this.root.addTrack();
		}	
		this.div.appendChild(this.relLink);
		this.tags = new Members(this, "tags", "new tag", true, ["key", "value"]);
		for(var i=0; i<relation.tags.length; i++)
		{
			this.tags.addMember([relation.tags[i].key, relation.tags[i].value], ["", ""]);
		}
		this.members = new Members(this, "additional members", "new member", false, ["type","member id", "role"]);
		for(var i=0; i<relation.members.length; i++)
		{
			this.members.addMember([relation.members[i].category, relation.members[i].id, relation.members[i].role], [["N", "W", "R"], "", ""]);
		}
	}
	root.div.appendChild(this.div);
}
Relation.prototype.getJSON = function()
{
	var tags = {};
	var members = {};
	if(this.tags!=undefined)
	{
		tags = this.tags.getTags(function(obj){return {key: obj[0], value: obj[1]}});
		members = this.members.getTags(function(obj){return {category: obj[0], id: obj[1], role: obj[2]}});
	}
	var todelete = false;
	if(this.relation.todelete==true)
		todelete=true;
	if(this.relation.track_type==undefined)
		this.relation.track_type="bus";
	return {track_type: this.relation.track_type, parentrel : this.relation.parentrel, todelete : todelete, id : this.relation.id, finaltrack : this.relacja.getTrackPoints(), tags: tags, members: members};
}
Relation.prototype.updatePosrednie = function()
{
	if(this.relacja!=null)
	{
		var lol = this.relacja.getTrackPoints();
		console.log(lol);
		this.posrednie.innerHTML = "";
		for(var i=0; i<lol[0].length; i++)
		{
			this.posrednie.innerHTML+=lol[0][i]+"</br>";
		}
	}
}
Relation.prototype.podswietl = function()
{
	this.div.style.backgroundColor="yellow";
}
Relation.prototype.zaciemnij = function()
{
	this.div.style.backgroundColor="white";
}
Relation.prototype.addTrack = function()
{
	if(aktRelacja!=null)
		aktRelacja.clear();
	if(this.relacja!=null)
	{
		this.relacja.reopen();
	}
	else
	{
		this.relacja = new Relacja(managerRelacji, this.relation.track, this.relation.track_type);
		this.relacja.handler = this;
		this.relacja.onChange = function (these)
		{
			these.updatePosrednie();
		}
		this.relacja.onReopen = function (these)
		{
			these.podswietl();
		}
		this.relacja.onClear = function (these)
		{
			these.zaciemnij();
		}
	}
	this.updatePosrednie();
	this.podswietl();
	aktRelacja = this.relacja;
}
var wszystkieRelacje = [];
var allJSON = function()
{
	var wynik = [];
	for(var i=0; i<wszystkieRelacje.length; i++)
	{
		wynik[i]=wszystkieRelacje[i].getJSON();
	}
	return wynik;
}
for(var i=0; i<tablica.length; i++)
{
	var gowno = new Relation({div : document.body}, tablica[i]);
	wszystkieRelacje [i] = gowno;
	gowno.addTrack();
}
$(document).ready(function(){
var but = document.getElementById("generalbutton");
but.onclick=function()
{
	if(this.value != "CZEKAJ około 15s")
	{
		$.post("/dijkstra", JSON.stringify(allJSON()), function(data, status, xhr){
		var ndiv = document.createElement("DIV");
		var a1 = document.createElement("A");
		var a2 = document.createElement("A");
		a1.href="/wyszuk/"+data+".osm";
		a1.innerHTML="SHOW GENERATED *.osm FILE";
		a1.target="_blank";
		a2.innerHTML="LOAD FILE TO JOSM";
		a2.href="http://localhost:8111/import?url=http%3A%2F%2Fvps134914.ovh.net%2Fwyszuk%2F"+data+".osm";
		a2.target="_blank";
		ndiv.appendChild(a1);
		ndiv.innerHTML+="<br>";
		ndiv.appendChild(a2);
		document.body.appendChild(ndiv);
		console.log(data);	
			});
		this.value = "PLEASE WAIT";
	}
}
var neurel = document.getElementById("neurel");
neurel.onclick=function()
{
	var prepared_tab={"id": wszystkieRelacje.length-999, "track_type":"bus", "parentrel":[], "tags":[], "track":[], "members":[]};
	var gowno = new Relation({div : document.body}, prepared_tab);
	wszystkieRelacje [wszystkieRelacje.length] = gowno;
	gowno.addTrack();
};

});
/*
$(function() {
  $( "#map0" ).resizable({
 minHeight: 150,  
    minWidth: 200,
	handles: "w"  
});
});*/
