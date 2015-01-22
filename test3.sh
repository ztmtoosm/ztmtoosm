#!/bin/bash
c=RA150123.TXT
d=$(date +"%Y/%m/%d")
./dijkstra $c latest.osm wyj4.osm "-noll"
