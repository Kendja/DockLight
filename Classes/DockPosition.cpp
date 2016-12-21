#include <cairo/cairo.h>

#include "DockPosition.h"
#include "MonitorGeometry.h"


class DockPanel;

namespace DockPosition
{

    /**
     * Calculates the center position required to center a target window.
     * to positioning on the center from a DockItem. 
     * @param dockitemscount the total count of items.
     * @param currentindex  the current DockItem index.
     * @param targetwidth the width of the target window.
     * @return the width center position.
     */
    int getCenterPosByCurrentDockItemIndex(int dockitemscount, 
            int currentindex,int targetwidth)
    {
        int center = (MonitorGeometry::getGeometry().width / 2);
        int col = center - (dockitemscount * DEF_CELLSIZE) / 2;
        int centerpos = MonitorGeometry::getGeometry().x + 
            col + (DEF_CELLSIZE / 2) + (DEF_CELLSIZE * currentindex);

        return centerpos - (targetwidth/2);
    }
}