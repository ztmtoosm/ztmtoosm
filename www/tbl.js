
var akttable = [];

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
		return "BRAK";
	return "<a href=\"http://openstreetmap.org/"+type2+"/"+id+"\">"+id+"</a>";
}

var tbl = function(url)
{
	jQuery.get(url.url, function(data) {
	for(var i=0; i<data.length; i++)
	{
		var div = document.createElement("div");
		var divHead = document.createElement("div");
		var divGroup = document.createElement("div");
		div.setAttribute('class', 'panel panel-success');
		div.setAttribute('id', data[i].id);
		divHead.setAttribute('class', 'panel-heading');
		divGroup.setAttribute('class', 'list-group');
		divHead.innerHTML = data[i].id+" "+data[i].name;
		div.appendChild(divHead);
		div.appendChild(divGroup);
		newGroupItem(divGroup, "bus_stop name: "+data[i].bus_stop_name+", stop_position name: "+data[i].stop_position_name+", platform name: "+data[i].platform_name);
		newGroupItem(divGroup, "bus_stop "+osmUrl('N', data[i].bus_stop)+" stop_position "+osmUrl('N', data[i].stop_position)+" platform "+osmUrl(data[i].platform_type, data[i].platform));
		newGroupItem(divGroup, "BS_SP="+data[i].BS_SP+" SP_PL="+data[i].SP_PL+" PL_BS="+data[i].PL_BS);
		for(var j=0; j<data[i].kierunki.length; j++)
			newGroupItem(divGroup, data[i].kierunki[j]);
		newGroupItem(divGroup, data[i].lon);
		newGroupItem(divGroup, data[i].lat);
		var x = document.getElementsByClassName("container");
		x[0].appendChild(div);
		akttable[akttable.length]=div;
	}
});
}
