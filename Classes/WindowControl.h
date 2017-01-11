

#ifndef WINDOWCONTROL_H
#define WINDOWCONTROL_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include "DockItem.h"


namespace WindowControl
{
    void HomeCloseAllExceptActive();
    void HomeCloseAllWindows();
    void HomeMinimizeAll();
    void HomeMinimizeAllExceptActive();
    void HomeUnMinimizeAll();
    
    void ActivateWindow(WnckWindow* window);
    void unminimizeAllByDockItem(DockItem* dockitem);
    void minimizeAllByDockItem(DockItem* dockitem);
    void unMinimizeAllByDockItem(DockItem* dockitem);
    
    bool isExistsUnMaximizedWindowsByDockItem(DockItem* dockitem);
    bool isExistsMinimizedWindowsByDockItem(DockItem* dockitem);
    
    int windowscount();
    int minimizedWindowscount();
    int unMinimizedWindowsCount();
    
    
}

#endif /* WINDOWCONTROL_H */

