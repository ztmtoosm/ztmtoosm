#!/bin/bash
$1"/download_osm_map.sh" $1 warszawa 20.5225 51.9 21.5250 52.4953 3651332 3651328 3651327 3651329 3651326 3651335 3651336 3651333 4656333 3651331 
$1"/download_osm_map.sh" $1 szczecin 14.2946 53.20 14.81 53.64 5291742
$1"/bin/"RouteInfoGenerator $1"/data/"warszawa.txt $1"/data/"warszawa.osm $1"/data/wyj4.osm" $1"/www" "-all"
$1"/bin/"Generator2 $1"/data/"szczecin $1"/data/"szczecin.osm $1"/www" Szczecin ref
#$1"/bin/"Generator2 $1"/data/"gdansk $1"/data/"gdansk.osm $1"/www" Gdańsk ref:ztm
$1"/bin/"Generator2 $1"/data/"warszawa.txt $1"/data/"warszawa.osm $1"/www" Warszawa ref
