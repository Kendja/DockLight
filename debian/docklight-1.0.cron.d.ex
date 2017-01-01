#
# Regular cron jobs for the docklight-1.0 package
#
0 4	* * *	root	[ -x /usr/bin/docklight-1.0_maintenance ] && /usr/bin/docklight-1.0_maintenance
