//*****************************************************************
//
//  Copyright (C) 2016 Juan R. Gonzalez
//  Created on December, 2016 
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

#include "IconLoader.h"
#include "Defines.h"
#include "Utilities.h"

#include <gtkmm/icontheme.h>

namespace IconLoader
{
    // https://developer.gnome.org/gtk3/stable/GtkIconTheme.html#GtkIconLookupFlags
    // enum GtkIconLookupFlags
    // Used to specify options for gtk_icon_theme_lookup_icon()
    GtkIconLookupFlags flags = GTK_ICON_LOOKUP_GENERIC_FALLBACK;

    // needs to keep the icon around.
    GdkPixbuf *referenceIcon = nullptr;

    // the desired icon size. 
    // The resulting icon may not be exactly this size.
    gint size = 64;

    /**
     * Get the window icons as Gdk::Pixbuf.
     * Icons and themes are looked for in a set of directories. 
     * By default, apps should look in $HOME/.icons (for backwards compatibility), 
     * in $XDG_DATA_DIRS/icons and in /usr/share/pixmaps,/usr/share/icons (in that order).
     * https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html

     * @param window
     * @return Glib::RefPtr<Gdk::Pixbuf>
     */
    Glib::RefPtr<Gdk::Pixbuf> GetWindowIcon(WnckWindow* window)
    {
        return GetWindowIcon(window, referenceIcon);
    }

    /**
     * Get the window icons as Gdk::Pixbuf.
     * Icons and themes are looked for in a set of directories. 
     * By default, apps should look in $HOME/.icons (for backwards compatibility), 
     * in $XDG_DATA_DIRS/icons and in /usr/share/pixmaps,/usr/share/icons (in that order).
     * https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html

     * @param window
     * @param icon The caller should reference the returned GdkPixbuf if it needs to keep the icon around.
     * @return Glib::RefPtr<Gdk::Pixbuf>
     */
    Glib::RefPtr<Gdk::Pixbuf> GetWindowIcon(WnckWindow* window, GdkPixbuf *icon)
    {
        Glib::RefPtr<Gdk::Pixbuf> result = NULLPB;
        GError *error = NULL;

        // Gets the icon name of window , as it should be displayed for an icon (minimized state). 
        // Always returns some value, even if window has no icon name set. 
        std::string iconname = wnck_window_get_icon_name(window);
        //Convert to lower case to get the icon name correctly.
        std::string lowerName = Utilities::stringToLower(iconname.c_str());

        // Get the icon name from wnck_window_get_icon_name(window) are far from perfect ;o(
        // We trying here to get the icon name that can be found in the theme.
        const char* _bettername = wnck_window_get_class_group_name(window);
        //const char* _bettername = wnck_window_get_class_instance_name(window);
        if (_bettername != NULL) {
          
            std::string bettername(Utilities::stringToLower(_bettername));
            std::size_t foundspace = lowerName.find(" ");
            if (foundspace > 0 && bettername != "wine") {
                lowerName = bettername;
            }
        }

        
        GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
        
        //auto icon_theme2 = Gtk::IconTheme::get_default() ;	//TODO use this instead of GtkIconTheme
        //icon_theme2->set_custom_theme( "gnome" );
        
        //https://code.launchpad.net/~ted/libappindicator/lp875770/+merge/95685
        /*
        gchar **path;
        gint n_elements,i;
        gtk_icon_theme_get_search_path(icon_theme,&path,&n_elements);
        for (i=0; i< n_elements || path[i] == NULL; i++) {
            std::string p( path[i] );
        }
        g_strfreev (path);        
        */
        //gtk_icon_
        //Glib::RefPtr<IconTheme> ttt = Gtk::IconTheme::get_default() ;	
	
        //tt->set_custom_theme("Adwaita");
        //const Glib::ustring themename = "Adwaita";
        //Gtk::IconTheme::set_
      // tt->load_icon()
        
                
    //Sets the name of the icon theme that the Gtk::IconTheme object uses overriding system configuration. 
    
    //Gtk::IconTheme::set_custom_theme(themename);	

        
        
        if (icon_theme != NULL && gtk_icon_theme_has_icon(icon_theme, lowerName.c_str())) {

            icon = gtk_icon_theme_load_icon(icon_theme, lowerName.c_str(),
                    size, flags, &error);
            if (icon == NULL)
                icon = wnck_window_get_icon(window);
        } else {

            // Gets the icon to be used for window.
            // If no icon was found, a fallback icon is used. 
            // wnck_window_get_icon_is_fallback() 
            // can be used to tell if the icon is the fallback icon.
            icon = wnck_window_get_icon(window);

            // check if the icon is the fallback icon.
            if (wnck_window_get_icon_is_fallback(window)) {
                g_warning("isfallback none icon was set on window: (%s)", lowerName.c_str());
                g_print("trying to get the icon from a desktop file: (%s)", lowerName.c_str());
                result = GetWindowIconFromDesktopFile(window, icon);
            }
        }

        
        if (result == NULLPB)
            result = PixbufConvert(icon);
       
        return result;

    }

    /**
     * Get the window icon from a desktop file
     * located at /usr/share/applications
     * @param window
     * @param icon The caller should reference the GdkPixbuf if it needs to keep the icon around.
     * @return Glib::RefPtr<Gdk::Pixbuf> 
     */
    Glib::RefPtr<Gdk::Pixbuf> GetWindowIconFromDesktopFile(WnckWindow* window, GdkPixbuf *icon)
    {
        // Get the possible desktop filename.
        const char* _desktopfilename = wnck_window_get_class_group_name(window);
        if (_desktopfilename == NULL) {
            return NULLPB;
        }
        // replace replace all ' ' to '-' 
        std::string desktopfilename(Utilities::stringToLower(_desktopfilename));
        std::size_t foundspace = desktopfilename.find(" ");
        if (foundspace > 0) {
            std::string s = desktopfilename;
            std::replace(s.begin(), s.end(), ' ', '-');
            desktopfilename = s;
        }

        // build the desktop file path 
        char filepath[PATH_MAX];
        sprintf(filepath, "/usr/share/applications/%s.desktop",
                desktopfilename.c_str());

        // https://gist.github.com/zdxerr/709169
        // check if the file exists
        GError *error = NULL;
        GKeyFile *key_file = g_key_file_new();
        gboolean found = g_key_file_load_from_file(key_file,
                filepath, GKeyFileFlags::G_KEY_FILE_NONE, &error);

        if (!found) {
            if (error) {
                g_warning("Desktop file not found  %s %s",
                        desktopfilename.c_str(), error->message);
                g_error_free(error);
            }

            return NULLPB;
        }

        // check if the Icon Desktop Entry name exists
        gchar* iconname = g_key_file_get_string(key_file,
                "Desktop Entry", "Icon", &error);

        if (iconname == NULL) {
            if (error) {
                g_warning("Icon Desktop Entry found: %s %s", iconname, error->message);
                g_error_free(error);
            }

            return NULLPB;
        }

        GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
        if (icon_theme != NULL && gtk_icon_theme_has_icon(icon_theme, iconname)) {

            icon = gtk_icon_theme_load_icon(icon_theme, iconname,
                    size, flags, &error);
            if (icon == NULL)
                icon = wnck_window_get_icon(window);

        } else {
            // Gets the icon to be used for window.
            // If no icon was found, a fallback icon is used. 
            // wnck_window_get_icon_is_fallback() 
            // can be used to tell if the icon is the fallback icon.
            icon = wnck_window_get_icon(window);
        }

        return PixbufConvert(icon);
    }

    /**
     * Converts a GdkPixbuf to a Glib::RefPtr<Gdk::Pixbuf>.
     * @param icon
     * @return 
     */
    Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* icon)
    {
        Glib::RefPtr<Gdk::Pixbuf> result = NULLPB;

        int width = gdk_pixbuf_get_width(icon);
        int height = gdk_pixbuf_get_height(icon);
        int stride = gdk_pixbuf_get_rowstride(icon);
        gboolean hasalpha = gdk_pixbuf_get_has_alpha(icon);
        int bitsaple = gdk_pixbuf_get_bits_per_sample(icon);
        guint8 *pdata;
        guchar *pixels = gdk_pixbuf_get_pixels(icon);

        pdata = static_cast<guint8*> (pixels);

        result = Gdk::Pixbuf::create_from_data(pdata,
                Gdk::COLORSPACE_RGB, hasalpha, bitsaple,
                width, height, stride);

        return result;
    }
}


