
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "Utilities.h"
class DockItem;

namespace Launcher
{
    bool Launch(DockItem* item);
    std::string getTitleNameFromDesktopFile(std::string desktopfile);
    std::string getTitleNameFromDesktopFile(std::string desktopfile, std::string desktopfile2 );
}

#endif /* LAUNCHER_H */

