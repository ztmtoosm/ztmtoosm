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
	this.div.className="keyvalue";
	this.key = document.createElement("INPUT");
	this.key.type="text";
	this.value = document.createElement("INPUT");
	this.value.type="text";
	this.div.appendChild(this.key);
	this.div.appendChild(this.value);
	this.key.value = kv.key;
	this.value.value = kv.value;
	root.div.appendChild(this.div);
}
KeyValue.prototype.getPair = function()
{
	return {key: this.key.value, value : this.value.value};
}
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

var RelationMember = function (root, memberData)
{
	this.div = document.createElement("DIV");
	this.div.className="relationmember";
	this.category = document.createElement("SELECT");
	this.category.className="membercategory";
	addOption(this.category, "N", ("N"==memberData.category));
	addOption(this.category, "W", ("W"==memberData.category));
	addOption(this.category, "R", ("R"==memberData.category));
	this.osmId = document.createElement("INPUT");
	this.osmId.type="text";
	var role = document.createElement("INPUT");
	role.type="text";
	this.div.appendChild(this.category);
	this.div.appendChild(this.osmId);
	this.div.appendChild(role);
	this.category.value = memberData.category;
	this.osmId.value = memberData.id;
	role.value = memberData.role;
	root.div.appendChild(this.div);
}
RelationMember.prototype.getPair = function()
{
	return {category: this.category.options[this.category.selectedIndex].value, id: this.osmId.value};
}


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
	this.divCore = document.createElement("DIV");
	this.showMore = document.createElement("A");
	this.showMore.innerHTML = "Dodadkowi członkowie - pokaż";
	this.showMore.showed = false;
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
	$(this.divDoUkrycia).hide();
	var addMember = document.createElement("INPUT");
	addMember.type="submit";
	addMember.id="nowytag";
	addMember.value="nowy członek";
	addMember.root=this;
	addMember.onclick=function()
	{
		this.root.memberValues[this.root.memberValues.length] = new RelationMember(this.root, {role:"", category:"", id:""});
	}
	this.showMore.onclick=function()
	{
		if(this.showed)
		{
			this.showed = false;
			this.innerHTML = "Dodadkowi członkowie - pokaż";
			$(this.root.divDoUkrycia).hide(1000);
		}
		else
		{
			this.showed = true;
			this.innerHTML = "Dodadkowi członkowie - ukryj";
			$(this.root.divDoUkrycia).show(1000);
		}
	}
	var headercategory = document.createElement("DIV");
	header.appendChild(headercategory);
	headercategory.innerHTML="member type";
	var headerid = document.createElement("DIV");
	headerid.innerHTML="id";
	header.appendChild(headerid);
	var headerrole = document.createElement("DIV");
	headerrole.innerHTML="role";
	header.appendChild(headerrole);
	this.memberValues = [];
	this.divDoUkrycia.appendChild(addMember);
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

var aktRelacja = null;

var Relation = function(root, relation)
{
	this.relation = relation;
	this.div = document.createElement("DIV");
	this.div.className="relation";
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
	this.relId.innerHTML = "Relacja id: "+relation.id;
	this.div.appendChild(this.relId);
	this.div.appendChild(this.posrednieHeader);
	this.div.appendChild(this.posrednie);
	this.relacja = null;
	this.relLink = document.createElement("A");
	this.relLink.root = this;
	this.relLink.innerHTML="pokaż trasę";
// - punkty pośrednie: "+relation.track.length;
	this.relLink.href="javascript:void(0)";
	this.relLink.onclick = function()
	{
		this.root.addTrack();
		console.log(JSON.stringify(this.root.getJSON()));
	}	
	this.div.appendChild(this.relLink);
	
	root.div.appendChild(this.div);
	this.tags = new Tags(this, relation.tags);
	this.members = new Members(this, relation.members);
}
Relation.prototype.getJSON = function()
{
	var tags = this.tags.getTags();
	var members = this.members.getTags();
	return {id : this.relation.id, finaltrack : this.relacja.getTrackPoints(), tags: tags, members: members};
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
		this.relacja = new Relacja(managerRelacji, this.relation.track);
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
for(var i=0; i<tablica.length; i++)
{
	var gowno = new Relation({div : document.body}, tablica[i]);
	gowno.addTrack();
}
/*
$(function() {
  $( "#map0" ).resizable({
 minHeight: 150,  
    minWidth: 200,
	handles: "w"  
});
});*/
