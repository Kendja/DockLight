
#include "Launcher.h"
#include "DockItem.h"
#include "gio/gdesktopappinfo.h"

namespace Launcher
{

    void Launch(DockItem* item)
    {
        std::string lowerrealgroupname(Utilities::stringToLower(item->m_realgroupname.c_str()));

        // Extract  desktop file.
        std::size_t foundspace = lowerrealgroupname.find(" ");
        if (foundspace > 0) {
            std::string s = lowerrealgroupname;
            std::replace(s.begin(), s.end(), ' ', '-'); // replace all ' ' to '-'
            std::string msg = "Expected Desktop file : \n" + s + "\n";
            g_print(msg.c_str());

            lowerrealgroupname = s;

        }

        char command[PATH_MAX];
        *command = 0;

       // GDesktopAppInfo *desktop_info;
        GError *error = NULL;
        GAppLaunchContext *context;
        GAppInfo *app_info;

        sprintf(command, "%s.desktop", lowerrealgroupname.c_str());
        app_info = (GAppInfo*) g_desktop_app_info_new(command);
        if (app_info == NULL) {
            g_warning("Launcher could not locate: %s\n", command);
            sprintf(command, "\"%s\"", lowerrealgroupname.c_str());
            if (g_spawn_command_line_async(command, &error) == FALSE) {
                if (error) {
                    g_warning("Launcher could not execute: %s: Error: %s\n", command, error->message);
                    g_error_free(error);
                }
            }
            return;
        }

        GdkDisplay *display = gdk_display_get_default();
        context = (GAppLaunchContext*) gdk_display_get_app_launch_context(display);
        g_app_info_launch(app_info, NULL, context, &error);
        if (error) {
            g_warning("Failed to launch %s: Error: %s", command, error->message);
            g_error_free(error);
        }
        g_object_unref(app_info);
        g_object_unref(context);
        
    }
}