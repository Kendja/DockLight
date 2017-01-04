#
# Regular cron jobs for the docklight package
#
0 4	* * *	root	[ -x /usr/bin/docklight_maintenance ] && /usr/bin/docklight_maintenance
