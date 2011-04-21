// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Scene.h"

// define RAM, ARMADILLO or HUMAN
#define RAM


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
    buildDefaultScene (true);
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

// Changer ce code pour créer des scènes originales
void Scene::buildDefaultScene (bool HD) {
#ifdef RAM    
    Mesh groundMesh;
    if (HD)
        groundMesh.loadOFF ("models/ground_HD.off");
    else
        groundMesh.loadOFF ("models/ground.off");
    Material groundMat;
    Object ground (groundMesh, groundMat);    
    objects.push_back (ground);
    
    Mesh ramMesh;
    if (HD)
        ramMesh.loadOFF ("models/ram_HD.off");
    else
        ramMesh.loadOFF ("models/ram.off");
    Material ramMat (1.f, 1.f, 128.f, Vec3Df (1.f, .6f, .2f));
    Object ram (ramMesh, ramMat);    
    objects.push_back (ram);
#endif
#ifdef ARMADILLO
    Mesh armadilloMesh;
    armadilloMesh.loadOFF ("models/armadillo.off");
    Material armadilloMat (1.f, 1.f, 128.f, Vec3Df (1.f, .6f, .2f));
    Object armadillo (armadilloMesh, armadilloMat);    
    objects.push_back (armadillo);
#endif
#ifdef HUMAN
    Mesh humanMesh;
    humanMesh.loadOFF ("models/max_50k.off");
    Material humanMat (1.f, 1.f, 128.f, Vec3Df (1.f, .6f, .2f));
    Object human (humanMesh, humanMat);    
    objects.push_back (human);
#endif

#ifdef RAM
    Light l1 (Vec3Df (3.0f, 3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f, 2.0f);
    lights.push_back (l1);
#endif
#if defined (HUMAN) || defined (ARMADILLO)
    Light l2 (Vec3Df (103.0f, 103.0f, 103.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f, 2.0f);
    lights.push_back (l2);
#endif
}
