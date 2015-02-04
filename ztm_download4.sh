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
	$(textdata $today $1/ztm_download0.7z)
	wynik=$?
	today2=$(textdata2 $today)
	echo $wynik
	let "today = today - 24*60*60"
	echo $today
done
7z e $1"/ztm_download0.7z"
rm $1"/ztm_download0.7z"
mv $1"/"$today2 $1"/ztm_download1.txt"
iconv -f windows-1250 -t utf-8 $1"/"ztm_download1.txt > $1"/"ztm_downloade.txt
mv $1"/"ztm_downloade.txt $1"/"ztm_download1.txt
$1"/ztm_download5" $1"/ztm_download1.txt" $1"/ztm_download2.txt"
mysql -u root --local-infile < $1"/"ztm_download3.txt
