//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on January  2016
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

#ifndef WINDOWCONTROL_H
#define WINDOWCONTROL_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm/window.h> 
#include "DockItem.h"

#include <X11/X.h>


namespace WindowControl
{
    void HomeCloseAllExceptActive();
    void HomeCloseAllWindows();
    void HomeMinimizeAll();
    void HomeMinimizeAllExceptActive();
    void HomeUnMinimizeAll();
    
    void ActivateWindow(WnckWindow* window);
    
    void closeAllExceptActiveByDockItem(DockItem* dockitem);
    void closeAllByDockItem(DockItem* dockitem);
    
    int isExitstWindowsByDockItem(DockItem* dockitem);
    bool isExitsActivetWindowByDockItem(DockItem* dockitem);

    
    void minimizeAllExceptActiveByDockItem(DockItem* dockitem);
    void minimizeAllByDockItem(DockItem* dockitem);
    
    void unMinimizeAllByDockItem(DockItem* dockitem);
    
    bool isExistsUnMaximizedWindowsByDockItem(DockItem* dockitem);
    bool isExistsMinimizedWindowsByDockItem(DockItem* dockitem);
    
    bool isExistsMinimizedWindows();
    
    int windowscount();
    int minimizedWindowscount();
    int unMinimizedWindowsCount();
    
    
    WnckWindow* getActive();
    bool isWindowExists(XID xid);
    
    void hideWindow(Gtk::Window* instance);
    
   
}

#endif /* WINDOWCONTROL_H */

