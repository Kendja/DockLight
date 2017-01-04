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

#ifndef DOCKPOSITION_H
#define DOCKPOSITION_H

#include "Defines.h"


namespace DockPosition
{
   
    int getDockItemCenterPos(int dockitemscount, int currentindex, int targetwidth);
    bool getDockItemGeometry(int dockitemscount, int &cellsize, int &iconsize);
    bool getPreviewItemGeometry(int dockitemscount, int &width,int &height);
    int getHomeMenuTopPosition();
    int getItemMenuTopPosition();
    bool isAutoHide();
    void setAutoHide(bool autohide);
    bool isVisible();
    void setVisibleState(bool visible);
}

#endif /* DOCKPOSITION_H */

