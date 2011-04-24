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
int RayTracer::AO_RAY_COUNT = 64;
int RayTracer::ANTIALIASING_RES = 2;

#define USE_PATH_TRACING_ENGINE 1
#define INDIRECT_ILLUMINATION 0
#define SQRT_PATHS_PER_PIXEL 16

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

#if USE_PATH_TRACING_ENGINE == 0

Vec3Df RayTracer::brdfPhong(const Vec3Df &omegaI, const Vec3Df &omega0, const Vec3Df &n, const Material &material) {

    Vec3Df R = n*Vec3Df::dotProduct(omegaI,n)*2-omegaI;
    return (material.getDiffuse()*Vec3Df::dotProduct(n,omegaI) + material.getSpecular()*pow(Vec3Df::dotProduct(R,omega0), material.getShininess()))*material.getColor();
}

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
    int progressValue = 0;
    
    Scene::getInstance()->setLights (lightChoice);
    QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);

    #pragma omp parallel for default(shared) schedule(dynamic)
    for (unsigned int i = 0; i < screenWidth; i++) {
        
        #pragma omp critical(progressupdate)
        {
            progressValue++;
            emit progress (progressValue);
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
                    Ray ray (camPos, dir);                    
                    color += 255.f*pathtrace(ray, 0);
                }
            }
            //color = 1.f/SQRT_PATHS_PER_PIXEL/SQRT_PATHS_PER_PIXEL * color;
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
    
	Vec3Df pointColor(0,0,0);

	// Russian Roulette
	double survival = 1.0;
	if (depth > 2)
	{
		if (russianRoulette(material.getDiffuse() + material.getSpecular(), survival))
			return pointColor;
	}
    
    // Direct illumination
	pointColor += directIllumination(closestIntersection, ray_intersection);

    #if INDIRECT_ILLUMINATION == 1
	// DIFFUSE OBJECTS, Indirect Illumination
	if (material.getSpecular() == 0)
		pointColor += survival * diffuseInterreflect(ray, closestIntersection, depth);

	// GLOSSY OBJECTS, Indirect Illumination
    else
	{
		double rrMult;
		if (glossyRussianRoulette(material.getDiffuse(), material.getSpecular(), rrMult))
			pointColor += survival * (1.0/(1-1.0/rrMult)) * diffuseInterreflect(ray, closestIntersection, depth);
		else
		    pointColor += survival * rrMult * specularInterreflect(ray, closestIntersection, depth);
    }
    #endif

	return pointColor;
}

bool RayTracer::russianRoulette(const float& weight, double& survivorMult)
{
	survivorMult = 1.0/weight;
    return (randf() > weight);
}

bool RayTracer::glossyRussianRoulette(const float& kS, const float& kD, double& survivorMult)
{
    double p = kS/(kS + kD);
	survivorMult = 1.0/p;
    return (randf() > p);
}

Vec3Df RayTracer::directIllumination(const struct IntersectionStruct& intersection, const Ray & ray)
{
    Scene * scene = Scene::getInstance ();
    std::vector<Object> & objects = scene->getObjects();
    std::vector<Light*> & lights = scene->getLights();
	Material & material = objects[intersection.object_id].getMaterial();
    float diffuse = material.getDiffuse();
    float specular = material.getSpecular();
    Vec3Df color = material.getColor();
    float shininess = material.getShininess();
    
	Vec3Df pointColor(0,0,0);

	for (unsigned int i = 0; i < lights.size(); i++) {
		Vec3Df intensity;
		Vec3Df lightincidence;
		if (lights[i]->sample(intersection.p, intersection.n, intensity, lightincidence, objects)) {
			if (diffuse)
				pointColor += (diffuse/M_PI) * intensity * color;
			if (specular)
			{
                Vec3Df r = reflect(lightincidence,intersection.n);
				r.normalize();
				Vec3Df v = -ray.getDirection();
				v.normalize();
				pointColor += (specular*(shininess+1)/(2*M_PI)) * pow(Vec3Df::dotProduct(r,v), shininess) * intensity * color;
			}
		}
	}

	return pointColor;
}

Vec3Df RayTracer::specularInterreflect(Ray& ray, const struct IntersectionStruct& intersection, int depth)
{
    Scene * scene = Scene::getInstance ();
    std::vector<Object> & objects = scene->getObjects();
	Material & material = objects[intersection.object_id].getMaterial();
    float specular = material.getSpecular();
    Vec3Df color = material.getColor();
    float shininess = material.getShininess();
    
	Vec3Df reflectDir = reflect(ray.getDirection(), intersection.n);
	reflectDir.normalize();
	
	Ray specRay = Ray(intersection.p, reflectDir);
    specRay.shuffleInHemisphere();
    Vec3Df rayDir = specRay.getDirection();
    
    Vec3Df specColor = pathtrace(specRay, depth + 1);
    // I think this is the way the color of the material impacts the incoming light, to be double-checked.
    specColor[0] *= color[0];
    specColor[1] *= color[1];
    specColor[2] *= color[2];
	// Probablity: 1/(2PI) -- (1/probability)*cos(theta)*brdf*radiancealongray
	return pow(Vec3Df::dotProduct(rayDir,reflectDir), shininess) * Vec3Df::dotProduct(rayDir, intersection.n) * specular * (shininess+1) * specColor;	
}

Vec3Df RayTracer::diffuseInterreflect(Ray& ray, const struct IntersectionStruct& intersection, int depth)
{
    Scene * scene = Scene::getInstance ();
    std::vector<Object> & objects = scene->getObjects();
	Material & material = objects[intersection.object_id].getMaterial();
    float diffuse = material.getDiffuse();
    Vec3Df color = material.getColor();
    
    
	Ray diffRay = Ray(intersection.p, intersection.n);
	diffRay.shuffleInHemisphere();    
	Vec3Df rayDir = diffRay.getDirection();
    
	Vec3Df diffColor = pathtrace(diffRay, depth + 1);
	// I think this is the way the color of the material impacts the incoming light, to be double-checked.
    diffColor[0] *= color[0];
    diffColor[1] *= color[1];
    diffColor[2] *= color[2];
	// Probablity: 1/(2PI) -- (1/probability)*cos(theta)*brdf*radiancealongray
	return 2 * Vec3Df::dotProduct(intersection.n, rayDir) * diffuse * diffColor;
}

Vec3Df inline RayTracer::reflect(const Vec3Df& dir, const Vec3Df& normal)
{
	return dir - (2 * (Vec3Df::dotProduct(dir, normal) * normal));
}