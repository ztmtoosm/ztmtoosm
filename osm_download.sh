#!/bin/bash
readarray -t a < data/lista9.ooo
pol="wget http://overpass-api.de/api/map?bbox=20.5225,51.9561,21.5250,52.4953 -O "+$1+"/data/latest0.osm"
# $pol
for i in "${a[@]}"
do
	wget http://api.openstreetmap.org/api/0.6/relation/"${i}" -O $1"/data/"test"${i}".osm
done
b='../osmconvert/osmconvert '$1'/data/latest0.osm'
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
$1"/bin/"dijkstra $c $1"/data/"latest.osm $1"/data/wyj4.osm" $1"/www" "-all"
