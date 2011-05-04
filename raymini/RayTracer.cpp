// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "RayTracer.h"
#include "Ray.h"
#include "Window.h"
#include "Scene.h"
#include <iostream.h>
#include <time.h>

static RayTracer * instance = NULL;
int RayTracer::AO_RAY_COUNT = 32;
int RayTracer::ANTIALIASING_RES = 2;

#define USE_PATH_TRACING_ENGINE 1
#define INDIRECT_ILLUMINATION 1
#define NUMBER_OF_BOUNCES 1
#define SQRT_PATHS_PER_PIXEL 16
#define USE_LENSE 0
#define LENSE_SIZE 0.3f

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

inline float randf() {
    return (float)rand()/(float)RAND_MAX;
}

inline int clamp (float f, int inf, int sup) {
    int v = static_cast<int> (f);
    return (v < inf ? inf : (v > sup ? sup : v));
}

Vec3Df RayTracer::brdfPhong(const Vec3Df &omegaI, const Vec3Df &omega0, const Vec3Df &n, const Material &material) {

    Vec3Df R = n*Vec3Df::dotProduct(omegaI,n)*2-omegaI;
    return (material.getDiffuse()*Vec3Df::dotProduct(n,omegaI) + material.getSpecular()*pow(Vec3Df::dotProduct(R,omega0), material.getShininess()))*material.getColor();
}

#if USE_PATH_TRACING_ENGINE == 0

QImage RayTracer::render (const Vec3Df & camPos,
        const Vec3Df & direction,
        const Vec3Df & upVector,
        const Vec3Df & rightVector,
        float fieldOfView,
        float aspectRatio,
        unsigned int screenWidth,
        unsigned int screenHeight,
        unsigned int lightChoice,
        unsigned int flags) {

    QTime time;
    time.start();
    emit init (0, screenWidth-1);

    QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);

    Scene * scene = Scene::getInstance ();
    scene->setLights (lightChoice);
    const BoundingBox & bbox = scene->getBoundingBox ();
    const Vec3Df & minBb = bbox.getMin ();
    const Vec3Df & maxBb = bbox.getMax ();
    const Vec3Df rangeBb = maxBb-minBb;
    std::vector<Object> & objects = scene->getObjects();
    float ambient_occlusion_R = (maxBb - minBb).getLength()*0.03;

    std::vector<Light*> lights = scene->getLights();
    int progressValue = 0;
    #pragma omp parallel for default(shared) schedule(dynamic)
    for (unsigned int i = 0; i < screenWidth; i++) 
    {
        #pragma omp critical(progressupdate)
        {
            //#pragma omp atomic
            progressValue++;
            emit progress (progressValue);
        }

        for (unsigned int j = 0; j < screenHeight; j++) 
        {

            Vec3Df color(0,0,0);

            for (unsigned int i_antialiasing = 0; i_antialiasing < ANTIALIASING_RES; i_antialiasing++) 
            {
                for (unsigned int j_antialiasing = 0; j_antialiasing < ANTIALIASING_RES; j_antialiasing++) 
                {

                    float tanX = tan (fieldOfView);
                    float tanY = tanX/aspectRatio;
                    Vec3Df stepX = (float (i) + float(i_antialiasing)/ANTIALIASING_RES - screenWidth/2.f)/screenWidth * tanX * rightVector;
                    Vec3Df stepY = (float (j) + float(j_antialiasing)/ANTIALIASING_RES - screenHeight/2.f)/screenHeight * tanY * upVector;
                    Vec3Df step = stepX + stepY;
                      dir = direction + step;
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
                                
                                // Ambient Occlusion                                
                                if (flags & ENABLE_AO)
                                {
                                    Vec3Df p_epsilon = closestIntersection.p + 0.0001 * n;
                                    //unsigned int rays_not_stopped = AMBIENT_OCCLUSION_RAY_COUNT;
                                    unsigned int rays_not_stopped = AO_RAY_COUNT;
                                    float AO = 0.f;
                                    float sumW = 0.f;

                                    for(unsigned int j = 0; j < AO_RAY_COUNT; j++) 
                                    {

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
                                        for(unsigned int k = 0; k < objects.size(); k++) 
                                        {

                                            const std::vector<Vertex> & vertices = objects[k].getMesh().getVertices();
                                            KdTree & tree = *(objects[k].getMesh().getKdTree());
                                            if(oray.intersect(tree, vertices, foundTriangle, intersection.p, intersection.t, intersection.u, intersection.v))
                                            {
                                                if(intersection.t < ambient_occlusion_R) 
                                                {
                                                    rays_not_stopped -= 1;
                                                    AO += weight;
                                                    break;
                                                }
                                            }
                                        }
                                        sumW += weight;
                                    }
                                    c = c* (1.f - AO/sumW);
                                }
                            }

                        }
                    }
                    color += c;
                }
            }
            color = 1.f/ANTIALIASING_RES/ANTIALIASING_RES * color;

            image.setPixel (i, (screenHeight-1)-j, qRgb (clamp (color[0], 0, 255),
                        clamp (color[1], 0, 255),
                        clamp (color[2], 0, 255)));
        }
    }
    emit renderDone(time);
    return image;
}

#else

QImage RayTracer::render (const Vec3Df & camPos,
        const Vec3Df & direction,
        const Vec3Df & upVector,
        const Vec3Df & rightVector,
        float fieldOfView,
        float aspectRatio,
        unsigned int screenWidth,
        unsigned int screenHeight,
        unsigned int lightChoice,
        unsigned int flags) {

    QTime time;
    time.start();
    
    Scene::getInstance()->setLights (lightChoice);
    QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);
    
    #if USE_LENSE == 1
    
    // We are going to shoot a ray in the middle of the screen and use it first
    // intersection as distance for the focal plane.
    
    Vec3Df camera_dir = Vec3Df::crossProduct(upVector, rightVector);    
    Ray mRay(camPos, camera_dir);
    
    struct IntersectionStruct intersection, closestIntersection;
    Scene * scene = Scene::getInstance ();
    std::vector<Object> & objects = scene->getObjects();
    bool hasIntersection = false;
    Triangle foundTriangle;
    
    for(unsigned int k = 0; k < objects.size(); k++) {
        const std::vector<Vertex> & vertices = objects[k].getMesh().getVertices();
        KdTree tree = *(objects[k].getMesh().getKdTree());

        if(mRay.intersect(tree, vertices, foundTriangle, intersection.p, intersection.t, intersection.u, intersection.v))
        {
            if(not hasIntersection or intersection.t < closestIntersection.t) {
                closestIntersection = intersection;
                hasIntersection = true;
            }
        }
    }
        
    if(not hasIntersection) {
        cerr << "Focal distance couldn't be guessed" << endl;
        return image;
    }
    
    float focal_distance = closestIntersection.t;
    cerr << "Rendering with focal distance : " << closestIntersection.t << endl;
    
    #endif
    
    #pragma omp parallel for default(shared) schedule(dynamic)
    for (int i = 0; i < (int) screenWidth; i++) {
        
        #pragma omp critical(progressupdate)
        {
            emit progress (i);
        }
        
        for (unsigned int j = 0; j < screenHeight; j++) {
            
            Vec3Df color(0,0,0);
            
            for (unsigned int i_antialiasing = 0; i_antialiasing < SQRT_PATHS_PER_PIXEL; i_antialiasing++) {
                for (unsigned int j_antialiasing = 0; j_antialiasing < SQRT_PATHS_PER_PIXEL; j_antialiasing++) {
                
                    float tanX = tan (fieldOfView);
                    float tanY = tanX/aspectRatio;
                    Vec3Df stepX = (float (i) + (i_antialiasing + randf())/SQRT_PATHS_PER_PIXEL - screenWidth/2.f)/screenWidth * tanX * rightVector;
                    Vec3Df stepY = (float (j) + (j_antialiasing + randf())/SQRT_PATHS_PER_PIXEL - screenHeight/2.f)/screenHeight * tanY * upVector;
                    Vec3Df step = stepX + stepY;
                    Vec3Df dir = direction + step;
                    dir.normalize ();
                    
                    #if USE_LENSE == 1
                    Vec3Df end = camPos + focal_distance / Vec3Df::dotProduct(dir, camera_dir) * dir;
                    float theta = 2*M_PI*randf();
                    float r = randf();
                    Vec3Df begin = camPos + r*sin(theta)*LENSE_SIZE*upVector + r*cos(theta)*LENSE_SIZE*rightVector;
                    Vec3Df newDir = end - begin;
                    newDir.normalize();
                    Ray ray(begin, newDir);
                    #else
                    Ray ray (camPos, dir);  
                    #endif
                                      
                    color += 255.f*pathtrace(ray, 0);
                }
            }
            // 255 is an empirical factor depending on how bounded is the scene
            color = 1.f/SQRT_PATHS_PER_PIXEL/SQRT_PATHS_PER_PIXEL * color;
            image.setPixel (i, (screenHeight-1)-j, qRgb (clamp (color[0], 0, 255),
                        clamp (color[1], 0, 255),
                        clamp (color[2], 0, 255)));
        }
    }
    emit renderDone(time);
    return image;
}

#endif

Vec3Df RayTracer::pathtrace(Ray& ray, unsigned int depth)
{

    struct IntersectionStruct intersection, closestIntersection;
    Scene * scene = Scene::getInstance ();
    std::vector<Object> & objects = scene->getObjects();
    bool hasIntersection = false;
    Triangle foundTriangle;
    Ray ray_intersection;
    
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
                ray_intersection = ray;
            }
        }
    }
    
	if (!hasIntersection)
		return Vec3Df(0,0,0);

    Material material = objects[closestIntersection.object_id].getMaterial();
    closestIntersection.n = (1-closestIntersection.u-closestIntersection.v)*closestIntersection.n1;
    closestIntersection.n += closestIntersection.u*closestIntersection.n2 + closestIntersection.v*closestIntersection.n3;
    closestIntersection.n.normalize ();

    // Mirror Material
    if (material.getMirror() > 0) {
        if (depth < NUMBER_OF_BOUNCES)
            return mirrorReflect(closestIntersection, ray_intersection, depth);
        return Vec3Df(0,0,0);
    }

    // Compute incoming light
    Vec3Df light = directIllumination(closestIntersection, ray_intersection);
    
    #if INDIRECT_ILLUMINATION == 1
    // Add the light coming from a random indirect source if depth is ok
    if (depth < NUMBER_OF_BOUNCES)
        light += bounceIllumination(closestIntersection, ray_intersection, depth);
    #endif
    return light;
}

Vec3Df RayTracer::directIllumination(const struct IntersectionStruct& intersection, const Ray & ray)
{
    Scene * scene = Scene::getInstance ();
    std::vector<Object> & objects = scene->getObjects();
    std::vector<Light*> & lights = scene->getLights();
	Material & material = objects[intersection.object_id].getMaterial();
    
	Vec3Df light(0,0,0);

	for (unsigned int i = 0; i < lights.size(); i++) {
		Vec3Df intensity;
		Vec3Df lightincidence;
		if (lights[i]->sample(intersection.p, intersection.n, intensity, lightincidence, objects)) {
			light += intensity * brdfPhong(lightincidence, -ray.getDirection(), intersection.n, material);
		}
	}

	return light;
}


Vec3Df RayTracer::bounceIllumination(const struct IntersectionStruct& intersection, const Ray & ray, int depth)
{
    Scene * scene = Scene::getInstance ();
    std::vector<Object> & objects = scene->getObjects();
	Material & material = objects[intersection.object_id].getMaterial();
    
	Ray diffRay = Ray(intersection.p, intersection.n);
	diffRay.shuffleInHemisphere();    
	Vec3Df rayDir = diffRay.getDirection();
    
    Vec3Df brdf = brdfPhong(rayDir, -ray.getDirection(), intersection.n, material);
	Vec3Df incoming = pathtrace(diffRay, depth + 1);

    brdf[0] *= incoming[0];
    brdf[1] *= incoming[1];
    brdf[2] *= incoming[2];
    
	return  brdf;
}

Vec3Df RayTracer::mirrorReflect(const struct IntersectionStruct & intersection, Ray & ray, int depth)
{
	Scene * scene = Scene::getInstance ();
    std::vector<Object> & objects = scene->getObjects();
	Material & material = objects[intersection.object_id].getMaterial();
    float mirror = material.getMirror();
    Vec3Df color = material.getColor();

    Ray mirrorRay(intersection.p, reflect(ray.getDirection(), intersection.n));
	Vec3Df mirrorColor = pathtrace(mirrorRay, depth + 1);
	
    mirrorColor[0] *= color[0];
    mirrorColor[1] *= color[1];
    mirrorColor[2] *= color[2];

    return mirror*mirrorColor;
}

Vec3Df inline RayTracer::reflect(const Vec3Df& dir, const Vec3Df& normal)
{
	return dir - (2 * (Vec3Df::dotProduct(dir, normal) * normal));
}