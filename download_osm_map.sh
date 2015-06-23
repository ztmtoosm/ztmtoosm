pol="wget http://overpass-api.de/api/map?bbox="$3","$4","$5","$6" -O "$1"/data/test.osm"
echo $pol
for ((i=2;i<$#;i++))
do
	echo ${!i}
done
