// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Ray.h"
#include "KdTree.h"

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
bool Ray::intersect (const Vec3Df & v1, const Vec3Df & v2, const Vec3Df & v3, Vec3Df & p, float & t, float & u, float & v) const {
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
    p = origin + t*direction;
	return true;
}

bool Ray::intersect (const KdTree & K, const vector<Vertex>& V, Triangle & triangle, Vec3Df & p, float & t, float & u, float & v) const {

    vector<Triangle> triangles = K.getTriangles();
    BoundingBox bbox = K.getBbox();

    //If ray does not intersect bbox no point in continuing...
    if(!intersect(bbox, p))
    {
        return false;
    }

    // Non-empty leaf
    if(!triangles.empty()) {
        
        float closest_t;
        float current_t, current_u, current_v;
        Vec3Df current_p;
        bool has_intersection = false;
        
        for(unsigned int i = 0; i < triangles.size(); i++) {
            
            const Vec3Df & v1 = V[triangles[i].getVertex(0)].getPos();
            const Vec3Df & v2 = V[triangles[i].getVertex(1)].getPos();
            const Vec3Df & v3 = V[triangles[i].getVertex(2)].getPos();            
            
            if (intersect (v1, v2, v3, current_p, current_t, current_u, current_v) && current_t > 0) {
                if(!has_intersection or current_t < t) {
                    t = current_t;
                    p = current_p;
                    u = current_u;
                    v = current_v;
                    triangle = triangles[i];                   
                }
                has_intersection = true;
            }
        }
        
        return has_intersection;
    }
    
    // Empty leaf
    if(K.getLeft() == NULL)
    {
        return false;
    }

    //Determine who is far and who is near
    //Then recursively traverse on near then far ?
    const KdTree * near;
    const KdTree * far;

    // TODO
    if(direction[K.getSplitAxis()] > 0.f)
    {
        near = K.getLeft();
        far = K.getRight();
    }
    else
    {
        near = K.getRight();
        far = K.getLeft();
    }
    
    if(intersect (*near, V, triangle, p, t, u, v))
        return true;
    else return intersect (*far, V, triangle, p, t, u, v);        
}

void Ray::shuffleInHemisphere() {
	float r0 = (double (rand()) - (double)RAND_MAX/2)/(double)RAND_MAX ;
	float r1 = (double (rand()) - (double)RAND_MAX/2)/(double)RAND_MAX ;
	float r2 = (double (rand()) - (double)RAND_MAX/2)/(double)RAND_MAX ;
	Vec3Df dRay (r0, r1, r2);
	dRay.normalize ();
	if (Vec3Df::dotProduct (dRay, direction) < 0.f)
		dRay = -dRay;
	direction = dRay;
    origin = origin + 0.0001*direction;
}