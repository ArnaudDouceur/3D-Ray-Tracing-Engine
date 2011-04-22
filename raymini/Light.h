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

#define POINT_LIGHT 0
#define AREA_LIGHT  1

class Light {
    public:
        inline Light () : color (Vec3Df (1.0f, 1.0f, 1.0f)), intensity (1.0f){}
        inline Light (const Vec3Df & pos, const Vec3Df & color, float intensity)
            : pos (pos), color (color), intensity (intensity) {}
        virtual ~Light () {}

        inline const Vec3Df & getPos () const { return pos; }
        inline const Vec3Df & getColor () const { return color; }
        inline float getIntensity () const { return intensity; }
        virtual inline int getLightType() { return POINT_LIGHT;}

        inline void setPos (const Vec3Df & p) { pos = p; }
        inline void setColor (const Vec3Df & c) { color = c; }
        inline void setIntensity (float i) { intensity = i; }
        // Determines if point light is visible 
        bool isVisible (Vec3Df & point, Vec3Df & dir, std::vector<Object> & objects);
        // Soft shading. Determines quantity of shade.
        // Between [0,1] with 0 = total shade and 1 = total visibility
        virtual float getVisibility (Vec3Df & point, Vec3Df & dir, std::vector<Object> & objects);
 

    private:
        Vec3Df pos;
        Vec3Df color;
        float intensity;
};


#endif // LIGHT_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
