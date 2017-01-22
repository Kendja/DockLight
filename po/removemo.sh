#!/bin/bash

for i in *.po
do
	d=${i%.*}
	#echo $d 
	if [ -d "$d" ]; then
		rm -fr $d  
	fi

	mofile="/usr/local/share/locale/$d/LC_MESSAGES/docklight.mo"
	if [ -f "$mofile" ]; then
		rm -f $mofile	
	fi
	
done
