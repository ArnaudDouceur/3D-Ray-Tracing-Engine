#include "AreaLight.h"
#include <algorithm>
using namespace std;

void AreaLight::setDirection(const Vec3Df & dir) {
    direction = dir;
    direction.normalize();
    
    direction.getTwoOrthogonals(u, v);
    u.normalize();
    v.normalize();
}

// rand() is pseudo random. Does it matter?
// Just means we will compute the same soft shadows each time!
float AreaLight::getVisibility(const Vec3Df & point, const Vec3Df & dir, std::vector<Object> & objects)
{
    int visibility = 0;
    for(unsigned int i=0; i<resolution; ++i)
    {
        float angle = (float) (rand()/(float)RAND_MAX) * 2.f * M_PI;
        //float r     = (float) (rand() % 10000) * radius * M_PI/ 10000.f;
        float r = (float) (rand()/(float)RAND_MAX) * radius;
        Vec3Df pos  = getPos() + r * (cos(angle)*u + sin(angle)*v);
        direction = pos - point;
        direction.normalize();

        if(isVisible(point, direction, objects))
            visibility++;
    }
	return (float) visibility / (float) resolution;
}

bool AreaLight::sample(const Vec3Df& fromPoint, const Vec3Df& fromNormal, Vec3Df& intensity_given, Vec3Df& incidence,  std::vector<Object> & objects)
{
	float angle = (float) (rand()/(float)RAND_MAX) * 2.f * M_PI;
    float r = (float) (rand()/(float)RAND_MAX) * radius;
    Vec3Df pos  = getPos() + r * (cos(angle)*u + sin(angle)*v);
    incidence = pos - fromPoint;
    float inlen = incidence.getLength();
    incidence.normalize();
    
    if(not isVisible(fromPoint, incidence, objects))
        return false;
    
    intensity_given = max(Vec3Df::dotProduct(-incidence, direction) / (inlen*inlen*inlen), 0.f) * intensity * color;
    return true;
}