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
#include "DockItem.h"
#include <string>
#include <iostream>

#include "Utilities.h"

using namespace std;

/**
 * ctor
 */
DockItem::DockItem()
{
    this->m_image = NULLPB;
}

/**
 * get the current item from the items vector
 * @return DockItem*
 */
DockItem* DockItem::GetCurrent()
{
    if (m_items.size() == 0)
        return nullptr;

    if (m_index >= (int)m_items.size())
        m_index = 0;

    DockItem* result = m_items.at(m_index);

    return result;

}
/**
 * get the next item from the items vector
 * @return DockItem*
 */
DockItem* DockItem::GetNext()
{
    if (m_items.size() == 0)
        return nullptr;

    if (m_index >= (int)m_items.size())
        m_index = 0;

    DockItem* result = m_items.at(m_index);
    m_index++;

    return result;

}
/**
 * dtor
 * release memory
 */
DockItem::~DockItem()
{
    for (auto itemtorelease : m_items)
        delete(itemtorelease);

    m_items.clear();

}


std::string DockItem::getTitle()
{
    if( !m_titlename.empty() )
        return m_titlename;
    
    return m_realgroupname;
}


std::string DockItem::getDesktopFileName()
{
    std::string desktopfile(Utilities::stringToLower(m_realgroupname.c_str()));
    std::size_t foundspace = desktopfile.find(" ");
    if (foundspace > 0) {
        std::string s = desktopfile;
        std::replace(s.begin(), s.end(), ' ', '-'); // replace all ' ' to '-'
        desktopfile = s;
    }
    
    return desktopfile+".desktop";
}