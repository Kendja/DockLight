#include "WindowControl.h"



namespace WindowControl
{

    void HomeCloseAllWindows()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, INSTANCENAME) == 0) {
                continue;
            }


            wnck_window_close(window, gtk_get_current_event_time());
        }
    }

    void HomeCloseAllExceptActive()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            if (wnck_window_is_active(window))
                continue;

            int ct = gtk_get_current_event_time();
            wnck_window_close(window, (guint32) ct);
        }
    }

    void HomeMinimizeAllExceptActive()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            if (wnck_window_is_active(window))
                continue;

            wnck_window_minimize(window);
        }
    }

    void HomeMinimizeAll()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            wnck_window_minimize(window);
        }
    }

    void HomeUnMinimizeAll()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            wnck_window_unminimize(window, gtk_get_current_event_time());
        }
    }

    void ActivateWindow(WnckWindow* window)
    {
        if (window == NULL)
            return;

        int ct = gtk_get_current_event_time();

        wnck_window_activate(window, ct);

        if (wnck_window_is_minimized(window))
            wnck_window_unminimize(window, ct);

        wnck_window_activate_transient(window, ct);
    }

    bool isExistsUnMaximizedWindowsByDockItem(DockItem* dockitem)
    {
        bool result = false;
        for (auto item : dockitem->m_items) {
            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            if (wnck_window_is_minimized(window) == FALSE) {
                result = true;

                break;
            }

        }
        
        return result;
    }
    
    bool isExistsMinimizedWindowsByDockItem(DockItem* dockitem)
    {
        bool result = false;
        for (auto item : dockitem->m_items) {
            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            if (wnck_window_is_minimized(window) == TRUE) {
                result = true;

                break;
            }

        }
        
        return result;
    }

    void minimizeAllByDockItem(DockItem* dockitem)
    {
        for (auto item : dockitem->m_items) {

            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            if (wnck_window_is_minimized(window) == false)
                wnck_window_minimize(window);
        }
    }

    void unMinimizeAllByDockItem(DockItem* dockitem)
    {
        for (auto item : dockitem->m_items) {

            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            wnck_window_unminimize(window, gtk_get_current_event_time());
        }
    }

    void unminimizeAllByDockItem(DockItem* dockitem)
    {
        for (auto item : dockitem->m_items) {
            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            if (wnck_window_is_minimized(window))
                wnck_window_unminimize(window, gtk_get_current_event_time());
        }
    }

    int windowscount()
    {
        int count = 0;
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, INSTANCENAME) == 0) {
                continue;
            }

            count++;
        }

        return count;
    }

    int minimizedWindowscount()
    {
        int count = 0;
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, INSTANCENAME) == 0) {
                continue;
            }

            if (wnck_window_is_minimized(window))
                count++;
        }

        return count;
    }

    int unMinimizedWindowsCount()
    {
        return windowscount() - minimizedWindowscount();
    }
}

