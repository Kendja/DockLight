//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 7:15 PM
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
#ifndef DOCKITEM_H
#define DOCKITEM_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>

#include <gtkmm/window.h> 
#include <glibmm/timer.h>
#include "Defines.h"

class DockItem
{
public:
    DockItem();
   ~DockItem();
    Glib::RefPtr<Gdk::Pixbuf> m_image;
    guchar m_pixelsbuf[DEF_PREVIEW_WIDTH * DEF_PREVIEW_HEIGHT * 3] = {0};
    GdkPixbuf *m_scaledPixbuf;
    bool m_imageLoadedRequired;
    bool m_isDynamic;
    DockItem* GetCurrent();
    DockItem* GetNext();
    WnckWindow *m_window;
    std::string m_titlename;
    std::string m_appname;
    std::string m_realgroupname;
    std::string m_instancename;
    std::vector<DockItem*> m_items;
    int m_xid;
    int m_index = 0;
    int m_attachedIndex=0;
    bool m_isAttached = false;
    bool m_isDirty = false;
    bool visible = true;
    int m_frames;
    std::string getTitle();
    std::string getDesktopFileName();
    gboolean isMovementDetected(GdkPixbuf *pixbuf);
    int m_dockitemSesssionGrpId = 0;
private:
    bool m_pixbufPreviousPass;

};


#endif /* DOCKITEM_H */

