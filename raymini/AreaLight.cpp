#include "AreaLight.h"


void AreaLight::setDirection(const Vec3Df & dir) {
    direction = dir;
    direction.normalize();
    
    direction.getTwoOrthogonals(u, v);
    u.normalize();
    v.normalize();
}

// rand() is pseudo random. Does it matter?
// Just means we will compute the same soft shadows each time!
float AreaLight::getVisibility(Vec3Df & point, Vec3Df & dir, std::vector<Object> & objects)
{
    int visibility = 0;
    for(unsigned int i=0; i<resolution; ++i)
    {
        float angle = (float) (rand()/(float)RAND_MAX) * 2.f * M_PI;
        //float r     = (float) (rand() % 10000) * radius * M_PI/ 10000.f;
        float r = (float) (rand()/(float)RAND_MAX) * radius;
        Vec3Df pos  = getPos() + r * (cos(angle)*u + sin(angle)*v);
        dir = pos - point;
        dir.normalize();

        if(isVisible(point, dir, objects))
            visibility++;
    }
    return 0.5f;
	//return (float) visibility / (float) resolution;
}
