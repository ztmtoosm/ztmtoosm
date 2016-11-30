#!/bin/bash
#service apache2 stop
wget https://overpass-api.de/api/interpreter?data=relation%283651336%29%20-%3E%20.a%3B%0A%0A%28node%5Bhighway%3Dbus_stop%5D%2851.9172%2C%2020.4469%2C%2052.4928%2C%2021.4687%29%3B%20%0A%20node%5Brailway%3Dtram_stop%5D%2851.9172%2C%2020.4469%2C%2052.4928%2C%2021.4687%29%3B%0A%20way%5Bhighway%3Dplatform%5D%2851.9172%2C%2020.4469%2C%2052.4928%2C%2021.4687%29%3B%0A%20%20way%5Bpublic_transport%3Dplatform%5D%2851.9172%2C%2020.4469%2C%2052.4928%2C%2021.4687%29%3B%0A%20%20way%5Brailway%3Dplatform%5D%2851.9172%2C%2020.4469%2C%2052.4928%2C%2021.4687%29%3B%0A%20%20relation%5Bhighway%3Dplatform%5D%2851.9172%2C%2020.4469%2C%2052.4928%2C%2021.4687%29%3B%0A%20%20relation%5Bpublic_transport%3Dplatform%5D%2851.9172%2C%2020.4469%2C%2052.4928%2C%2021.4687%29%3B%0A%20%20relation%5Brailway%3Dplatform%5D%2851.9172%2C%2020.4469%2C%2052.4928%2C%2021.4687%29%3B%0Anode%5Bpublic_transport%3Dstop_position%5D%2851.9172%2C%2020.4469%2C%2052.4928%2C%2021.4687%29%3B%0A.a%20%3E%3E%3B%29%3B%0Aout%20meta%3B -O $1"/data/"warszawa.osm
#$1"/download_osm_map.sh" $1 szczecin 14.2946 53.20 14.81 53.64 5291742
#$1"/download_osm_map.sh" $1 gdansk 18.2854 54.1547 18.9724 54.5308 5411513
#$1"/bin/"RouteInfoGenerator $1"/data/"warszawa.txt $1"/data/"warszawa.osm Warszawa $1"/www" "-all"
#$1"/bin/"RouteInfoGenerator $1"/data/"szczecin $1"/data/"szczecin.osm Szczecin $1"/www" "-all"
#$1"/bin/"Generator2 $1"/data/"gdansk $1"/data/"gdansk.osm $1"/www" Gdańsk ref:ztm
#$1"/bin/"RouteInfoGeneratorPrim $1"/data/"gdansk $1"/data/"gdansk.osm Gdańsk $1"/www" "-all"
#$1"/bin/"Generator2 $1"/data/"szczecin $1"/data/"szczecin.osm $1"/www" Szczecin ref
#$1"/bin/"Generator2 $1"/data/"warszawa.txt $1"/data/"warszawa.osm $1"/www" Warszawa ref
#$1"/bin/"RouteInfoGeneratorPrim $1"/data/"warszawa.txt $1"/data/"warszawa.osm $1"/www" Warszawa ref
#service apache2 start
