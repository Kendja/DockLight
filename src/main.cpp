//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 12:17 PM 
//  j-gonzalez@email.de
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************
#include <gtkmm/application.h>
#include "AppWindow.h"
#include <sstream>
#include "Utilities.h"

#include <glibmm/i18n.h>
#include <gtkmm/main.h>
#include <libintl.h>
#include <config.h>

/**
 * The Entry Point.
 * @param argc
 * @param argv
 * @return exit code
 */
int main(int argc, char *argv[])
{

   // const char* localedir = "/usr/lib/docklight/docklight.data/locale";
    const char* localedir = "/usr/lib/docklight/share/locale";
    
    
    
   
    char* domain = bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
    g_print("bindtextdomain: %s %s %s\n",domain,GETTEXT_PACKAGE,PROGRAMNAME_LOCALEDIR);
   
    // bind_textdomain_codeset - set encoding of message trans‐lations
    char* btdcodeset = bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    g_print("bind_textdomain_codeset: %s\n",btdcodeset);
    // The  textdomain  function  sets or retrieves the current  message domain.
    char* txtdomain = textdomain(GETTEXT_PACKAGE);
    g_print("textdomain: %s\n\n",txtdomain);
      
    /*
    char* domain = bindtextdomain(GETTEXT_PACKAGE, localedir);
    g_print("bindtextdomain: %s %s %s\n",domain,GETTEXT_PACKAGE,localedir);
   
    // bind_textdomain_codeset - set encoding of message trans‐lations
    char* btdcodeset = bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    g_print("bind_textdomain_codeset: %s\n",btdcodeset);
    // The  textdomain  function  sets or retrieves the current  message domain.
    char* txtdomain = textdomain(GETTEXT_PACKAGE);
    g_print("textdomain: %s\n\n",txtdomain);
    */
    
    Glib::RefPtr<Gtk::Application> app =
            Gtk::Application::create(argc, argv, "org.gtkmm.docklight");

    AppWindow win;
    int autohide = 0;

    std::string filepath = Utilities::getExecPath(DEF_INITNAME);
    GError *error = NULL;
    GKeyFile *key_file = g_key_file_new();

    gboolean found = g_key_file_load_from_file(key_file,
            filepath.c_str(), GKeyFileFlags::G_KEY_FILE_NONE, &error);

    if (found) {
        // read the Autohide Entry 
        found = g_key_file_get_boolean(key_file, "DockLight", "Autohide", &error);
        autohide = (int) found;
        if (error) {
            g_warning("Autohide Entry not found. %s\n", error->message);
            g_error_free(error);
            error = NULL;
        }
    } else {
        g_warning("docklight config  file not found.\n");
        if (error) {
            g_error_free(error);
            error = NULL;
        }
    }


    int r = win.Init(panel_locationType::BOTTOM, autohide);
    if (r != 0) {
        exit(r);
    }
    
    int result =  app->run(win);
    g_print("Terminate with code %d \n",result);
    return result;
}
