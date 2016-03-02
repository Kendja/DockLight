/* 
 * File:   Utilities.h
 * Author: yoo
 *
 * Created on February 22, 2016, 4:40 PM
 */

#ifndef UTILITIES_H
#define	UTILITIES_H

#include <string>
#include <algorithm>
#include <gtk/gtk.h> 	// user c version for samples
#include <gtkmm.h> 	// remove this include
#include <math.h>

class Utilities
{
public:
    Utilities();
    Utilities(const Utilities& orig);
    virtual ~Utilities();


    static std::string getExecPath();
    static std::string getExecPath(const std::string str);
    static std::vector<std::string> split(const std::string &text, char sep);

    static void RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr,
            double x, double y, double width, double height, double radius)
    {
        // our radius can be no larger than half our height or width
        radius = std::min(radius, std::min(width / 2, height / 2));
        cr->move_to(x + radius, y);
        cr->arc(x + width - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
        cr->arc(x + width - radius, y + height - radius, radius, 0, M_PI * .5);
        cr->arc(x + radius, y + height - radius, radius, M_PI * .5, M_PI);
        cr->arc(x + radius, y + radius, radius, M_PI, M_PI * 1.5);

    }


    //    public static void RoundedRectangle (this Context self, double x, double y, double width, double height, double radius)
    //		{
    //			// our radius can be no larger than half our height or width
    //			radius = Math.Min (radius, Math.Min (width / 2, height / 2));
    //			
    //			self.MoveTo (x + radius, y);
    //			self.Arc (x + width - radius, y + radius, radius, Math.PI * 1.5, Math.PI * 2);
    //			self.Arc (x + width - radius, y + height - radius, radius, 0, Math.PI * .5);
    //			self.Arc (x + radius, y + height - radius, radius, Math.PI * .5, Math.PI);
    //			self.Arc (x + radius, y + radius, radius, Math.PI, Math.PI * 1.5);
    //		}

private:

};

#endif	/* UTILITIES_H */

