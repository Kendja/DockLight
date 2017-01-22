#! /bin/sh -e

rm -rf debian/docklight 
rm -f src/docklight
rm -f src/*.o
rm -f src/Makefile src/Makefile.in 

ls -1 | egrep -v "^(debian|package|m4|bin|po|src|data|man|clean.sh|configure.ac|autogen.sh|autogen2.sh|Makefile.am|nbproject|README.md|LICENSE)$"  | xargs rm -r

cd m4
ls -1 | egrep -v "^(NOTES)$"  | xargs rm -r	


cd ../po
ls -1 | egrep -v "^(de.po|hu.po|LINGUAS|POTFILES.in|compile_all.sh|deploymo.sh)$"  | xargs rm -r

cd ../data
ls -1 | egrep -v "^(docklight.home.ico|docklight.ini|docklight.logo.png|docklight.launcher.sh|Makefile.am|docklight.menu.desktop|org.freedesktop.docklight.policy|docklight.template.desktop)$"  | xargs rm -r



exit 0


