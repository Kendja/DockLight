//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 12:17 PM 
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

#ifndef DEFINES_H
#define DEFINES_H



#define APPNAME "DockLight"
#define NULLPB (Glib::RefPtr<Gdk::Pixbuf>)NULL

typedef enum Window_action_t
{
    OPEN,
    CLOSE
} Window_action;

typedef enum panel_location_t
{
    TOP,
    BOTTOM
} panel_locationType;

typedef enum StrutsPosition_t
{
    Left = 0,
    Right = 1,
    Top = 2,
    Bottom = 3,
    LeftStart = 4,
    LeftEnd = 5,
    RightStart = 6,
    RightEnd = 7,
    TopStart = 8,
    TopEnd = 9,
    BottomStart = 10,
    BottomEnd = 11
} strutsPosition;

#define DEF_ICONFILE    "docklight.ico"

#define DEF_CELLWIDTH 58                    // Cell Width
#define DEF_CELLHIGHT 62                    // Cell Height
#define DEF_CELLTOPMARGIN 6                 // Cell Top Position
#define DEF_ICONSIZE 48                     // Icon Size Width and height
#define DEF_MINCONSIZE 20                  // Minimun Icon Size allowed
#define DEF_ICONTOPMARGIN 10                // Icon top Margin from panel

// Icon top Margin from the cell
#define DEF_ICONTOPCELLMARGIN DEF_ICONTOPMARGIN -DEF_CELLTOPMARGIN                



#define DEF_PANELHIGHTHIDE  4                 
#define DEF_PANELHIGHT 72 
#define DEF_PREVIEW_WIDTH 220               // the preview popup window width
#define DEF_PREVIEW_HEIGHT 200              // the preview popup window height
#define DEF_PREVIEW_MINHEIGHT 112           // the minimun preview height
#define DEF_PREVIEW_LEFT_MARGING 14         // the preview left marging 
#define DEF_PREVIEW_RIGHT_MARGING 30        // the preview right marging 
#define DEF_PREVIEW_SCALE_WIDTH_OFFSET  12  // the offset for the scale method
#define DEF_PREVIEW_SCALE_HEIGHT_OFFSET 54  // the offset for the scale method
#define DEF_PREVIEW_PIXBUF_TOP 36       // the image pixbuf top


#endif /* DEFINES_H */

