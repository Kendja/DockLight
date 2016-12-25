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

/**
 * The Entry Point.
 * @param argc
 * @param argv
 * @return exit code
 */
int main(int argc, char *argv[])
{
    Glib::RefPtr<Gtk::Application> app =
            Gtk::Application::create(argc, argv, "org.gtkmm.docklight");
    
    AppWindow win;
    int r = win.Init(panel_locationType::BOTTOM);
    if (r != 0) {
        exit(r);
    }

    return app->run(win);
}
