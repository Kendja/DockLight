
EXTRA_DIST = \
	docklight.menu.desktop.in \
	org.freedesktop.docklight.policy \
	docklight.home.ico \
	docklight.ini \
	docklight.launcher.sh \
	docklight.logo.png \
	$(NULL)

#/usr/share/polkit-1/actions
policydir = /usr/share/polkit-1/actions
policy_DATA = org.freedesktop.docklight.policy

#/usr/local/share
applicationsdir = $(datadir)/applications
applications_in_files = docklight.menu.desktop.in
applications_DATA = $(applications_in_files:.desktop.in=.desktop)

#/docklight.data
docklightdatadir = $(bindir)/docklight.data
docklightdata_DATA = docklight.home.ico docklight.ini docklight.launcher.sh docklight.logo.png


install-exec-hook:
	if test ! -d $(bindir)/docklight.data; then mkdir $(bindir)/docklight.data; fi
	if test ! -d $(bindir)/docklight.data/attachments; then mkdir $(bindir)/docklight.data/attachments; fi
	chmod -R 0777 $(bindir)/docklight.data		
	
	
uninstall-hook:
	if test -d $(bindir)/docklight.data; then rm -fr $(bindir)/docklight.data; fi


#desktopdir = $(datadir)/applications
#desktop_in_files = docklight.menu.desktop.in
#desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
#@INTLTOOL_DESKTOP_RULE@


