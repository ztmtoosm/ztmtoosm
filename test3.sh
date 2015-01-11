#!/bin/bash
c=RA141229.TXT
d=$(date +"%Y/%m/%d")
./dijkstra $c latest.osm wyj4.osm "-noll"
