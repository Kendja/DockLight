
#ifndef LAUNCHER_H
#define LAUNCHER_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include "Utilities.h"
#include <libwnck/libwnck.h>

class DockItem;

namespace Launcher
{
    bool Launch(DockItem* item);
    std::string getTitleNameFromDesktopFile(std::string desktopfile);
    std::string getTitleNameFromDesktopFile(std::string desktopfile, std::string desktopfile2 );
    std::string getWindowTitle(WnckWindow *window);
}

#endif /* LAUNCHER_H */

