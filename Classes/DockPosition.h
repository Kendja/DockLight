#ifndef DOCKPOSITION_H
#define DOCKPOSITION_H

#include "Defines.h"

namespace DockPosition
{
    int getCenterPosByCurrentDockItemIndex(int dockitemscount, 
            int currentindex,int targetwidth);
 
    bool getDockItemGeometry(int dockitemscount, int &cellsize, int &iconsize);
}

#endif /* DOCKPOSITION_H */

