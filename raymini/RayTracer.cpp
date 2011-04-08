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
#define NB_THREADS 8

static RayTracer * instance = NULL;

RayTracer * RayTracer::getInstance () {
    if (instance == NULL)
        instance = new RayTracer ();
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

struct thread_data{
    const Vec3Df * camPos;
    const Vec3Df * direction;
    const Vec3Df * upVector;
    const Vec3Df * rightVector;
    float fieldOfView;
    float aspectRatio;
    unsigned int screenWidth;
    unsigned int screenHeight;
    QImage* image;
    Scene* scene;
    const BoundingBox * bbox;
    const Vec3Df * minBb;
    const Vec3Df * maxBb;
    const Vec3Df * rangeBb;
    std::vector<Object> * objects;
    Vec3Df * backgroundColor;
    unsigned short working_zone;
};

void *RenderingThread(void *data) {
    
    struct thread_data *d = (struct thread_data *) data;
    
    const Vec3Df & camPos = *d->camPos;         
    const Vec3Df & direction = *d->direction;      
    const Vec3Df & upVector = *d->upVector;       
    const Vec3Df & rightVector = *d->rightVector;    
    float fieldOfView = d->fieldOfView;             
    float aspectRatio = d->aspectRatio;             
    unsigned int screenWidth = d->screenWidth;      
    unsigned int screenHeight = d->screenHeight;     
    QImage* image = d->image;                 
    Scene* scene = d->scene;                  
    const BoundingBox & bbox = *d->bbox;      
    const Vec3Df & minBb = *d->minBb;          
    const Vec3Df & maxBb = *d->maxBb;      
    const Vec3Df rangeBb = *d->rangeBb;    
    std::vector<Object> & objects =  *d->objects; 
    Vec3Df backgroundColor = *d->backgroundColor;
    unsigned short working_zone = d->working_zone;
    
    unsigned int threadStep = screenWidth/NB_THREADS;
    
    for (unsigned int i = working_zone*threadStep; i < (working_zone+1)*threadStep; i++) {
        for (unsigned int j = 0; j < screenHeight; j++) {
        
            float tanX = tan (fieldOfView);
            float tanY = tanX/aspectRatio;
            Vec3Df stepX = (float (i) - screenWidth/2.f)/screenWidth * tanX * rightVector;
            Vec3Df stepY = (float (j) - screenHeight/2.f)/screenHeight * tanY * upVector;
            Vec3Df step = stepX + stepY;
            Vec3Df dir = direction + step;
            dir.normalize ();
            Ray ray (camPos, dir);
            Vec3Df intersectionPoint;
            bool hasIntersection = false;
            float closestIntersectionDistance;

            for(unsigned int k = 0; k < objects.size(); k++) {
                const std::vector<Triangle> & triangles = objects[k].getMesh().getTriangles();
                const std::vector<Vertex> & vertices = objects[k].getMesh().getVertices();
                for(unsigned int l=0; l < triangles.size(); l++) {
                    float t,u,v;
                    Vec3Df currentIntersectionPoint;
                    if(ray.intersect(triangles[l], vertices, currentIntersectionPoint, t, u ,v)) {
                        if(not hasIntersection or t < closestIntersectionDistance) {
                            closestIntersectionDistance = t;
                            intersectionPoint = currentIntersectionPoint;
                        } 
                        hasIntersection = true;                                                
                    }
                }
            }
        
            Vec3Df c (backgroundColor);
            if (hasIntersection) 
                c = 255.f * ((intersectionPoint - minBb) / rangeBb);
            image->setPixel (i, ((screenHeight-1)-j), qRgb (clamp (c[0], 0, 255),
                                                       clamp (c[1], 0, 255),
                                                       clamp (c[2], 0, 255)));
        }
    }
    return NULL;
}

QImage RayTracer::render (const Vec3Df & camPos,
                          const Vec3Df & direction,
                          const Vec3Df & upVector,
                          const Vec3Df & rightVector,
                          float fieldOfView,
                          float aspectRatio,
                          unsigned int screenWidth,
                          unsigned int screenHeight) {
    QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);
    
    Scene * scene = Scene::getInstance ();
    const BoundingBox & bbox = scene->getBoundingBox ();
    const Vec3Df & minBb = bbox.getMin ();
    const Vec3Df & maxBb = bbox.getMax ();
    const Vec3Df rangeBb = maxBb-minBb;
    std::vector<Object> & objects = scene->getObjects();
    
    thread_data thread_data_array[NB_THREADS];
    pthread_t threads[NB_THREADS];
    
    for (unsigned int i = 0; i < NB_THREADS; i++) {
        thread_data_array[i].camPos = &camPos;
        thread_data_array[i].direction = &direction;
        thread_data_array[i].upVector = &upVector;
        thread_data_array[i].rightVector = &rightVector;
        thread_data_array[i].fieldOfView = fieldOfView;
        thread_data_array[i].aspectRatio = aspectRatio;
        thread_data_array[i].screenWidth = screenWidth;
        thread_data_array[i].screenHeight = screenHeight;
        thread_data_array[i].image = &image;
        thread_data_array[i].scene = scene;
        thread_data_array[i].bbox = &bbox;
        thread_data_array[i].minBb = &minBb;
        thread_data_array[i].maxBb = &maxBb;
        thread_data_array[i].rangeBb = &rangeBb;
        thread_data_array[i].objects = &objects;
        thread_data_array[i].backgroundColor = &backgroundColor;
        thread_data_array[i].working_zone = i;
        pthread_create(&threads[i], NULL, RenderingThread, (void *) &thread_data_array[i]);        
    }
    void *status;
    for (unsigned short i = 0; i < NB_THREADS; i++)
        pthread_join(threads[i], &status);
    return image;
}
