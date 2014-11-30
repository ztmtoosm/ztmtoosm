#!/bin/bash
c=RA141124.TXT
d=$(date +"%Y/%m/%d")
./dijkstra $c latest.osm wyj4.osm $d
