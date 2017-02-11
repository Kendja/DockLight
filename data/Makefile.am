SUBDIRS = \
	icons \
	$(NULL)

EXTRA_DIST = \
	docklight.menu.desktop \
	org.freedesktop.docklight.policy \
	docklight.template.desktop \
	docklight.home.ico \
	docklight-session.png \
	docklight.logo.png \
	docklight.ini \
	docklight.launcher.sh \
	$(NULL)

#CLEANFILES = \
#	docklight.menu.desktop \
#	$(NULL)

#/usr/share/polkit-1/actions
policydir = /usr/share/polkit-1/actions
policy_DATA = org.freedesktop.docklight.policy

#/usr/local/share
#applicationsdir = $(datadir)/applications
#applications_DATA = 4autotools/docklight.menu.desktop

applicationsdir = /usr/share/applications
applications_DATA = docklight.menu.desktop


#/docklight.data
docklightdatadir = $(bindir)/data
docklightdata_DATA = \
		docklight.home.ico \
		docklight.ini \
		docklight.logo.png \
		docklight-session.png \
		docklight.launcher.sh \
		docklight.template.desktop \
		$(NULL)

@INTLTOOL_DESKTOP_RULE@

#if test -f "/usr/local/bin/docklight"; then rm -f "/usr/local/bin/docklight"; fi
#ln -s  $(bindir)/docklight "/usr/local/bin/docklight"
install-exec-hook:
	if test ! -d $(bindir)/data; then mkdir $(bindir)/data; fi
	if test ! -d $(bindir)/data/attachments; then mkdir $(bindir)/data/attachments; fi
	chmod -R 0777 $(bindir)/data		
	
	
	

	
	
uninstall-hook:
	if test -L "/usr/local/bin/docklight"; then rm "/usr/local/bin/docklight"; fi
	if test -d $(bindir)/data; then rm -fr $(bindir)/data; fi
	if test -d $(prefix); then rm -fr $(prefix); fi


#desktopdir = $(datadir)/applications
#desktop_in_files = docklight.menu.desktop.in
#desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
#@INTLTOOL_DESKTOP_RULE@


