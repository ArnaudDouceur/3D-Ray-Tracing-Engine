// *********************************************************
// Light Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Light.h"
#include "Ray.h"
#include "Triangle.h"
#include "Vertex.h"
#include "KdTree.h"

#define EPSILON 0.001

bool Light::isVisible(Vec3Df & point, Vec3Df & dir, std::vector<Object> & objects)
{
    Ray sray = Ray(point+EPSILON*dir, dir);
    Triangle foundTriangle;
    float t, u, v;
    Vec3Df p;

    for(unsigned int k = 0; k < objects.size(); k++) 
    {
        const std::vector<Vertex> & vertices = objects[k].getMesh().getVertices();
        KdTree tree = *(objects[k].getMesh().getKdTree());

        if(sray.intersect(tree, vertices, foundTriangle, p, t, u, v)) 
            return false;
    }

    return true;
}

float Light::getVisibility (Vec3Df & point, Vec3Df & dir, std::vector<Object> & objects)
{
    return isVisible(point, dir, objects)? 1.0f : 0.0f; 
}

