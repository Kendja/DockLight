/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Configuration.h"


namespace Configuration
{

    Theme m_theme;

    Color::Color()
    {
    };

    Color::Color(const double red, const double green, const double blue, const double alpha)
    {
        set(red, green, blue, alpha);
    }

    void Color::set(const double red, const double green, const double blue, const double alpha)
    {
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->alpha = alpha;

    }

    ColorWindow::ColorWindow(){}
    
    ColorWindow::ColorWindow(const bool enabled, const Color background, const Color foreground,
            const double lineWith, const double roundedRatious)
    {
        this->set(enabled, background, foreground, lineWith, roundedRatious);
    }

    void ColorWindow::set(const bool enabled, const Color background, const Color foreground,
            const double lineWith, const double roundedRatious)
    {
        
        this->m_enabled = enabled;
        this->m_background = background;
        this->m_foreground = foreground;
        this->m_lineWith = lineWith;
        this->m_roundedRatious = roundedRatious;
    }

    /**
     * Constructs the default theme
     */
    Theme::Theme()
    {
        Color backColor(0.27, 0.44, 0.58, 1.0);
        Color foreColor(1.0, 1.0, 1.0, 1.0);
        
        Color backSelector(1.9,1.0,1.0,0.3);
        Color foreSelector(1.1,1.1,1.0,1.0);
        
        
        
        this->m_window = new ColorWindow();
        this->m_panel =  new ColorWindow(true,backColor,foreColor,0.7, 4.0 );
        this->m_panelSelector =  new ColorWindow(true,backSelector,foreSelector,1.5, 4.0 );
        this->m_panelTitle = new ColorWindow(true,backColor,foreColor,0.0, 4.0 );
        this->m_panelTitleText = new ColorWindow(true,Color(),Color(1.0,1.0,1.0,1.0),0.0, 0.0 );
        
        this->m_preview = new ColorWindow(true,backColor,foreColor,0.7, 4.0 );
        this->m_previewTitle = new ColorWindow(true,Color(0.0,0.0,1.0,0.0),Color(1.0,1.0,1.0,0.0),1.0, 0.0 );
        this->m_previewTitleText = new ColorWindow(true,Color(),Color(),0.0, 0.0 );
        this->m_previewSelector = new ColorWindow(true,backSelector,foreSelector,2.0, 4.0 );
        this->m_previewSelectorClose = new ColorWindow(true,Color(0.0, 0.50, 0.0,1.0),foreColor,4.0, 4.0 );
        
    }

    Theme getTheme()
    {
        return m_theme;
    };
}