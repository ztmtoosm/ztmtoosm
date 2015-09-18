
var akttable = [];

var newGroupItem = function(ojciec, data)
{
	var li = document.createElement("li");
	li.setAttribute('class', 'list-group-item');
	li.innerHTML=data;
	ojciec.appendChild(li);
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
		newGroupItem(divGroup, data[i].name);
		newGroupItem(divGroup, data[i].stop_position);
		newGroupItem(divGroup, data[i].kierunki0);
		newGroupItem(divGroup, data[i].lon);
		newGroupItem(divGroup, data[i].lat);
		var x = document.getElementsByClassName("container");
		x[0].appendChild(div);
		akttable[akttable.length]=div;
	}
});
}
