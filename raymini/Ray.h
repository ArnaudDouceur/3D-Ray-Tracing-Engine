// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef RAY_H
#define RAY_H

#include <iostream>
#include <vector>

#include "Vec3D.h"
#include "BoundingBox.h"
#include "Scene.h"

struct IntersectionStruct {
    Vec3Df p;
    float t;
    float u;
    float v;
    Vec3Df n1;
    Vec3Df n2;
    Vec3Df n3;
	Vec3Df n;
    unsigned short object_id;
};

class Ray {
public:
    inline Ray () {}
    inline Ray (const Vec3Df & origin, const Vec3Df & direction)
        : origin (origin), direction (direction) {}
	
    inline virtual ~Ray () {}

    inline const Vec3Df & getOrigin () const { return origin; }
    inline Vec3Df & getOrigin () { return origin; }
    inline const Vec3Df & getDirection () const { return direction; }
    inline Vec3Df & getDirection () { return direction; }

    bool intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const;
    bool intersect (const Vec3Df & v1, const Vec3Df & v2, const Vec3Df & v3, Vec3Df & p, float & t, float & u, float & v) const;
    bool intersect (const KdTree & K, const std::vector<Vertex>& V, Triangle & triangle, Vec3Df & p, float & t, float & u, float & v) const;
	void shuffleInHemisphere();
	
private:
    Vec3Df origin;
    Vec3Df direction;
};


#endif // RAY_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
