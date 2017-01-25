SUBDIRS = \
	icons \
	$(NULL)

EXTRA_DIST = \
	4autotools/docklight.menu.desktop \
	4autotools/org.freedesktop.docklight.policy \
	docklight.template.desktop \
	docklight.home.ico \
	docklight.ini \
	docklight.launcher.sh \
	docklight.logo.png \
	$(NULL)

#CLEANFILES = \
#	docklight.menu.desktop \
#	$(NULL)

#/usr/share/polkit-1/actions
policydir = /usr/share/polkit-1/actions
policy_DATA = 4autotools/org.freedesktop.docklight.policy

#/usr/local/share
applicationsdir = $(datadir)/applications
applications_DATA = 4autotools/docklight.menu.desktop

#/docklight.data
docklightdatadir = $(bindir)/docklight.data
docklightdata_DATA = \
		docklight.home.ico \
		docklight.ini \
		docklight.launcher.sh \
		docklight.logo.png \
		docklight.template.desktop \
		$(NULL)

@INTLTOOL_DESKTOP_RULE@

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


