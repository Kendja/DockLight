/* 
 * File:   DockItem.cpp
 * Author: yoo
 * 
 * Created on November 20, 2015, 7:15 PM
 */

#include "DockItem.h"

DockItem::DockItem()
{
    this->m_image = (Glib::RefPtr<Gdk::Pixbuf>)NULL;
    this->m_items = new std::vector<DockItem*>();


}

DockItem* DockItem::GetCurrent()
{
    if (m_items->size() == 0)
        return NULL;
    
    if (m_index >= m_items->size())
        m_index = 0;

    DockItem* result = m_items->at(m_index);

    return result;

}

DockItem* DockItem::GetNext()
{

    if (m_items->size() == 0)
        return NULL;
        
    if (m_index >= m_items->size())
        m_index = 0;

    DockItem* result = m_items->at(m_index);
    m_index++;

    return result;

}

DockItem::~DockItem()
{
}

