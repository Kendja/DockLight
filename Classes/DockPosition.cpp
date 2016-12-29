#include <cairo/cairo.h>

#include "DockPosition.h"
#include "MonitorGeometry.h"


class DockPanel;

namespace DockPosition
{

    int m_cellwidth = DEF_CELLWIDTH;
    
    /**
     * If the DockItems count are greater then the monitor width we need 
     * to compute the new width of the cell and the new size of the icon.
     * @param int dockitemscount
     * @param int& cellsize
     * @param int& iconsize
     * @return true if was computed false if not.
     */
    bool getDockItemGeometry(int dockitemscount, int &cellsize, int &iconsize)
    {
        // avoid 0 division 
        if( dockitemscount < 3 )
            return false;
        
        int maxize = (dockitemscount + 2) * DEF_CELLWIDTH;
        int diff = maxize - MonitorGeometry::getGeometry().width;

        if (diff < 0) {

            m_cellwidth = cellsize = DEF_CELLWIDTH;
            iconsize = DEF_ICONSIZE;
            return true;
        }

        int factor = abs((int) (diff / dockitemscount - 2));
        m_cellwidth = cellsize = DEF_CELLWIDTH - factor;
        iconsize = DEF_ICONSIZE - factor;
        
        return true;

    }

    /**
     * Calculates the center position required to center a target window
     * on top of a DockItem. 
     * @param int dockitemscount the total count of items.
     * @param int currentindex  the current DockItem index.
     * @param int targetwidth the width of the target window.
     * @return the center position.
     */
    int getCenterPosByCurrentDockItemIndex(int dockitemscount,
            int currentindex, int targetwidth)
    {

        int center = (MonitorGeometry::getGeometry().width / 2);
        int col = center - (dockitemscount * m_cellwidth) / 2;
        int centerpos = MonitorGeometry::getGeometry().x +
                col + (m_cellwidth / 2) + (m_cellwidth * currentindex);

        return centerpos - (targetwidth / 2);
    }
}