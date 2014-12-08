#!/bin/bash
c=RA141208.TXT
d=$(date +"%Y/%m/%d")
./dijkstra $c latest.osm wyj4.osm "-noll"
