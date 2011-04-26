// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Scene.h"

// define RAM, ARMADILLO or HUMAN
#define RAM
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
            l = new AreaLight(Vec3Df (2.0f, -0.5f, 4.0f), Vec3Df (1.f, 1.f, 1.f), 1.f, Vec3Df (-2.f, 0.5f, -4.f), 1.f);
            lights.push_back (l);

            break;
        default:
            std::cerr << "Somebody switched the lights off..." << std::endl;
    }

}


// TODO find nicer scenes if possible
void Scene::buildDefaultScene (bool HD) {
#ifdef RAM
   /*
    Mesh groundMesh;
    if (HD)
        groundMesh.loadOFF ("models/ground_HD.off");
    else
        groundMesh.loadOFF ("models/ground.off");
    Material groundMat;
    groundMat.setColor(Vec3Df(1,1,1));
    Object ground (groundMesh, groundMat);    
    objects.push_back (ground);
*/
    
    /*
    Mesh ramMesh;
    if (HD)
        ramMesh.loadOFF ("models/ram_HD.off");
    else
        ramMesh.loadOFF ("models/ram.off");
    Material ramMatY (1.f, 1.f, 128.f, Vec3Df (1, 0, 0));
    // Mirror effect
    Material ramMat (Vec3Df(1.f,1.f,1.f), .7f);
    Material ramMat2;
    ramMat2.setColor (Vec3Df(0,1,0));
    Object ram (ramMesh, ramMat2);    
    objects.push_back (ram);
    */
/*

    // SPHERE
    Mesh sphereMesh;
    sphereMesh.makeSphere();
    Object sphere(sphereMesh, ramMat);
    objects.push_back(sphere);
    
    
    // More objects
    // ram to the left and slightly behind
    Mesh ram_lbMesh;
    ram_lbMesh.loadOFF ("models/ram_lb.off");
    Object ram_lb (ram_lbMesh, ramMatY);
    objects.push_back (ram_lb);

    // ram to the right and slightly behind
    Mesh ram_rbMesh;
    ram_rbMesh.loadOFF ("models/ram_rb.off");
    Object ram_rb (ram_rbMesh, ramMatY);
    objects.push_back (ram_rb);
    */
    
/*
    // More objects
    // sphere left
    Mesh sphereLMesh;
    sphereLMesh.loadOFF ("models/floor.off");
    Material sphereMat (1.f, 1.f, 128.f, Vec3Df (1.f, .6f, .2f));    
    Object sphereL (sphereLMesh, sphereMat);
    objects.push_back (sphereL);
*/
    Mesh* wallRedMesh = new Mesh();
    wallRedMesh->makeWall(Vec3Df(-2.f,-2.f,0.f), Vec3Df(-2.f,2.f, 0.f), Vec3Df(-2.f, -2.f, 3.f), Vec3Df(-2.f, 2.f, 3.f));
    Material wallRedMat (1.f, 1.f, 128.f, Vec3Df (1.f, 0.f, 0.f));
    
    Object wallRed (*wallRedMesh, wallRedMat);
    objects.push_back(wallRed);

    Mesh* wallWhiteMesh = new Mesh();
    wallWhiteMesh->makeWall(Vec3Df(-2.f,-2.f,0.f), Vec3Df(2.f,-2.f, 0.f), Vec3Df(-2.f, 2.f, 0.f), Vec3Df(2.f, 2.f, 0.f));
    Material wallWhiteMat (1.f, 1.f, 128.f, Vec3Df (1.f, 1.f, 1.f));
    
    Object wallWhite (*wallWhiteMesh, wallWhiteMat);
    objects.push_back(wallWhite);

     Mesh* wallGreenMesh = new Mesh();
    wallGreenMesh->makeWall(Vec3Df(2.f,-2.f,0.f), Vec3Df(2.f, -2.f, 3.f), Vec3Df(2.f,2.f, 0.f), Vec3Df(2.f, 2.f, 3.f));
    Material wallGreenMat (1.f, 1.f, 128.f, Vec3Df (0.f, 1.f, 0.f));
    
    Object wallGreen (*wallGreenMesh, wallGreenMat);
    objects.push_back(wallGreen);

    Mesh* wallBlueMesh = new Mesh();
    wallBlueMesh->makeWall(Vec3Df(-2.f,2.f,0.f), Vec3Df(2.f,2.f, 0.f), Vec3Df(-2.f, 2.f, 3.f), Vec3Df(2.f, 2.f, 3.f));
    Material wallBlueMat (1.f, 1.f, 128.f, Vec3Df (0.f, 0.f, 1.f));
    
    Object wallBlue (*wallBlueMesh, wallBlueMat);
    objects.push_back(wallBlue);


    
/*
    // right sphere
    Mesh sphereRMesh;
    sphereRMesh.loadOFF ("models/sphereR.off");
    Object sphereR (sphereRMesh, groundMat);
    objects.push_back (sphereR);
*/
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
    //setLights (AREA_LIGHT);
/*
#ifdef RAM
    AreaLight* al1 = new AreaLight(Vec3Df (2.0f, -0.5f, 4.0f), Vec3Df (1.f, 1.f, 1.f), 1.f, Vec3Df (-2.f, 0.5f, -4.f), 1.f);
    lights.push_back (al1);
    //Light* l1 = new Light (Vec3Df (3.0f, 3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
    //lights.push_back (l1);

#endif
#if defined (HUMAN) || defined (ARMADILLO)
    Light* l2 = new Light (Vec3Df (103.0f, 103.0f, 103.0f), Vec3Df (1.0f, 1.0f, 1.0f), 2.5f);
    lights.push_back (l2);
#endif
*/
}
