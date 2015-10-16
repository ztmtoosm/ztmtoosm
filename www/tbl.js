var loadedPag = false;
var nodejs = false;
var akttable = [];
var addr = "";
var tableSize = 0;
var from = 0;
var aktsort= -1;
var to = 100;
var sorttable = [function(a, b){if(a.name<b.name) return -1; if(a.name>b.name) return 1; return 0;},
function(a, b){if(a.BS_SP<b.BS_SP) return -1; if(a.BS_SP>b.BS_SP) return 1; return 0;},
function(a, b){if(a.powod<b.powod) return -1; if(a.powod>b.powod) return 1; return 0;}
 ];



var divAll;
var divPag1;
var divPag2;
var newGroupItem = function(ojciec, data)
{
	var li = document.createElement("li");
	li.setAttribute('class', 'list-group-item');
	li.innerHTML=data;
	ojciec.appendChild(li);
}

var osmUrl = function(type, id)
{
	var type2 = "node";
	if(type=="W")
		type2="way";
	if(type=="R")
		type2="relation";
	if(id==0 || id=="0" || id==undefined || type==undefined)
		return "<b>BRAK</b>";
	return "<a href=\"http://openstreetmap.org/"+type2+"/"+id+"\">"+id+"</a>";
}

var sortuj = function(fsort)
{
	$(divAll).html("czekaj");
	akttable.sort(fsort);
	$(divAll).html("");
	for(var i=0; i<akttable.length; i++)
	{
		divAll.appendChild(akttable[i].div);
	}
}

var sortujnode = function(fsortid)
{
	from = 0;
	to = 100;
	var url = {};
	url.url = addr+"&from="+from+"&to="+to+"&sort="+fsortid;
	tblLoad(url);
}

var addBtn = function (par, txt, fsrtid)
{
	var btn = document.createElement("button");
	btn.setAttribute("type", "button");
	btn.setAttribute("class", "btn btn-default");
	btn.innerHTML=txt;
	btn.fsrt = sorttable[fsrtid];
	btn.fsrtid = fsrtid;
	if(nodejs)
	{
		btn.addEventListener('click', function() {
			sortujnode(this.fsrtid);
		}, false);
	}
	else
	{
		btn.addEventListener('click', function() {
			sortuj(this.fsrt);
		}, false);
	}
	//$(btn).click=sortuj(divAll, fsrt);
	par.appendChild(btn);
}

var BpSpCreator = function (BS_SP, SP_PL, PL_BS)
{
	var wynik = "";
	if(BS_SP>=0 || SP_PL>=0 || PL_BS >=0)
	{
		wynik+="Odległości: ";
	}
	if(BS_SP>=0)
	{
		wynik+=" bus_stop-stop_position: ";
		if(BS_SP>100)
			wynik+="<b>";
		wynik+=BS_SP;
		if(BS_SP>100)
			wynik+="</b>";
	}
	if(SP_PL>=0)
	{
		wynik+=" platform-stop_position: ";
		if(SP_PL>100)
			wynik+="<b>";
		wynik+=SP_PL;
		if(SP_PL>100)
			wynik+="</b>";
	}
	if(PL_BS>=0)
	{
		wynik+=" platform-bus_stop: ";
		if(PL_BS>100)
			wynik+="<b>";
		wynik+=PL_BS;
		if(PL_BS>100)
			wynik+="</b>";
	}
	return wynik;
}

var infoLabel = function (url, desc)
{
	return "<a class=\"label label-info\" target=\"_blank\"   href=\""+url+"\">"+desc+"</a>";
}

var osmInfo = function(lon, lat)
{
	var url = "http://openstreetmap.org/?mlat="+lat+"&mlon="+lon+"&zoom=18";
	var left = lon-0.002;
	var right = lon+0.002;
	var topp = lat+0.001;
	var bottom = lat-0.001;
	var url2 = "http://localhost:8111/load_and_zoom?left="+left+"&right="+right+"&top="+topp+"&bottom="+bottom;
	return infoLabel(url, 'OSM')+" "+infoLabel(url2, 'JOSM');
}
var tblLoad = function(url)
{
	divAll.innerHTML = "";
	var x = document.getElementsByClassName("container");
	x[0].innerHTML="czekaj";
	jQuery.get(url.url, function(data) {
	var x = document.getElementsByClassName("container");
	x[0].innerHTML="";


	akttable = [];
	for(var i=0; i<data.length; i++)
	{if(data[i]!=null){
		console.log(akttable.length);
		var div = document.createElement("div");
		var divHead = document.createElement("div");
		var divGroup = document.createElement("div");
		var stat = "panel-success";
		if(data[i].powod==1)
			stat = "panel-danger";
		if(data[i].powod>2 && data[i].powod<1000)
			stat = "panel-warning";
		div.setAttribute('class', "panel "+stat);
		div.setAttribute('id', data[i].id);
		divHead.setAttribute('class', 'panel-heading');
		divGroup.setAttribute('class', 'list-group');
		divHead.innerHTML = data[i].id+" "+data[i].name;
		divHead.innerHTML += " - b_stop "+osmUrl('N', data[i].bus_stop)+" stop_pos "+osmUrl('N', data[i].stop_position)+" platform "+osmUrl(data[i].platform_type, data[i].platform);
		div.appendChild(divHead);
		div.appendChild(divGroup);
		newGroupItem(divGroup, "bus_stop name: "+data[i].bus_stop_name+", stop_position name: "+data[i].stop_position_name+", platform name: "+data[i].platform_name);
		//newGroupItem(divGroup, "bus_stop "+osmUrl('N', data[i].bus_stop)+" stop_position "+osmUrl('N', data[i].stop_position)+" platform "+osmUrl(data[i].platform_type, data[i].platform));
		var slo1 = BpSpCreator(data[i].BS_SP, data[i].SP_PL, data[i].PL_BS);
		if(slo1.length>0)
			newGroupItem(divGroup, slo1);
		var divP = document.createElement("button");
		var divQ = document.createElement("div");
		divP.setAttribute("class", "list-group-item");
		divP.setAttribute("type", "button");
		divP.setAttribute("data-toggle", "collapse");
		divP.setAttribute("href", "#"+data[i].id+"linie");
		divP.setAttribute("aria-controls", data[i].id+"linie");
		divP.setAttribute("aria-expanded", "false");
		divP.innerHTML+="Pokaż linie przebiegające przez przystanek <span class=\"caret\"></span>";
	
		divQ.setAttribute("class", "collapse");
		divQ.setAttribute("id", data[i].id+"linie");
		
		if(data[i].latlon_jakosc>0 || data[i].lat2!=undefined)
		{
			var info = "";
			if(data[i].latlon_jakosc>0)
			{
				info = "wsp operatora: "+data[i].lon+" "+data[i].lat+" ";
				if(data[i].latlon_jakosc==1)
					info += "NIEDOKŁADNE ";
				info+=osmInfo(data[i].lon, data[i].lat);
				if(data[i].lat2!=undefined)
					info+=" ";
			}

			if(data[i].lat2!=undefined)
			{
				info += "wsp OSM: "+data[i].lon2+" "+data[i].lat2+" ";
				info+=osmInfo(data[i].lon2, data[i].lat2);
			}
			newGroupItem(divGroup, info);
		}

		divGroup.appendChild(divP);
		divGroup.appendChild(divQ);
		for(var j=0; j<data[i].kierunki.length; j++)
			newGroupItem(divQ, data[i].kierunki[j]);

		

		divAll.appendChild(div);
		data[i].div = div;
		akttable[akttable.length]=data[i];
	}}
	var divBtn = document.createElement("div");
	divBtn.setAttribute("class", "btn-group");
	divBtn.setAttribute("role", "group");
	addBtn(divBtn, "nazwa", 0);
	addBtn(divBtn, "BP_SP", 1);
	addBtn(divBtn, "problem", 2);
	x[0].innerHTML+="sortowanie:<br>";
	x[0].appendChild(divBtn);
	//x[0].innerHTML+="<div class=\"col-xs-12\" style=\"height:30px;\"></div>";
	x[0].appendChild(divAll);

	});
	/*
		jQuery.get("List.json", function(data) {
		var navbar = document.getElementById("tutej0");
		navbar.innerHTML += "<li class=\"dropdown\">  <a href=\"#\" class=\"dropdown-toggle\" data-toggle=\"dropdown\" role=\"button\" aria-haspopup=\"true\" aria-expanded=\"false\">Dropdown <span class=\"caret\"></span></a><ul class=\"dropdown-menu\" id=\"tutej\"> </ul><li>";
		var tutej = document.getElementById("tutej");
		for(var i=0; i<data.length; i++)
		{
			tutej.innerHTML+="<li><a href=\"Pelne"+data[i]+".html\">"+data[i]+"</a></li>";
		}
	});*/
}

var tbl = function(url)
{
	divAll = document.createElement("xyz");
	tblLoad(url);
}

var tbl2 = function(serverAddr)
{
	aktsort = 1;
	addr = serverAddr;
	nodejs = true;
	divAll = document.createElement("xyz");
	var url = {};
	url.url = serverAddr+"&from=0&to=100&sort=1";
	jQuery.get(serverAddr+"&size=true", function(data) {
	tableSize = data.size;
	console.log(tableSize);
	
	   var options = {
	bootstrapMajorVersion : 3,
		    currentPage: from/100+1,
		    totalPages: parseInt(tableSize/100)+1,
		   onPageChanged: function(e,oldPage,newPage){
		    to = newPage*100;
		    from = (newPage-1)*100;
			var url = {};
		url.url = addr+"&from="+from+"&to="+to+"&sort="+aktsort;
		tblLoad(url);
		console.log(oldPage+" "+newPage);
			}
		};
			divpag1 = document.createElement("ul");
			divpag2 = document.createElement("ul");
			document.body.appendChild(divpag1);
			document.body.appendChild(divpag2);
			$(divpag1).bootstrapPaginator(options);
			$(divpag2).bootstrapPaginator(options);
	tblLoad(url);
	document.body.appendChild(divpag2);
	});
}
$.support.transition = false;
