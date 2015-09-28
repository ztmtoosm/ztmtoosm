var run = function()
{
	var name = document.getElementById("lin").value;
	jQuery.get("http://127.0.0.1:1337/?id="+name, function(data) {
		var tab = data;
		var table = document.createElement("table");
		document.getElementById("divCore").innerHTML="";
		document.getElementById("divCore").appendChild(table);
		for(var i=0; i<tab.rows.length; i++)
		{
			var row = document.createElement("tr");
			
			var fromname = document.createElement("td");
			var toname = document.createElement("td");
			var line = document.createElement("td");
			var desc = document.createElement("td");
			
			fromname.innerHTML=tab.rows[i].fromname;
			toname.innerHTML=tab.rows[i].toname;
			line.innerHTML=tab.rows[i].line_info;
			desc.innerHTML=tab.rows[i].descript;

			row.appendChild(fromname);
			row.appendChild(toname);
			row.appendChild(line);
			row.appendChild(desc);
			table.appendChild(row);
		}
	});
}
