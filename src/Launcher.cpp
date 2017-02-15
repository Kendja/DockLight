
#include "Launcher.h"
#include "DockItem.h"
#include "gio/gdesktopappinfo.h"
#include <map>
#include <glibmm/i18n.h>


namespace Launcher
{
    std::map<std::string, std::string> dictionary = {
        {"Gpk-update-viewer", _("Gpk-update-viewer")},
        {"Gpk-application", _("Gpk-application")}
       
        
    };

    /**
     * Launch an application from a desktop file or from bash.
     * @param DockItem* item
     */
    bool Launch(DockItem* item)
    {
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

    std::string getWindowTitle(WnckWindow *window)
    {
        const char* _instancename = wnck_window_get_class_instance_name(window);
        if (_instancename == NULL) {
            return "";
        }
        const std::string extensions[] = {".py", ".exe", ".sh"};
        std::string instanceName(_instancename);
        instanceName = Utilities::removeExtension(instanceName, extensions);
        std::replace(instanceName.begin(), instanceName.end(), ' ', '-');

        if (dictionary.count(instanceName) == 1) {
            std::string value = dictionary[instanceName];
            if (value.empty())
                return "";

            return gettext(value.c_str());
        }

        return "";

    }

    gboolean getDesktopFile(GKeyFile *key_file, std::string& desktopFileName)
    {
        GError *error = NULL;

        char filepath[PATH_MAX];
        sprintf(filepath, "/usr/share/applications/%s.desktop", desktopFileName.c_str());

        gboolean found = g_key_file_load_from_file(key_file,
                filepath, GKeyFileFlags::G_KEY_FILE_NONE, &error);

        if (error) {
            g_error_free(error);
            error = NULL;
        }

        if (found)
            return TRUE;

        sprintf(filepath, "/usr/local/share/applications/%s.desktop", desktopFileName.c_str());
        found = g_key_file_load_from_file(key_file,
                filepath, GKeyFileFlags::G_KEY_FILE_NONE, &error);

        if (error)
            g_error_free(error);

        return found;
    }

    std::string getTitleNameFromDesktopFile(std::string appname)
    {
        if (appname.empty())
            return "";

        if (appname == "untitled window")
            return "";

        if (appname == "wine")
            return "";


        std::replace(appname.begin(), appname.end(), ' ', '-');

        if (dictionary.count(appname) == 1) {
            std::string value = dictionary[appname];
            if (value.empty())
                return appname;

            std::string s = gettext(value.c_str());
            return s;
        }

        // build the desktop file path 
        std::string appnameLowercase = Utilities::stringToLower(appname.c_str());
        std::string desktopfile = appname;
        std::string desktopfile_Lowercase = appnameLowercase;
        std::string desktopfile_orgGnome = "org.gnome." + appname;
        std::string desktopfile_orgGnomeLowercase = "org.gnome." + appnameLowercase;

        // trying to find a desktop file.
        GError *error = NULL;
        gchar* titlename = NULL;
        GKeyFile *key_file = g_key_file_new();
        gboolean desktopFilefound = FALSE;

        desktopFilefound = getDesktopFile(key_file, desktopfile);
        if (desktopFilefound == FALSE)
            desktopFilefound = getDesktopFile(key_file, desktopfile_Lowercase);
        if (desktopFilefound == FALSE)
            desktopFilefound = getDesktopFile(key_file, desktopfile_orgGnome);
        if (desktopFilefound == FALSE)
            desktopFilefound = getDesktopFile(key_file, desktopfile_orgGnomeLowercase);

        if (desktopFilefound) {

            std::string langNameKey = "Name";
            std::string genericName = "";
            char* elang = getenv("LANG");
            if (elang != NULL) {
                std::string eviromentlang = elang;
                std::string lang = eviromentlang.substr(0, 2);

                if (lang != "en") {
                    langNameKey = "Name[" + lang + "]";
                    genericName = "GenericName[" + lang + "]";
                }

            }

            // check if the Icon Desktop Entry name exists
            titlename = g_key_file_get_string(key_file,
                    "Desktop Entry", langNameKey.c_str(), &error);

            if (error) {
                g_error_free(error);
                error = NULL;
            }

            if (titlename == NULL && !genericName.empty()) {
                titlename = g_key_file_get_string(key_file,
                        "Desktop Entry", genericName.c_str(), &error);

                if (error) {
                    g_error_free(error);
                    error = NULL;
                }

            }

            if (titlename == NULL) {
                titlename = g_key_file_get_string(key_file,
                        "Desktop Entry", "Name", &error);

                if (error)
                    g_error_free(error);
            }

        }

        if (titlename == NULL) {
            dictionary[appname] = appname;
            g_print("Application (%s) without desktop launcher\n", appname.c_str());
        } else {
            dictionary[appname] = titlename;
        }

        g_key_file_free(key_file);


        return dictionary[appname];

    }

    std::string getTitleNameFromDesktopFile(std::string desktopfile, std::string desktopfile2)
    {
        std::string result = getTitleNameFromDesktopFile(desktopfile);
        if (result == "")
            result = getTitleNameFromDesktopFile(desktopfile2);

        return result;
    }
}