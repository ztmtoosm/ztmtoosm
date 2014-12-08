#!/bin/bash
readarray -t a < ../lista9.ooo
pol="wget http://overpass-api.de/api/map?bbox=20.5225,51.9561,21.5250,52.4953 -O latest0.osm"
# $pol
for i in "${a[@]}"
do
	wget http://api.openstreetmap.org/api/0.6/relation/"${i}" -O test"${i}".osm
done
b='../osmconvert/osmconvert latest0.osm'
for i in "${a[@]}"
do
	b+=" test"${i}".osm"
done
b+=' -o=latest.osm'
$b
for i in "${a[@]}"
do
	rm "test"${i}".osm"
done
c=RA141208.TXT
d=$(date +"%Y/%m/%d")
./dijkstra $c latest.osm wyj4.osm "-all"
