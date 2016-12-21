#ifndef PREVIEW_H
#define PREVIEW_H

#include <gtkmm.h>
#include "DockItem.h"

class DockPanel;

class Preview : public Gtk::Window
{
public:
    Preview();
    virtual ~Preview();
    void hideMe();
    void setOwner(DockPanel& dockpanel)
    {
        m_dockpanelReference = &dockpanel;
    };
    void setDockItem(DockItem* item);

protected:
    //Override default signal handler:
    // Signal handlers:
    // Note that on_draw is actually overriding a virtual function
    // from the Gtk::Window class. I set it as virtual here in case
    // someone wants to override it again in a derived class.
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    bool on_timeoutDraw();

    bool on_motion_notify_event(GdkEventMotion*event);
    bool on_scroll_event(GdkEventScroll *event);
    bool on_button_press_event(GdkEventButton *event);
   
private:
    bool m_mouseIn;
    bool m_isActive;
    int m_currentIndex;
    std::string m_title;
    DockPanel* m_dockpanelReference;
    DockItem* m_dockItemReference;
    Glib::Timer m_timer;
    Pango::FontDescription font;

    int getIndex(int x, int y);
};

#endif /* PREVIEW_H */

