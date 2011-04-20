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
    return (material.getDiffuse()*Vec3Df::dotProduct(n,omegaI) + material.getSpecular()*Vec3Df::dotProduct(R,omega0))*material.getColor();
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
    std::vector<Light> lights = scene->getLights();

    unsigned int threadStep = screenWidth/NB_THREADS;
    unsigned int max_i;
    if(working_zone == NB_THREADS-1)
        max_i = screenWidth;
    else
        max_i = (working_zone+1)*threadStep;


    for (unsigned int i = working_zone*threadStep; i < max_i; i++) {
        for (unsigned int j = 0; j < screenHeight; j++) {

            float tanX = tan (fieldOfView);
            float tanY = tanX/aspectRatio;
            Vec3Df stepX = (float (i) - screenWidth/2.f)/screenWidth * tanX * rightVector;
            Vec3Df stepY = (float (j) - screenHeight/2.f)/screenHeight * tanY * upVector;
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
                for(unsigned int k = 0; k < lights.size(); k++) {
                    Vec3Df omegaI = lights[k].getPos() - closestIntersection.p;
                    omegaI.normalize();
                    
                    // Do we see the light ?
                    Ray sray = Ray(closestIntersection.p+0.00001*omegaI, omegaI);
                    bool canReachLight = true;
                    
                    for(unsigned int k = 0; k < objects.size(); k++) {
                        const std::vector<Vertex> & vertices = objects[k].getMesh().getVertices();
                        KdTree tree = *(objects[k].getMesh().getKdTree());

                        if(sray.intersect(tree, vertices, foundTriangle, intersection.p, intersection.t, intersection.u, intersection.v)) {
                                canReachLight = false;
                                break;
                        }
                    }
                    
                    // If we see the light, let's use it
                    if(canReachLight) {
                        // TODO : How to use light color ? So far we assume light is white
                        Vec3Df n = (1-closestIntersection.u-closestIntersection.v)*closestIntersection.n1;
                        n += closestIntersection.u*closestIntersection.n2 + closestIntersection.v*closestIntersection.n3;
                        c = 255.f*RayTracer::brdfPhong(omegaI, -ray.getDirection(), n, objects[closestIntersection.object_id].getMaterial());
                    }
                }
            }
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

    QTime time;
    time.start();

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

    //cout << "Render time = " << time.elapsed() << endl;
    emit renderDone(time);
    return image;
}
