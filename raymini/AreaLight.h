#ifndef AREALIGHT_H
#define AREALIGHT_H

#include <iostream>
#include <cmath>

#include "Light.h"

#define RESOLUTION 128

class AreaLight : public Light {
public:
    inline AreaLight () : Light (), radius(1.0f), resolution(RESOLUTION) {}
    inline AreaLight (const Vec3Df & position, const Vec3Df & color, float intensity,const Vec3Df & direction, float radius)
        : Light (position, color, intensity), radius(radius), resolution(RESOLUTION)
    {
            setDirection(direction);
    }
    
    inline const Vec3Df & getDirection() const { return direction; }
    inline float getRadius() const { return radius; }
    virtual inline int getLightType() { return AREA_LIGHT;}
    
    
    void setDirection(const Vec3Df & dir);
    inline void setRadius(float _radius) { radius = _radius; }
    inline void setResolution(int res) { resolution = (res > 0) ? res : -res; }
    
    virtual float getVisibility(Vec3Df & point, Vec3Df & dir, std::vector<Object> & objects);

private:
    Vec3Df direction;
    Vec3Df u, v;
    float radius;
    int resolution;
};


#endif /* AREALIGHT_H */
