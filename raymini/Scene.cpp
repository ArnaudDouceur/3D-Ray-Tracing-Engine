// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Scene.h"
#include "SceneConfig.h"

#define ALIGHT_RADIUS 1.f

using namespace std;

static Scene * instance = NULL;

Scene * Scene::getInstance () {
    if (instance == NULL)
        instance = new Scene ();
    return instance;
}

void Scene::destroyInstance () {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

Scene::Scene () {
    buildDefaultScene (false);
    updateBoundingBox ();
}

Scene::~Scene () {
}

void Scene::updateBoundingBox () {
    if (objects.empty ())
        bbox = BoundingBox ();
    else {
        bbox = objects[0].getBoundingBox ();
        for (unsigned int i = 1; i < objects.size (); i++)
            bbox.extendTo (objects[i].getBoundingBox ());
    }
}

void Scene::setLights(unsigned int lightChoice)
{
    lights.clear();
    Light* l;
    Light* l1;
    Light* l2;
    Light* l3;

    switch (lightChoice)
    {
        case POINT_LIGHT:
            //l = new Light (Vec3Df (3.0f, 3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
            l = new Light (Vec3Df (0.0f, -10.0f, 5.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
            lights.push_back (l);
           // l1 = new Light (Vec3Df (3.0f, -3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
           // lights.push_back (l1);
            //l2 = new Light (Vec3Df (-10.0f, -10.0f, -10.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
            //lights.push_back (l2);
            //l3 = new Light (Vec3Df (3.0f, 3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
            //lights.push_back (l3);


            break;
        case AREA_LIGHT:
            //l = new AreaLight(Vec3Df (2.0f, -2.0f, 2.0f), Vec3Df (1.f, 1.f, 1.f), 1.f, Vec3Df (2.f, 2.f, -2.f), ALIGHT_RADIUS);
            l = new AreaLight(Vec3Df (1, 1, 4), Vec3Df (1.f, 1.f, 1.f), 10, Vec3Df (0,0,-1), 1.f);
            lights.push_back (l);
            l1 = new AreaLight(Vec3Df (-1, 1, 4), Vec3Df (1.f, 1.f, 1.f), 10, Vec3Df (0,0,-1), 1.f);
            lights.push_back (l1);
            l2 = new AreaLight(Vec3Df (0, -5, 2), Vec3Df (1.f, 1.f, 1.f), 100, Vec3Df (0,1,0), 2.f);
            lights.push_back (l2);
            

            break;
        default:
            std::cerr << "Somebody switched the lights off..." << std::endl;
    }

}


// TODO find nicer scenes if possible
void Scene::buildDefaultScene (bool HD) {

    // Built from SceneConfig.h
    buildObjects (objects, HD);
        
}
