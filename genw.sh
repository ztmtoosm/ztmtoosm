#if [ -z "$2" ]
#then
#	rm $1/lol45.db
#fi
cd src && make && cd ../
#$1/ztmtoosm/bin/treelite $1/ztmtoosm/data/warszawa.osm $1/ztmtoosm/lol45.db 3651336
#echo $?
#$1/ztmtoosm/bin/osmlite $1/ztmtoosm/data/warszawa.osm $1/ztmtoosm/lol45.db
#echo $?
#dropdb nexty
#createdb nexty
#$1/bin/mgen2 Warszawa $1/data/warszawa.txt
#$1/bin/treelite $1/data/warszawa.osm  xx 3651336
#$1/bin/operatorpg Warszawa $1/data/warszawa.txt
$1/bin/osmpg $1/data/warszawa.osm xx 3651336
echo $?
