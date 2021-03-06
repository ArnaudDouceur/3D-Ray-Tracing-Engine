#include "Material.h"
#include "Object.h"
#include "Mesh.h"

/**
 * Modify this file according to the scene you wish to render.
 *
 * Only change the first line.
 * Valid scene names are :
 *  RAM
 *  RAM_ARMY
 *  RAM_IN_THE_BOX
 *  TAURUS_IN_THE_BOX
 *  ARMADILLO
 * 
 */

#define RAM_IN_THE_BOX

void buildObjects(std::vector<Object> & objects, bool HD)
{
#ifdef RAM
    // Big ground
    Mesh groundMesh;
    if (HD)
        groundMesh.loadOFF ("models/ground_HD.off");
    else
        groundMesh.loadOFF ("models/ground.off");

    Material groundMat;
    groundMat.setColor(Vec3Df(1,1,1));
    Object ground (groundMesh, groundMat);    
    objects.push_back (ground);

    Mesh ramMesh;
    if (HD)
        ramMesh.loadOFF ("models/ram_HD.off");
    else
        ramMesh.loadOFF ("models/ram.off");
    Material ramMatY (1.f, 1.f, 128.f, Vec3Df (1, .7, .2));
    Object ram (ramMesh, ramMatY);    
    objects.push_back (ram);

#endif

#ifdef RAM_ARMY
    // Big ground
    Mesh groundMesh;
    groundMesh.loadOFF ("models/groundBig.off");
    Material groundMat;
    groundMat.setColor(Vec3Df(1,1,1));
    Object ground (groundMesh, groundMat);    
    objects.push_back (ground);

    // Chief Ram the foremost
    Mesh ramChiefMesh;
    ramChiefMesh.loadOFF("models/ramChief.off");
    Material ramChiefMat (1.f, 1.f, 128.f, Vec3Df(1.f, 0.6f, 0.2f));
    Object ramChief (ramChiefMesh, ramChiefMat);
    objects.push_back(ramChief);

    // First line of 2 rams left and right behind chief
    Mesh ramL1Mesh;
    ramL1Mesh.loadOFF("models/ramL1Deep.off");
    Material ramMinionMat;
    Object ramL1 (ramL1Mesh, ramMinionMat);
    objects.push_back(ramL1);

    Mesh ramR1Mesh;
    ramR1Mesh.loadOFF("models/ramR1Deep.off");
    Object ramR1 (ramR1Mesh, ramMinionMat);
    objects.push_back(ramR1);

    // Second line of 2 rams left and right behind chief
    Mesh ramL2Mesh;
    ramL2Mesh.loadOFF("models/ramL2Deep.off");
    Object ramL2 (ramL2Mesh, ramMinionMat);
    objects.push_back(ramL2);

    Mesh ramR2Mesh;
    ramR2Mesh.loadOFF("models/ramR2Deep.off");
    Object ramR2 (ramR2Mesh, ramMinionMat);
    objects.push_back(ramR2);

    // Third line of 2 rams left and right behind chief
    Mesh ramL3Mesh;
    ramL3Mesh.loadOFF("models/ramL3Deep.off");
    Object ramL3 (ramL3Mesh, ramMinionMat);
    objects.push_back(ramL3);

    Mesh ramR3Mesh;
    ramR3Mesh.loadOFF("models/ramR3Deep.off");
    Object ramR3 (ramR3Mesh, ramMinionMat);
    objects.push_back(ramR3);

    // Fourth line of 2 rams left and right behind chief
    Mesh ramL4Mesh;
    ramL4Mesh.loadOFF("models/ramL4Deep.off");
    Object ramL4 (ramL4Mesh, ramMinionMat);
    objects.push_back(ramL4);

    Mesh ramR4Mesh;
    ramR4Mesh.loadOFF("models/ramR4Deep.off");
    Object ramR4 (ramR4Mesh, ramMinionMat);
    objects.push_back(ramR4);
#endif


#ifdef RAM_IN_THE_BOX
    // BOX
    Mesh* wallLeftMesh = new Mesh();
    wallLeftMesh->makeWall(Vec3Df(-2.f,-2.f,0.f), Vec3Df(-2.f,2.f, 0.f), Vec3Df(-2.f, -2.f, 3.f), Vec3Df(-2.f, 2.f, 3.f));
    Material wallLeftMat (1.f, 1.f, 128.f, Vec3Df (0.f, 0, 0.6f));

    Object wallLeft (*wallLeftMesh, wallLeftMat);
    objects.push_back(wallLeft);

    Mesh* wallGroundMesh = new Mesh();
    wallGroundMesh->makeWall(Vec3Df(-2.f,-2.f,0.f), Vec3Df(2.f,-2.f, 0.f), Vec3Df(-2.f, 2.f, 0.f), Vec3Df(2.f, 2.f, 0.f));
    Material wallGroundMat (1.f, 1.f, 128.f, Vec3Df (1.f, 1.f, 1.f));

    Object wallGround (*wallGroundMesh, wallGroundMat);
    objects.push_back(wallGround);

    Mesh* wallRightMesh = new Mesh();
    wallRightMesh->makeWall(Vec3Df(2.f,-2.f,0.f), Vec3Df(2.f, -2.f, 3.f), Vec3Df(2.f,2.f, 0.f), Vec3Df(2.f, 2.f, 3.f));
    Material wallRightMat (1.f, 1.f, 128.f, Vec3Df (.6, 0, 0));

    Object wallRight (*wallRightMesh, wallRightMat);
    objects.push_back(wallRight);

    Mesh* wallBackMesh = new Mesh();
    wallBackMesh->makeWall(Vec3Df(-2.f,2.f,0.f), Vec3Df(2.f,2.f, 0.f), Vec3Df(-2.f, 2.f, 3.f), Vec3Df(2.f, 2.f, 3.f));
    Material wallBackMat (1.f, 1.f, 128.f, Vec3Df (0.f, .6f, 0.f));

    Object wallBack (*wallBackMesh, wallBackMat);
    objects.push_back(wallBack);

    // SPHERE
    Mesh sphereMesh;
    sphereMesh.loadOFF("models/sphere0.5.off", Vec3Df(-1.2,1,1));
	Material sphereMat(Vec3Df(1,1,1), 0.8);
    Object sphere (sphereMesh, sphereMat);
    objects.push_back(sphere);

	// SPHERE
    Mesh sphereMesh2;
    sphereMesh2.loadOFF("models/sphere0.5.off", Vec3Df(1.2,1,1));
    Object sphere2 (sphereMesh2, sphereMat);
    objects.push_back(sphere2);

	Mesh sphereMesh3;
	sphereMesh3.loadOFF("models/sphere.off");
	Material sphereMat2 (1.f, 1.f, 128.f, Vec3Df (1,1,1));
	//Material sphereMat (Vec3Df (1.f,1.f,1.f), 0.8f);     
	Object sphere3(sphereMesh3, sphereMat2);
	objects.push_back(sphere3);
	
    // RAM
    Mesh ramMesh;
    if (HD)
       ramMesh.loadOFF ("models/ramOnUnitSphere_HD.off"); //TODO
   else
       ramMesh.loadOFF ("models/ramOnUnitSphere.off");
       
   Material ramMatY (1.f, 1.f, 128.f, Vec3Df (1, .7, .2));
   // Mirror effect
   Object ram (ramMesh, ramMatY);    
   objects.push_back (ram);
#endif

#ifdef TAURUS_IN_THE_BOX
 // BOX
    Mesh* wallLeftMesh = new Mesh();
    wallLeftMesh->makeWall(Vec3Df(-2.f,-2.f,0.f), Vec3Df(-2.f,2.f, 0.f), Vec3Df(-2.f, -2.f, 3.f), Vec3Df(-2.f, 2.f, 3.f));
    Material wallLeftMat (1.f, 1.f, 128.f, Vec3Df (0.2, 0.2, 0.8));

    Object wallLeft (*wallLeftMesh, wallLeftMat);
    objects.push_back(wallLeft);

    Mesh* wallGroundMesh = new Mesh();
    wallGroundMesh->makeWall(Vec3Df(-2.f,-2.f,0.f), Vec3Df(2.f,-2.f, 0.f), Vec3Df(-2.f, 2.f, 0.f), Vec3Df(2.f, 2.f, 0.f));
    Material wallGroundMat (1.f, 1.f, 128.f, Vec3Df (1.f, 1.f, 1.f));

    Object wallGround (*wallGroundMesh, wallGroundMat);
    objects.push_back(wallGround);

    Mesh* wallRightMesh = new Mesh();
    wallRightMesh->makeWall(Vec3Df(2.f,-2.f,0.f), Vec3Df(2.f, -2.f, 3.f), Vec3Df(2.f,2.f, 0.f), Vec3Df(2.f, 2.f, 3.f));
    Material wallRightMat (1.f, 1.f, 128.f, Vec3Df (0.8, 0.2, 0.2));

    Object wallRight (*wallRightMesh, wallRightMat);
    objects.push_back(wallRight);

    Mesh* wallBackMesh = new Mesh();
    wallBackMesh->makeWall(Vec3Df(-2.f,2.f,0.f), Vec3Df(2.f,2.f, 0.f), Vec3Df(-2.f, 2.f, 3.f), Vec3Df(2.f, 2.f, 3.f));
    Material wallBackMat (1.f, 1.f, 128.f, Vec3Df (0.2f, 0.8f, 0.2f));

    Object wallBack (*wallBackMesh, wallBackMat);
    objects.push_back(wallBack);

    
    //TAURUS
    Mesh taurusMesh;
    taurusMesh.loadOFF("models/taurus.off", Vec3Df(0.f, 0.f, 0.8f));
	Material mirrorMat(Vec3Df(1,1,1), 0.8);    
    Object taurus (taurusMesh, mirrorMat);
    objects.push_back(taurus);
#endif

#ifdef ARMADILLO
    Mesh armadilloMesh;
    armadilloMesh.loadOFF ("models/armadillo.off");
    Material armadilloMat (1.f, 1.f, 128.f, Vec3Df (1.f, .6f, .2f));
    Object armadillo (armadilloMesh, armadilloMat);    
    objects.push_back (armadillo);
#endif

}


