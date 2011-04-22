#include "AreaLight.h"


void AreaLight::setDirection(const Vec3Df & dir) {
    direction = dir;
    direction.normalize();
    
    direction.getTwoOrthogonals(u, v);
    u.normalize();
    v.normalize();
}

float AreaLight::getVisibility(Vec3Df & point, Vec3Df & dir, std::vector<Object> & objects)
{
    int visibility = 0;
    for(int i=0; i<resolution; ++i)
    {
        float angle = (float) (rand() % 10000) * 2.f * M_PI/ 10000.f;
        float r     = (float) (rand() % 10000) * radius * M_PI/ 10000.f;
        Vec3Df pos  = getPos() + r * (cos(angle)*u + sin(angle)*v);

        if(isVisible(point, dir, objects))
            visibility++;
    }
	return (float) visibility / (float) resolution;
}
