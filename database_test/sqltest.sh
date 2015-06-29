dropdb gis
createdb gis
psql -d gis -c 'CREATE EXTENSION postgis; CREATE EXTENSION hstore;'
osm2pgsql --create --database gis /ztmtoosm/data/szczecin.osm --slim -C 400 --proj 4326 -l --style style.txt  --tag-transform-script style.lua
# psql -d gis -U root -a -f sqltest2.txt
