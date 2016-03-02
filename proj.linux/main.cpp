#include <algorithm>
#include <string>

#include <gtk/gtk.h> 	// user c version for samples
#include <gtkmm.h> 	// remove this include 
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include "Utilities.h"
#include <gtkmm/application.h>

#include "AppWindow.h"



int main(int argc, char *argv[])
{
    Glib::RefPtr<Gtk::Application> app = 
            Gtk::Application::create(argc, argv, "org.gtkmm.docklight");
    
    AppWindow win;
    int r = win.Init(panel_locationType::BOTTOM); 
    if( r != 0 )
    {
        exit(r);
    }
    
    return app->run(win);
   
}
