

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

