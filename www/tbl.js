
var akttable = [];
var divAll;
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

var addBtn = function (par, txt, fsrt)
{
	var btn = document.createElement("button");
	btn.setAttribute("type", "button");
	btn.setAttribute("class", "btn btn-default");
	btn.innerHTML=txt;
	btn.fsrt = fsrt;
	btn.addEventListener('click', function() {
		sortuj(this.fsrt);
	}, false);
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

var tbl = function(url)
{
	divAll = document.createElement("xyz");
	var x = document.getElementsByClassName("container");
	x[0].innerHTML="czekaj";
	jQuery.get(url.url, function(data) {
	for(var i=0; i<data.length; i++)
	{
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
		div.appendChild(divHead);
		div.appendChild(divGroup);
		newGroupItem(divGroup, "bus_stop name: "+data[i].bus_stop_name+", stop_position name: "+data[i].stop_position_name+", platform name: "+data[i].platform_name);
		newGroupItem(divGroup, "bus_stop "+osmUrl('N', data[i].bus_stop)+" stop_position "+osmUrl('N', data[i].stop_position)+" platform "+osmUrl(data[i].platform_type, data[i].platform));
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

		divGroup.appendChild(divP);
		divGroup.appendChild(divQ);
		for(var j=0; j<data[i].kierunki.length; j++)
			newGroupItem(divQ, data[i].kierunki[j]);
		newGroupItem(divGroup, data[i].lon);
		newGroupItem(divGroup, data[i].lat);
		divAll.appendChild(div);
		data[i].div = div;
		akttable[akttable.length]=data[i];
	}
	var divBtn = document.createElement("div");
	divBtn.setAttribute("class", "btn-group");
	divBtn.setAttribute("role", "group");
	var x = document.getElementsByClassName("container");
	addBtn(divBtn, "nazwa", function(a, b){if(a.name<b.name) return -1; if(a.name>b.name) return 1; return 0;});
	addBtn(divBtn, "BS_SP", function(a, b){if(a.BS_SP<b.BS_SP) return -1; if(a.BS_SP>b.BS_SP) return 1; return 0;});
	addBtn(divBtn, "problem", function(a, b){if(a.powod>b.powod) return -1; if(a.powod<b.powod) return 1; return 0;});
	addBtn(divBtn, "id", function(a, b){if(a.id<b.id) return -1; if(a.id>b.id) return 1; return 0;});
	x[0].innerHTML="sortowanie:<br>";
	x[0].appendChild(divBtn);
	//x[0].innerHTML+="<div class=\"col-xs-12\" style=\"height:30px;\"></div>";
	x[0].appendChild(divAll);
});
}
