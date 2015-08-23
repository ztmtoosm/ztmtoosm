#!/bin/bash

function textdata ()
{
	this_year=$(date -d @"$1" +%y)
	this_month=$(date -d @"$1" +%m)
	this_day=$(date -d @"$1" +%d)
	echo "wget --user=anonymous ftp://rozklady.ztm.waw.pl/RA"$this_year$this_month$this_day".7z -O "$2
}

function textdata2 ()
{
	this_year=$(date -d @"$1" +%y)
	this_month=$(date -d @"$1" +%m)
	this_day=$(date -d @"$1" +%d)
	echo "RA"$this_year$this_month$this_day".TXT"
}

today=$(date +%s)

wynik=8

while [ $wynik -ne 0  ] ; do
	$(textdata $today $1/data/ztm_download0.7z)
	wynik=$?
	today2=$(textdata2 $today)
	echo $wynik
	let "today = today - 24*60*60"
	echo $today
done
7z e $1"/data/ztm_download0.7z" -o$1"/data"
rm $1"/data/ztm_download0.7z"
mv $1"/data/"$today2 $1"/data/ztm_download1.txt"
iconv -f windows-1250 -t utf-8 $1"/data/"ztm_download1.txt > $1"/data/"ztm_downloade.txt
mv $1"/data/"ztm_downloade.txt $1"/data/"ztm_download1.txt
mv $1"/data/"ztm_download1.txt $1"/data/"warszawa.txt
wget "http://www.ztm.waw.pl/rss.php?l=1&IDRss=3" -O $1"/www/rss.xml"

# SZCZECIN

rm -r $1"/data/szczecin"
wget http://zditm.szczecin.pl/rozklady/GTFS/latest/google_gtfs.zip -O $1"/data/szczecin.zip"
unzip $1"/data/szczecin.zip" -d $1"/data/szczecin"

chmod -R 777 $1"/data"

#GDAŃSK

rm -r $1"/data/gdansk2"
rm -r $1"/data/gdansk"
wget http://www.ztm.gda.pl/rozklady/pobierz_rozklady.php -O $1"/data/gdansk.zip"
unzip $1"/data/gdansk.zip" -d $1"/data/gdansk2"
mkdir $1"/data/gdansk"
cp -r $1"/data/gdansk2"/*/* $1"/data/gdansk"

$1"/osm_download.sh" $1
# mysql -u root --local-infile < $1"/ztm_download/"ztm_download3.txt
