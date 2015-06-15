#!/bin/bash
readarray -t a < $1/data/lista9.ooo
pol="wget http://overpass-api.de/api/map?bbox=20.5225,51.9,21.5250,52.4953 -O "$1"/data/latest0.osm"
pol2="wget http://overpass-api.de/api/map?bbox=18.3691,54.2175,18.9439,54.4968 -O "$1"/data/gdansk.osm"
pol3="wget http://overpass-api.de/api/map?bbox=14.2946,53.20,14.81,53.64 -O "$1"/data/szczecin.osm"
$pol
$pol2
$pol3
for i in "${a[@]}"
do
	wget http://api.openstreetmap.org/api/0.6/relation/"${i}" -O $1"/data/"test"${i}".osm
done
b='../osmconvert '$1'/data/latest0.osm'
for i in "${a[@]}"
do
	b+=" "$1"/data/test"${i}".osm"
done
b+=' -o='$1'/data/latest.osm'
$b
for i in "${a[@]}"
do
	rm $1"/data/test"${i}".osm"
done
c=$1"/data/ztm_download1.txt"
d=$(date +"%Y/%m/%d")
$1"/bin/"RouteInfoGenerator $c $1"/data/"latest.osm $1"/data/wyj4.osm" $1"/www" "-all"
$1"/bin/"Generator2 $1"/data/"gdansk $1"/data/"gdansk.osm $1"/www" Gdańsk ref:ztm
