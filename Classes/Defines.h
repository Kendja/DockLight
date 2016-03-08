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
#define	DEFINES_H

// Enum constants must be carefully named to avoid name collisions 

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



#define	DEF_ICONSIZE 48
#define	DEF_CELLSIZE 58//48
#define	DEF_PANELHIGHT 72 //68
#define	DEF_PANELBCKHIGHT 62 
#define	DEF_PANELBCKTOP 6 
#define	DEF_OFFSE_TOP 10
#define	DEF_PREVIEW_WIDTH 220           // the preview popup window width
#define	DEF_PREVIEW_HEIGHT 200          // the preview popup window height
#define	DEF_PREVIEW_PIXBUF_WIDTH 210    // the image pixbuf width
#define	DEF_PREVIEW_PIXBUF_HEIGHT 140   // the image pixbuf height
#define	DEF_PREVIEW_PIXBUF_TOP 36       // the image pixbuf top
#define	DEF_UTF8MAX  512                // the max buff used by g_utf8_strncpy   
#define	DEF_FRAME_DELAY  50             

        
#endif	/* DEFINES_H */

