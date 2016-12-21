CC = g++
LDFLAGS =   -static-libstdc++ -lX11  -lxcb  `pkg-config  --cflags --libs gtkmm-3.0 libwnck-3.0 `
CPPFLAGS =  -g -Wall -Wno-reorder -std=c++11 `pkg-config  --cflags gtkmm-3.0 libwnck-3.0`



#-DDEBUG=1

APPNAME = docklight
BINDIR = bin

# Macro that uses the backslash to extend to multiple lines.
OBJS =  \
 Utilities.o \
 AppWindow.o \
 MonitorGeometry.o \
 DockPanel.o \
 DockItem.o \
 IconLoader.o \
 Launcher.o \
 TitleWindow.o \
 Preview.o \
 DockPosition.o \
 main.o \
 $(NULL)

# Where are include files kept
INCLUDE = Classes -I/usr/include/libwnck-3.0 -I/usr/include/gtk-3.0/gtk 

all:	$(OBJS)
	$(CC) $(CPPFLAGS) -o$(BINDIR)/$(APPNAME) $(OBJS) $(LDFLAGS) 
clean:  
	rm -f *.o
	rm -f Classes/*.o

main.o: proj.linux/main.cpp 
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c proj.linux/main.cpp

AppWindow.o: Classes/AppWindow.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/AppWindow.cpp

Utilities.o: Classes/Utilities.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/Utilities.cpp	

TitleWindow.o: Classes/TitleWindow.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/TitleWindow.cpp	

MonitorGeometry.o: Classes/MonitorGeometry.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/MonitorGeometry.cpp	

DockPanel.o: Classes/DockPanel.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/DockPanel.cpp	
	
DockItem.o: Classes/DockItem.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/DockItem.cpp	
		
IconLoader.o: Classes/IconLoader.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/IconLoader.cpp	

Launcher.o: Classes/Launcher.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/Launcher.cpp

DockPosition.o: Classes/DockPosition.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/DockPosition.cpp

Preview.o: Classes/Preview.o
	$(CC) -I$(INCLUDE) $(CPPFLAGS) -c Classes/Preview.cpp

