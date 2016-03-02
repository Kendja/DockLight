CC = g++

LDFLAGS =   -lX11  -lxcb  `pkg-config  --cflags --libs gtkmm-3.0 libwnck-3.0 `
CPPFLAGS =  -g  -std=c++11 `pkg-config  --cflags gtkmm-3.0 libwnck-3.0`

#-DDEBUG=1

APPNAME = docklight
BINDIR = bin

# Macro that uses the backslash to extend to multiple lines.
OBJS =  \
 Utilities.o \
 AppWindow.o \
 DockPanel.o \
 DockItem.o \
 XPreview.o \
 main.o \
 $(NULL)

# Where are include files kept
INCLUDE = Classes -I/usr/include/libwnck-3.0 -I/usr/include/gtk-3.0/gtk 

all:	$(OBJS)
	$(CC) $(CPPFLAGS) -o$(BINDIR)/$(APPNAME) $(OBJS) $(LDFLAGS) 
clean:  
	rm -f $(OBJS)
	
	
main.o: proj.linux/main.cpp 
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c proj.linux/main.cpp

AppWindow.o: Classes/AppWindow.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/AppWindow.cpp

DockPanel.o: Classes/DockPanel.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/DockPanel.cpp
	
DockItem.o: Classes/DockItem.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/DockItem.cpp	

XPreview.o: Classes/XPreview.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/XPreview.cpp	

Utilities.o: Classes/Utilities.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/Utilities.cpp	
