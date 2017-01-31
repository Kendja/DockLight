
#include "Launcher.h"
#include "DockItem.h"
#include "gio/gdesktopappinfo.h"
#include <map>



namespace Launcher
{
    std::map<std::string, std::string> dictionary;
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

    std::string getTitleNameFromDesktopFile(std::string desktopfile)
    {
        if (desktopfile.empty())
            return "";

        if (desktopfile == "untitled window")
            return "";

        if (desktopfile == "wine")
            return "";

        // build the desktop file path 
        desktopfile = Utilities::stringToLower(desktopfile.c_str());
        std::replace(desktopfile.begin(), desktopfile.end(), ' ', '-');

        if (dictionary.count(desktopfile) == 1) {
            return dictionary.at(desktopfile);
        }

        char filepath[PATH_MAX];
        sprintf(filepath, "/usr/share/applications/%s.desktop", desktopfile.c_str());

        GError *error = NULL;
        GKeyFile *key_file = g_key_file_new();

        gboolean found = g_key_file_load_from_file(key_file,
                filepath, GKeyFileFlags::G_KEY_FILE_NONE, &error);

        if (!found) {
            if (error) {
                g_warning("Desktop file not found  %s %s",
                        desktopfile.c_str(), error->message);
                g_error_free(error);
                error = NULL;
            }

            dictionary[desktopfile] = "";
            return "";
        }

        // check if the Icon Desktop Entry name exists
        gchar* titlename = g_key_file_get_string(key_file,
                "Desktop Entry", "Name", &error);

        if (titlename == NULL) {
            if (error) {
                g_warning("Name Desktop Entry found: %s %s", titlename, error->message);
                g_error_free(error);
                error = NULL;
            }
            dictionary[desktopfile] = "";
            return "";
        }

        dictionary[desktopfile] = titlename;
        g_key_file_free(key_file);

        return titlename;
    }

    std::string getTitleNameFromDesktopFile(std::string desktopfile, std::string desktopfile2)
    {
        std::string result = getTitleNameFromDesktopFile(desktopfile);
        if (result == "")
            result = getTitleNameFromDesktopFile(desktopfile2);

        return result;
    }
}