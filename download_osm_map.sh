pol="wget http://overpass-api.de/api/map?bbox="$3","$4","$5","$6" -O "$1"/data/test.osm"
$pol
for ((i=7;i<=$#;i++))
do
	wget http://api.openstreetmap.org/api/0.6/relation/"${!i}" -O $1"/data/"test"${!i}".osm
done
b='osmconvert '$1'/data/test.osm'
for ((i=7;i<=$#;i++))
do
	b+=" "$1"/data/test"${!i}".osm"
done
b+=' -o='$1'/data/'$2'.osm'
$b
for ((i=7;i<=$#;i++))
do
	rm $1"/data/test"${!i}".osm"
done
rm $1'/data/test.osm'
