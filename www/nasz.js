var style = {
  'Point': [new ol.style.Style({
    image: new ol.style.Circle({
      fill: new ol.style.Fill({
        color: 'rgba(255,255,0,0.4)'
      }),
      radius: 5,
      stroke: new ol.style.Stroke({
        color: '#ff0',
        width: 1
      })
    })
  })],
  'LineString': [new ol.style.Style({
    stroke: new ol.style.Stroke({
      color: '#f0f',
      width: 12
    })
  })],
  'MultiLineString': [new ol.style.Style({
    stroke: new ol.style.Stroke({
      color: '#fff',
      width: 5
    })
  })]
};
var style2 = {
  'Point': [new ol.style.Style({
    image: new ol.style.Circle({
      fill: new ol.style.Fill({
        color: 'rgba(255,255,0,0.4)'
      }),
      radius: 5,
      stroke: new ol.style.Stroke({
        color: '#ff0',
        width: 1
      })
    })
  })],
  'LineString': [new ol.style.Style({
    stroke: new ol.style.Stroke({
      color: '#000',
      width: 1
    })
  })],
  'MultiLineString': [new ol.style.Style({
    stroke: new ol.style.Stroke({
      color: '#f07',
      width: 2
    })
  })]
};
    var projection = ol.proj.get('EPSG:3857');
var layerLines = new ol.layer.Vector({
  source: new ol.source.GPX({
  projection: projection,
    url: "http://192.168.1.110/142.gpx"
  }),
    style: function(feature, resolution) {
    return style[feature.getGeometry().getType()];
  }
});
var layerLines2 = new ol.layer.Vector({
  source: new ol.source.GPX({
  projection: projection,
    url: "http://192.168.1.110/142.gpx"
  }),
    style: function(feature, resolution) {
    return style2[feature.getGeometry().getType()];
  }
});

      var map = new ol.Map({
        target: 'map',
        layers: [
          new ol.layer.Tile({
            source: new ol.source.OSM({layer: 'sat'})
          }), layerLines, layerLines2],
        view: new ol.View({
          center: ol.proj.transform([21,52.3], 'EPSG:4326', 'EPSG:3857'),
          zoom: 10
        })});
            function changesource(path)
            {
	      map.removeLayer(layerLines2);
map.removeLayer(layerLines);
layerLines = new ol.layer.Vector({
source: new ol.source.GPX({
projection: projection,
url: path
}),
style: function(feature, resolution) {
return style[feature.getGeometry().getType()];
}
});
layerLines2 = new ol.layer.Vector({
source: new ol.source.GPX({
projection: projection,
url: path
}),
style: function(feature, resolution) {
return style2[feature.getGeometry().getType()];
}
});
    map.addLayer(layerLines);
        map.addLayer(layerLines2);
            }
    
    var elementos = document.getElementsByClassName("ok_route");
    console.log(elementos.length);
    for(i=0; i<elementos.length; i++)
    {
      var str=elementos[i].innerHTML;
     elementos[i].addEventListener('click', function() { changesource(this.getAttribute('id')+".gpx"); console.log(this.innerHTML)});    
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
