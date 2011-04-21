// *********************************************************
// Light Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <vector>

#include "Vec3D.h"
#include "Object.h"

class Light {
    public:
        inline Light () : color (Vec3Df (1.0f, 1.0f, 1.0f)), intensity (1.0f), radius (0.f){}
        inline Light (const Vec3Df & pos, const Vec3Df & color, float intensity)
            : pos (pos), color (color), intensity (intensity), radius (0.f) {}
        inline Light (const Vec3Df & pos, const Vec3Df & color, float intensity, float radius)
            : pos (pos), color (color), intensity (intensity), radius (radius) 
        {

            if (radius > 0.f)
            {
                std::cerr << "Area Light of radius : "<< radius << std::endl;

            }            


        }
        virtual ~Light () {}

        inline const Vec3Df & getPos () const { return pos; }
        inline const Vec3Df & getColor () const { return color; }
        inline float getIntensity () const { return intensity; }
        inline float getRadius() const {return radius;}

        inline void setPos (const Vec3Df & p) { pos = p; }
        inline void setColor (const Vec3Df & c) { color = c; }
        inline void setIntensity (float i) { intensity = i; }
        inline void setRadius (float r) {radius = r;}
        // Determines if point light is visible 
        bool isVisible (Vec3Df & point, Vec3Df & dir, std::vector<Object> & objects);
        //Determines area light amount of visibility for a given amount of rays
        // Between [0,1] 0 = no light/ 1 = complete light
        float isVisible (Vec3Df & point, Vec3Df & dir, unsigned int rays, std::vector<Object> & objects);
 


    private:
        Vec3Df pos;
        Vec3Df color;
        float intensity;
        float radius;
};


#endif // LIGHT_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
