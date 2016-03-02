




#include <algorithm>
#include <string>

#include <gtk/gtk.h> 	// user c version for samples
#include <gtkmm.h> 	// remove this include 
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include "Utilities.h"

/*
#include <libwnck/libwnck.h>

#include <gtk-3.0/gtk/gtkwindow.h>
#include <gtk-3.0/gtk/gtktypes.h>
#include <gtk-3.0/gdk/gdkmain.h>
#include <gtk-3.0/gdk/gdkwindow.h>
#include <gtk-3.0/gtk/gtkenums.h>
#include <gtk-3.0/gdk/gdkproperty.h>
#include <gtk-3.0/gdk/gdktypes.h>
#include <gtk-3.0/gdk/gdkscreen.h>
#include <gtk-2.0/gdk/gdkscreen.h>
#include <gdkmm-3.0/gdkmm/general.h>

*/

#include <gtkmm/application.h>
#include "AppWindow.h"


//http://cairographics.org/tutorial/
int main(int argc, char *argv[])
{
    Glib::RefPtr<Gtk::Application> app = 
            Gtk::Application::create(argc, argv, "org.gtkmm.docklight");
    
//    
//    char path[PATH_MAX];
//    // getcwd(path, 255);
//     strcat(path, "/");
//     strcat(path, argv[0]);
////
//     printf("--%s\n", path);

     
//     char result[ PATH_MAX ];
//    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
//    std::string path = result;
//    std::size_t found = path.find_last_of("/\\");
//    path = path.substr(0, found);
//    
    
//    printf("--%s\n", Utilities::getExecPath("test").c_str());
//    return 1;
    
    AppWindow win;
    int r = win.Init(panel_locationType::BOTTOM); 
    if( r != 0 )
    {
        exit(r);
    }
    
    return app->run(win);
   
    
    //return 0;
}
