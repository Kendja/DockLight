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

#include <string.h>
#include "Launcher.h"
#include "DockItem.h"
#include "gio/gdesktopappinfo.h"
#include <map>
#include <glibmm/i18n.h>
#include "MoReader.h"

namespace Launcher {
    std::map<std::string, std::string> dictionary = {
        {"Gpk-update-viewer", _("Gpk-update-viewer")},
        {"Gpk-application", _("Gpk-application")}

    };

    /**
     * Launch an application from a desktop file or from bash.
     * @param DockItem* item
     */
    bool Launch(DockItem* item) {
        std::string lowerrealgroupname(Utilities::stringToLower(item->m_realgroupname.c_str()));

        // Extract  desktop file.
        std::size_t foundspace = lowerrealgroupname.find(" ");
        if (foundspace > 0) {
            std::string s = lowerrealgroupname;
            std::replace(s.begin(), s.end(), ' ', '-'); // replace all ' ' to '-'
            std::string msg = "Expected Desktop file : \n" + s + "\n";
#pragma GCC diagnostic ignored "-Wformat-security"
            g_print(msg.c_str());

            lowerrealgroupname = s;
        }

        char command[PATH_MAX];
        *command = 0;

        // GDesktopAppInfo *desktop_info;
        GError *error = NULL;
        GAppLaunchContext *context = NULL;
        GAppInfo *app_info = NULL;

        sprintf(command, "%s.desktop", lowerrealgroupname.c_str());
        app_info = (GAppInfo*) g_desktop_app_info_new(command);
        if (app_info == NULL) {
            g_warning("Launcher could not locate desktop file : %s\n", command);
            sprintf(command, "\"%s\"", lowerrealgroupname.c_str());
            // Launch from command line
            if (g_spawn_command_line_async(command, &error)) {
                return true;
            }

            if (error) {
                g_warning("Launcher could not execute: %s: Error: %s\n", command, error->message);
                g_error_free(error);
                error = NULL;
            }

        } else {
            // Launch desktop file
            GdkDisplay *display = gdk_display_get_default();
            context = (GAppLaunchContext*) gdk_display_get_app_launch_context(display);
            if (g_app_info_launch(app_info, NULL, context, &error)) {

                g_object_unref(app_info);
                g_object_unref(context);

                return true;
            }
            if (error) {
                g_warning("Failed to launch desktop file %s: Error: %s", command, error->message);
                g_error_free(error);
                error = NULL;
            }

            g_object_unref(app_info);
            if (context != NULL)
                g_object_unref(context);
        }

        g_critical("Launcher not found: %s.desktop\n", lowerrealgroupname.c_str());
        return false;
    }

    gboolean getDesktopFile(GKeyFile *key_file, const char* appname) {

        if (appname == NULL)
            return FALSE;

        if (appname == "Untitled Window")
            return FALSE;

        if (appname == "untitled window")
            return FALSE;

        if (appname == "Wine")
            return FALSE;


        GError *error = NULL;

        std::string name(appname);
        std::string lowername = Utilities::stringToLower(appname);

        const std::string desktopfilename[] = {
            {appname},
            {lowername},
            {"org.gnome." + name},
            {"org.gnome." + lowername}
        };

        const std::string filelocaltion[] = {
            {"/usr/share/applications/"},
            {"/usr/local/share/applications/"}
        };

        char filepath[PATH_MAX];
        for (auto p : filelocaltion) {
            for (auto f : desktopfilename) {
                sprintf(filepath, "%s%s.desktop", p.c_str(), f.c_str());

                gboolean found = g_key_file_load_from_file(key_file,
                        filepath, GKeyFileFlags::G_KEY_FILE_NONE, &error);

                if (error) {
                    g_error_free(error);
                    error = NULL;
                    continue;
                }

                if (found)
                    return TRUE;

            }
        }

        return true;
    }

    std::string getTitleNameFromDesktopFile(const std::string appname) {

        std::string theappname(appname);
        std::replace(theappname.begin(), theappname.end(), ' ', '-');

        if (dictionary.count(theappname) == 1) {
            std::string value = dictionary[theappname];
            if (value.empty())
                return theappname;

            return gettext(value.c_str());
        }

        GError *error = NULL;
        GKeyFile *key_file = g_key_file_new();
        const char* _appname = theappname.c_str();
        if (!getDesktopFile(key_file, _appname)) {
            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        char* elang = getenv("LANG");
        if (elang == NULL) {
            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        std::string lang(elang);
        lang = lang.substr(0, 2);
        if (lang == "en") {
            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }


        // check if the GenericName Entry exists
        std::string langNameKey = "GenericName[" + lang + "]";
        char* titlename = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (titlename != NULL) {
            dictionary[theappname] = titlename;
            g_key_file_free(key_file);
            return titlename;
        }

        if (error) {
            g_error_free(error);
            error = NULL;
        }

        // check if the Name Entry exists
        langNameKey = "Name[" + lang + "]";
        titlename = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (titlename != NULL) {
            dictionary[theappname] = titlename;
            g_key_file_free(key_file);
            return titlename;
        }

        if (error) {
            g_error_free(error);
            error = NULL;
        }


        // check if the dektop file is a Domain file 
        langNameKey = "X-Ubuntu-Gettext-Domain";
        char* mofile = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (mofile == NULL) {
            if (error) {
                g_error_free(error);
                error = NULL;
            }

            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        langNameKey = "X-GNOME-FullName";
        char* xFullname = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (xFullname == NULL) {
            if (error) {
                g_error_free(error);
                error = NULL;
            }
        }

        langNameKey = "GenericName";
        char* xGenericName = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (xGenericName == NULL) {
            if (error) {
                g_error_free(error);
                error = NULL;
            }
        }

        langNameKey = "Name";
        char* xName = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (xName == NULL) {
            if (error) {
                g_error_free(error);
                error = NULL;
            }
        }

        MoReader* mo = new MoReader();
        std::string moFilePath("/usr/share/locale-langpack/" +
                lang + "/LC_MESSAGES/" + mofile + ".mo");

        if (mo->Load(moFilePath.c_str()) < 0) {

            dictionary[theappname] = theappname;
            g_key_file_free(key_file);

            delete mo;
            return theappname;

        }

        std::string xFullnameTranslatedText = "";
        std::string xGenericNameTranslatedText = "";
        std::string xNameTranslatedText = "";


        if (xFullname != NULL)
            xFullnameTranslatedText = mo->getText(xFullname);

        if (xGenericName != NULL)
            xGenericNameTranslatedText = mo->getText(xGenericName);

        if (xName != NULL)
            xNameTranslatedText = mo->getText(xName);


        if (!xFullnameTranslatedText.empty() &&
                xFullnameTranslatedText != theappname) {

            dictionary[theappname] = xFullnameTranslatedText;
            g_key_file_free(key_file);
            delete mo;
            return dictionary[theappname];

        }

        if (!xGenericNameTranslatedText.empty() &&
                xGenericNameTranslatedText != theappname) {

            dictionary[theappname] = xGenericNameTranslatedText;
            g_key_file_free(key_file);
            delete mo;
            return dictionary[theappname];

        }

         if (!xNameTranslatedText.empty() &&
                xNameTranslatedText != theappname) {

            dictionary[theappname] = xNameTranslatedText;
            g_key_file_free(key_file);
            delete mo;
            return dictionary[theappname];

        }
       
        dictionary[theappname] = theappname;
        g_key_file_free(key_file);
        delete mo;

        return dictionary[theappname];

    }

    std::string getTitleNameFromDesktopFile(std::string desktopfile, std::string desktopfile2) {
        std::string result = getTitleNameFromDesktopFile(desktopfile);
        if (result == "")
            result = getTitleNameFromDesktopFile(desktopfile2);

        return result;
    }
}