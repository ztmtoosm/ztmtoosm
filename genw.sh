if [ -z "$1" ]
then
	rm /ztmtoosm/lol45.db
fi
/ztmtoosm/bin/treelite /ztmtoosm/data/warszawa.osm /ztmtoosm/lol45.db 3651336
echo $?
/ztmtoosm/bin/osmlite /ztmtoosm/data/warszawa.osm /ztmtoosm/lol45.db
echo $?
/ztmtoosm/bin/mgen Warszawa /ztmtoosm/data/warszawa.txt /ztmtoosm/lol45.db
echo $?
