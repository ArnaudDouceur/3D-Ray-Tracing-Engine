// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include <iostream.h>
#include <pthread.h>
#include <time.h>

#define NB_THREADS 8
#define AMBIENT_OCCLUSION_RAY_COUNT 64
#define ANTIALIASING_RESOLUTION 2
#define AMBIENT_OCCLUSION ON
static RayTracer * instance = NULL;

RayTracer * RayTracer::getInstance () {
    if (instance == NULL) {
        instance = new RayTracer ();
        
        Scene * scene = Scene::getInstance ();
        std::vector<Object> & objects = scene->getObjects();
        BoundingBox bbox = scene->getBoundingBox();
        
        for(unsigned int k = 0; k < objects.size(); k++) {
            const std::vector<Triangle> & triangles = objects[k].getMesh().getTriangles();
            const std::vector<Vertex> & vertices = objects[k].getMesh().getVertices();
            KdTree * myTree = new KdTree(bbox, triangles);
            std::vector<Vec3Df> vertices_pos;
            for(unsigned int i = 0; i < vertices.size(); i++)
                vertices_pos.push_back(vertices[i].getPos());
            myTree->build(vertices_pos);
            objects[k].getMesh().setKdTree(myTree);
        }
    }
    return instance;
}

void RayTracer::destroyInstance () {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

inline int clamp (float f, int inf, int sup) {
    int v = static_cast<int> (f);
    return (v < inf ? inf : (v > sup ? sup : v));
}

Vec3Df RayTracer::brdfPhong(const Vec3Df &omegaI, const Vec3Df &omega0, const Vec3Df &n, const Material &material) {
    
    Vec3Df R = n*Vec3Df::dotProduct(omegaI,n)*2-omegaI;
    return (material.getDiffuse()*Vec3Df::dotProduct(n,omegaI) + material.getSpecular()*pow(Vec3Df::dotProduct(R,omega0), material.getShininess()))*material.getColor();
}

struct IntersectionStruct {
    Vec3Df p;
    float t;
    float u;
    float v;
    Vec3Df n1;
    Vec3Df n2;
    Vec3Df n3;
    unsigned short object_id;
};

QImage RayTracer::render (const Vec3Df & camPos,
        const Vec3Df & direction,
        const Vec3Df & upVector,
        const Vec3Df & rightVector,
        float fieldOfView,
        float aspectRatio,
        unsigned int screenWidth,
        unsigned int screenHeight) {

    QTime time;
    time.start();

    QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);

    Scene * scene = Scene::getInstance ();
    const BoundingBox & bbox = scene->getBoundingBox ();
    const Vec3Df & minBb = bbox.getMin ();
    const Vec3Df & maxBb = bbox.getMax ();
    const Vec3Df rangeBb = maxBb-minBb;
    std::vector<Object> & objects = scene->getObjects();
    float ambient_occlusion_R = (maxBb - minBb).getLength()*0.03;

    std::vector<Light*> lights = scene->getLights();
#pragma omp parallel for default(shared) schedule(dynamic)
    for (unsigned int i = 0; i < screenWidth; i++) {
        for (unsigned int j = 0; j < screenHeight; j++) {
            
            Vec3Df color(0,0,0);
            
            for (unsigned int i_antialiasing = 0; i_antialiasing < ANTIALIASING_RESOLUTION; i_antialiasing++) {
                for (unsigned int j_antialiasing = 0; j_antialiasing < ANTIALIASING_RESOLUTION; j_antialiasing++) {
                
                    float tanX = tan (fieldOfView);
                    float tanY = tanX/aspectRatio;
                    Vec3Df stepX = (float (i) + float(i_antialiasing)/ANTIALIASING_RESOLUTION - screenWidth/2.f)/screenWidth * tanX * rightVector;
                    Vec3Df stepY = (float (j) + float(j_antialiasing)/ANTIALIASING_RESOLUTION - screenHeight/2.f)/screenHeight * tanY * upVector;
                    Vec3Df step = stepX + stepY;
                    Vec3Df dir = direction + step;
                    dir.normalize ();
                    Ray ray (camPos, dir);
                    struct IntersectionStruct intersection;
                    struct IntersectionStruct closestIntersection;
                    bool hasIntersection = false;
                    Triangle foundTriangle;

                    for(unsigned int k = 0; k < objects.size(); k++) {
                        const std::vector<Vertex> & vertices = objects[k].getMesh().getVertices();
                        KdTree tree = *(objects[k].getMesh().getKdTree());
                
                        if(ray.intersect(tree, vertices, foundTriangle, intersection.p, intersection.t, intersection.u, intersection.v))
                        {
                            if(not hasIntersection or intersection.t < closestIntersection.t) {
                                closestIntersection = intersection;
                                closestIntersection.object_id = k;
                                closestIntersection.n1 = vertices[foundTriangle.getVertex(0)].getNormal();
                                closestIntersection.n2 = vertices[foundTriangle.getVertex(1)].getNormal();
                                closestIntersection.n3 = vertices[foundTriangle.getVertex(2)].getNormal();
                                hasIntersection = true;
                            }
                        }
                    }
            
                    Vec3Df c (backgroundColor);
                    if (hasIntersection) {
                        //TODO fix up lights make code nicer... we know this is an area light
                        //but we should test type first will do that later
                        for(unsigned int k = 0; k < lights.size(); k++) {
                
                            Vec3Df omegaI = (*lights[k]).getPos() - closestIntersection.p;
                            omegaI.normalize();
                            float shade = (*lights[k]).getVisibility(closestIntersection.p, omegaI, objects);

                            if(shade > 0)
                            {
                                // TODO : How to use light color ? So far we assume light is white
                                Vec3Df n = (1-closestIntersection.u-closestIntersection.v)*closestIntersection.n1;
                                n += closestIntersection.u*closestIntersection.n2 + closestIntersection.v*closestIntersection.n3;
                                n.normalize ();
                                c = 255.f*RayTracer::brdfPhong(omegaI, -ray.getDirection(), n, objects[closestIntersection.object_id].getMaterial())*shade;
#ifdef AMBIENT_OCCLUSION
                                // Ambient Occlusion
                                Vec3Df p_epsilon = closestIntersection.p + 0.0001 * n;
                                unsigned int rays_not_stopped = AMBIENT_OCCLUSION_RAY_COUNT;
                                float AO = 0.f;
                                float sumW = 0.f;
        
                                for(unsigned int j = 0; j < AMBIENT_OCCLUSION_RAY_COUNT; j++) {

                     float r0 = (double (rand()) - (double)RAND_MAX/2)/(double)RAND_MAX ;
                     float r1 = (double (rand()) - (double)RAND_MAX/2)/(double)RAND_MAX ;
                     float r2 = (double (rand()) - (double)RAND_MAX/2)/(double)RAND_MAX ;
                                    Vec3Df dRay (r0, r1, r2);
                                    dRay.normalize ();
                                    if (Vec3Df::dotProduct (dRay, n) < 0.f)
                                        dRay = -dRay;
                     Ray oray = Ray(p_epsilon, dRay);
                     float weight = Vec3Df::dotProduct (n, dRay);
                                    // Now let's see if this ray hits a nearby Triangle
                                    for(unsigned int k = 0; k < objects.size(); k++) {
                                
                                        const std::vector<Vertex> & vertices = objects[k].getMesh().getVertices();
                                        KdTree & tree = *(objects[k].getMesh().getKdTree());
                                        if(oray.intersect(tree, vertices, foundTriangle, intersection.p, intersection.t, intersection.u, intersection.v)) {
                                                if(intersection.t < ambient_occlusion_R) {
                                                    rays_not_stopped -= 1;
                                                    AO += weight;
                                                    break;
                                                }
                                        }
                     }
                                    sumW += weight;
                     }
                                c = c* (1.f - AO/sumW);
#endif
                            }
                    
                        }
                    }
                    color += c;
                }
            }
            color = 1.f/ANTIALIASING_RESOLUTION/ANTIALIASING_RESOLUTION * color;
            image.setPixel (i, (screenHeight-1)-j, qRgb (clamp (color[0], 0, 255),
                        clamp (color[1], 0, 255),
                        clamp (color[2], 0, 255)));
        }
    }
    emit renderDone(time);
    return image;
}

