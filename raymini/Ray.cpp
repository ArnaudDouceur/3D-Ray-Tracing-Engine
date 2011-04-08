// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Ray.h"

#define EPSILON 0.000001

using namespace std;

static const unsigned int NUMDIM = 3, RIGHT = 0, LEFT = 1, MIDDLE = 2;

bool Ray::intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const {
    const Vec3Df & minBb = bbox.getMin ();
    const Vec3Df & maxBb = bbox.getMax ();
    bool inside = true;
    unsigned int  quadrant[NUMDIM];
    register unsigned int i;
    unsigned int whichPlane;
    Vec3Df maxT;
    Vec3Df candidatePlane;
    
    for (i=0; i<NUMDIM; i++)
        if (origin[i] < minBb[i]) {
            quadrant[i] = LEFT;
            candidatePlane[i] = minBb[i];
            inside = false;
        } else if (origin[i] > maxBb[i]) {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxBb[i];
            inside = false;
        } else	{
            quadrant[i] = MIDDLE;
        }

    if (inside)	{
        intersectionPoint = origin;
        return (true);
    }

    for (i = 0; i < NUMDIM; i++)
        if (quadrant[i] != MIDDLE && direction[i] !=0.)
            maxT[i] = (candidatePlane[i]-origin[i]) / direction[i];
        else
            maxT[i] = -1.;

    whichPlane = 0;
    for (i = 1; i < NUMDIM; i++)
        if (maxT[whichPlane] < maxT[i])
            whichPlane = i;

    if (maxT[whichPlane] < 0.) return (false);
    for (i = 0; i < NUMDIM; i++)
        if (whichPlane != i) {
            intersectionPoint[i] = origin[i] + maxT[whichPlane] *direction[i];
            if (intersectionPoint[i] < minBb[i] || intersectionPoint[i] > maxBb[i])
                return (false);
        } else {
            intersectionPoint[i] = candidatePlane[i];
        }
    return (true);			
}

// Moller Trumbore algorithm, MT97.pdf
// origin + t*direction = (1-u-v)*v1 + u*v2 + v*v3 = intersection point
bool Ray::intersect (const Triangle & triangle, const std::vector<Vertex> & vertices, Vec3Df & intersectionPoint, float & t, float & u, float & v) const {
    Vec3Df v1 = vertices[triangle.getVertex(0)].getPos();
    Vec3Df v2 = vertices[triangle.getVertex(1)].getPos();
    Vec3Df v3 = vertices[triangle.getVertex(2)].getPos();
	Vec3Df e1 = v2-v1;
	Vec3Df e2 = v3-v1;
	Vec3Df w = Vec3Df::crossProduct(direction,e2);
	float det = Vec3Df::dotProduct(e1,w);
	if(det > -EPSILON && det < EPSILON)
		return false;
	float invDet = 1.0/det;
	Vec3Df d = origin - v1;
	u = Vec3Df::dotProduct(d,w)*invDet;
	if(u < 0.0 || u > 1.0)
		return false;
	Vec3Df q = Vec3Df::crossProduct(d,e1);
	v = Vec3Df::dotProduct(direction, q)*invDet;
	if(v < 0.0 || u + v > 1.0)
		return false;
    t = Vec3Df::dotProduct(e2, q)*invDet;
    intersectionPoint = origin + t*direction;
	return true;
}