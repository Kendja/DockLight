//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 
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

/**
 * ctor
 */
DockItem::DockItem()
: m_pixbufPreviousPass(false),
m_image(NULLPB),
m_scaledPixbuf(nullptr),
m_imageLoadedRequired(true),
m_isDynamic(false),
m_frames(0),
m_titlename("")
{

}

/**
 * get the current item from the items vector
 * @return DockItem*
 */
DockItem* DockItem::GetCurrent()
{
    if (m_items.size() == 0)
        return nullptr;

    if (m_index >= (int) m_items.size())
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

    if (m_index >= (int) m_items.size())
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
    if (!m_titlename.empty())
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

    return desktopfile + ".desktop";
}

/**
 * This function will not work for pixbufs with images that are other than 8 bits 
 * per sample or channel, but it will work for most of the pixbufs that GTK+ uses.
 * @param GdkPixbuf* pixbuf
 * @return true is a movement detected or false is none.
 */
gboolean DockItem::isMovementDetected(GdkPixbuf* pixbuf)
{
    gint x, y;
    int w = gdk_pixbuf_get_width(pixbuf);
    int h = gdk_pixbuf_get_height(pixbuf);
    int channels = 3; //gdk_pixbuf_get_n_channels(pixbuf);
    gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    gint pixel_offset;

    // Returns a read-only pointer to the raw pixel data;
    // must not be modified. This function allows skipping the 
    // implicit copy that must be made if gdk_pixbuf_get_pixels()
    // is called on a read-only pixbuf.
    const guint8* pixels = gdk_pixbuf_read_pixels(pixbuf);

    if (!m_pixbufPreviousPass) {
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                pixel_offset = y * rowstride + x * channels;
                this->m_pixelsbuf[pixel_offset] = pixels[ pixel_offset];
            }
        }
        m_pixbufPreviousPass = true;
        return FALSE;
    }

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            pixel_offset = y * rowstride + x * channels;
            if (this->m_pixelsbuf[pixel_offset] != pixels[pixel_offset]) {
                m_pixbufPreviousPass = FALSE;
                return TRUE;
            }
        }
    }

    m_pixbufPreviousPass = FALSE;
    return FALSE;
}

