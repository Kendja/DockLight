
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "Utilities.h"
class DockItem;

namespace Launcher
{
    bool Launch(DockItem* item);
    std::string getTitleNameFromDesktopFile(std::string desktopfile);
}

#endif /* LAUNCHER_H */

