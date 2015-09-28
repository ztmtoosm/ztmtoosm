var http = require('http');
var pg = require('pg');
var url = require('url');

var conString = "postgres://postgres:1234@localhost/koordynacje";

//var Warszawa = JSON.parse(fs.readFileSync('Przystanki.json', 'utf8'));
http.createServer(function (req, res) {


	    // Website you wish to allow to connect
	        res.setHeader('Access-Control-Allow-Origin', '*');

		    // Request methods you wish to allow
		        res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');

			    // Request headers you wish to allow
			        res.setHeader('Access-Control-Allow-Headers', 'X-Requested-With,content-type');

				    // Set to true if you need the website to include cookies in the requests sent
				        // to the API (e.g. in case you use sessions)
					    res.setHeader('Access-Control-Allow-Credentials', true);


	var url_parts = url.parse(req.url, true);
	res.setHeader('Content-Type', 'application/json');
	/*if(url_parts.query.size==undefined)
	{
		var from1 = url_parts.query.from;
		var from = parseInt(from1);
		var to1 = url_parts.query.to;
		var to = parseInt(to1);
		var wynik = [];
		for(var i=from; i<=to; i++)
		{
			wynik[i-from]=Warszawa[i];
		}
		console.log(JSON.stringify(wynik));
		console.log("--------------------");
		res.end(JSON.stringify(wynik));
	}
	else
	{
		res.end("{\"size\":"+Warszawa.length+"}");
	}*/
 pg.connect(conString, function(err, client, done) {




client.query('SELECT b.name AS fromname, c.name AS toname, info_super.fromx, info_super.tox, info_super.line_info, info_super.descript FROM line_info INNER JOIN info_super ON (line_info.hashs=info_super.hashs AND line_info.lineid=\''+url_parts.query.id+'\') INNER JOIN stop_names AS b ON (info_super.fromx=b.id) INNER JOIN stop_names AS c ON (info_super.tox=c.id) ORDER BY linekol;', function(err, result) {

done();
res.writeHead(200, {'content-type': 'text/plain'});
res.end(JSON.stringify(result));


});
});
}).listen(1337, '127.0.0.1');
