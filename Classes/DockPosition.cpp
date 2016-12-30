//*****************************************************************
//
//  Copyright (C) 2016 Juan R. Gonzalez
//  Created on December 2016
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
#include <cairo/cairo.h>

#include "DockPosition.h"
#include "MonitorGeometry.h"


class DockPanel;

namespace DockPosition
{

    int m_cellwidth = DEF_CELLWIDTH;
    int m_previewWidth = DEF_PREVIEW_WIDTH;
    int m_previewHeight = DEF_PREVIEW_HEIGHT;
    
    
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
     * If the PreviewItems count are greater then the monitor width we need 
     * to compute the new width. 
     * @param int  dockitemscount
     * @param int &width
     * @param itn &height
     * @return true if was computed false if not.
     */
    bool getPreviewItemGeometry(int dockitemscount, int &width, int &height)
    {
         // avoid 0 division 
         if( dockitemscount < 1 )
            return false;
        
        int maxize = (dockitemscount  ) * DEF_PREVIEW_WIDTH;
        int diff = maxize - MonitorGeometry::getGeometry().width;

        if (diff < 0) {

            m_previewWidth = width = DEF_PREVIEW_WIDTH;
            m_previewHeight = height = DEF_PREVIEW_HEIGHT;
           
            return true;
        }

        int factor = abs((int) (diff / dockitemscount   ));
        m_previewWidth = width = (DEF_PREVIEW_WIDTH - factor) - 6 ;
        if( m_previewWidth <= DEF_PREVIEW_HEIGHT )
            m_previewHeight = height = m_previewWidth - (DEF_PREVIEW_WIDTH - DEF_PREVIEW_HEIGHT);
       
        
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