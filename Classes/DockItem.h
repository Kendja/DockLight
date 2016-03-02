/* 
 * File:   DockItem.h
 * Author: yoo
 *
 * Created on November 20, 2015, 7:15 PM
 */

#ifndef DOCKITEM_H
#define	DOCKITEM_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <gtkmm.h> 	
#include <libwnck/libwnck.h>
#include "Defines.h"

class DockItem
{
public:
    DockItem();
    virtual ~DockItem();

    Glib::RefPtr<Gdk::Pixbuf> m_image;

    DockItem* GetCurrent();
    DockItem* GetNext();
    WnckWindow *m_window;
    std::string m_appname;
    std::string m_groupname;
    std::string m_realgroupname;
    std::string m_instancename;
        
    std::vector<DockItem*>* m_items;
    int m_xid;
    int m_index = 0;
    int m_isFixed = 0;
    bool m_isDirty = false;
    bool visible = true;
private:
    //TODO: setters getters

};

#endif	/* DOCKITEM_H */

