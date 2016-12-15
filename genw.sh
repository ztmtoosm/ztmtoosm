#if [ -z "$2" ]
#then
#	rm $1/lol45.db
#fi
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/src && make && cd $DIR
#$1/ztmtoosm/bin/treelite $1/ztmtoosm/data/warszawa.osm $1/ztmtoosm/lol45.db 3651336
#echo $?
#$1/ztmtoosm/bin/osmlite $1/ztmtoosm/data/warszawa.osm $1/ztmtoosm/lol45.db
#echo $?
#dropdb nexty
#createdb nexty
#$1/bin/mgen2 Warszawa $1/data/warszawa.txt
#$1/bin/treelite $1/data/warszawa.osm  xx 3651336
$DIR/bin/operatorpg Warszawa $DIR/data/warszawa.txt
$DIR/bin/osmpg $DIR/data/warszawa.osm 3651336
echo $?
