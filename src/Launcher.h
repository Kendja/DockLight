//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on January  2017
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

