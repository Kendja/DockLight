/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Configuration.h
 * Author: yoo
 *
 * Created on February 7, 2017, 7:15 PM
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H


namespace Configuration
{

    struct Color
    {
        Color();
        Color(const double red, const double green, const double blue, const double alpha);
        void set(const double red, const double green, const double blue, const double alpha);

        double red = 1.0;
        double green = 1.0;
        double blue = 1.0;
        double alpha = 1.0;
    };

    class ColorWindow
    {
    public:

        ColorWindow();
        ColorWindow(const bool enabled, const Color background, const Color foreground,
                const double lineWith, const double roundedRatious);

        void set(const bool enabled, const Color background, const Color foreground,
                const double m_lineWith, const double m_roundedRatious);

        bool enabled()
        {
            return m_enabled;
        }

        Color background()
        {
            return m_background;
        }

        Color foreground()
        {
            return m_foreground;
        }

        double lineWith()
        {
            return m_lineWith;
        }

        double roundedRatious()
        {
            return m_roundedRatious;
        }
    private:
        Color m_background;
        Color m_foreground;
        bool m_enabled = false;
        double m_lineWith = 1.0;
        double m_roundedRatious = 2.0;


    };

    class Theme
    {
    public:
        Theme();

        ColorWindow& forWindow()
        {
            return *m_window;
        }

        ColorWindow& forPanel()
        {
            return *m_panel;
        }

        ColorWindow& forPanelTitle()
        {
            return *m_panelTitle;
        }

        ColorWindow& forPanelTitleText()
        {
            return *m_panelTitleText;
        }

        ColorWindow& forPanelSelector()
        {
            return *m_panelSelector;
        }

        ColorWindow& forPreview()
        {
            return *m_preview;
        }

        ColorWindow& forPreviewTitle()
        {
            return *m_previewTitle;
        }
        
        
        ColorWindow& forPreviewTitleText()
        {
            return *m_previewTitleText;
        }
        
        ColorWindow& forPreviewSelector()
        {
            return *m_previewSelector;
        }
        
        ColorWindow& forPreviewSelectorClose()
        {
            return *m_previewSelectorClose;
        }
        
        bool panelScaleOnhover()
        {
            return m_panelScaleOnhover;
        }

    private:
        ColorWindow* m_window;

        ColorWindow* m_panel;
        ColorWindow* m_panelTitle;
        ColorWindow* m_panelTitleText;

        ColorWindow* m_panelSelector;

        ColorWindow* m_preview;
        ColorWindow* m_previewTitle;
        ColorWindow* m_previewTitleText;
        ColorWindow* m_previewSelector;
        ColorWindow* m_previewSelectorClose;
        

        bool m_panelScaleOnhover = true;

    };


    Theme getTheme();

}




#endif /* CONFIGURATION_H */

