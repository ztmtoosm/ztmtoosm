    var elementos = document.getElementsByClassName("ok_route");
    console.log(elementos.length);
    for(i=0; i<elementos.length; i++)
    {
      var str=elementos[i].innerHTML;
     elementos[i].addEventListener('click', function() { var lol=layerLines2.getSource().getFeatures();
console.log(lol.length); changesource(this.getAttribute('id')+".gpx"); console.log(this.innerHTML)});    
    }


  var elementos = document.getElementsByClassName("bledy1");
    console.log(elementos.length);
    for(i=0; i<elementos.length; i++)
    {
	   var str1= elementos[i].id;
	      var str2=str1.match(/\_.+/);
	      var str3=str2[0].substring(1, str2[0].length);
	      elementos[i].str3=str3;
     		elementos[i].addEventListener('click', function() {document.getElementById("roznice_"+this.str3).style.display="inline"});
      console.log(str3);
    }
