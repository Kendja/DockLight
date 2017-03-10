/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Animations.h
 * Author: yoo
 *
 * Created on March 9, 2017, 2:28 PM
 */

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <math.h>

class Animation
{
public:

    /**
     * Time can be anything
     * Frames, time etc,
     */
    float t = 0;
    
    /**
     * beginning
     */
    float b = 0;
    
    /**
     * change
     */
    
    float c = 0;
    
    /**
     * Duration
     */
    float d = 0;
    
    bool m_animationInitSet = false;

};

class ElasticEaseOut : public Animation
{
public:
    
    float compute(float t, float b, float c, float d)
    {
        if (t == 0) return b;
        if ((t /= d) == 1) return b + c;
        float p = d * .3f;
        float a = c;
        float s = p / 4;
        return (a * pow(2, -10 * t) * sin((t * d - s)*(2 * M_PI) / p) + c + b);
    }
};

#endif /* ANIMATIONS_H */

