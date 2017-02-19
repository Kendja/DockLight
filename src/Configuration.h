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
        
        void setPanelScaleOnhover(bool value)
        {
            m_panelScaleOnhover=value;
        }
        
        int getPanelBinaryValue()
        {
            return m_panelLinesBinaryValue;
        }
        
        int getPanelSelectorBinaryValue()
        {
            return m_panelSelectorLinesBinaryValue;
        }
        
        void setPanelSelectorBinaryValue(int value)
        {
            m_panelSelectorLinesBinaryValue = value;
        }
              
        
        int getPreviewBinaryValue()
        {
            return m_previewLinesBinaryValue;
        }
        
        void setPanelBinaryValue(int value)
        {
            m_panelLinesBinaryValue = value;
        }
        
        
        void setPreviewBinaryValue(int value)
        {
            m_previewLinesBinaryValue = value;
        }
        
        
        void setPreviewSelectorBinaryValue(int value)
        {
            m_previewSelectorLinesBinaryValue = value;
        }
        
         int getPreviewSelectorBinaryValue()
        {
            return m_previewSelectorLinesBinaryValue;
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
        
        
        int m_panelLinesBinaryValue = 0;
        int m_panelSelectorLinesBinaryValue = 0;
        int m_previewSelectorLinesBinaryValue =0;
        int m_previewLinesBinaryValue = 0;
        
        
        

    };


    Theme getTheme();
    void Load();
    void setAutohide(bool value);
    bool getAutohide();
    void setAutohide(bool value);
        
}




#endif /* CONFIGURATION_H */

