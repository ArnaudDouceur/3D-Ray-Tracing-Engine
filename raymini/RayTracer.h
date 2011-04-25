// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <iostream>
#include <vector>
#include <QImage>
#include <QTime>

#include "Vec3D.h"
#include "Material.h"
#include "Scene.h"
#include "Ray.h"

class RayTracer : public QObject {
    Q_OBJECT
public:
    enum OptionFlag 
    {
        NONE = 0,
        ENABLE_AO = 0x2,
        ENABLE_AA = 0x4       
    };
    static int AO_RAY_COUNT;
    static int ANTIALIASING_RES;
    static RayTracer * getInstance ();
    static void destroyInstance ();

    inline const Vec3Df & getBackgroundColor () const { return backgroundColor;}
    inline void setBackgroundColor (const Vec3Df & c) { backgroundColor = c; }
    
    static inline Vec3Df brdfPhong(const Vec3Df &, const Vec3Df &, const Vec3Df &, const Material &);
    
    QImage render (const Vec3Df & camPos,
                   const Vec3Df & viewDirection,
                   const Vec3Df & upVector,
                   const Vec3Df & rightVector,
                   float fieldOfView,
                   float aspectRatio,
                   unsigned int screenWidth,
                   unsigned int screenHeight,
                   unsigned int lightChoice,
                   unsigned int flags);
signals:
	void renderDone(QTime time);
    void init (int min, int max);
    void progress (int val);
    
    
protected:
    inline RayTracer () {}
    inline virtual ~RayTracer () {}
    
private:
    Vec3Df backgroundColor;
	Vec3Df pathtrace(Ray& ray, unsigned int depth);
	bool russianRoulette(const float& weight, double& survivorMult);
	bool glossyRussianRoulette(const float& kS, const float& kD, double& survivorMult);
	Vec3Df directIllumination(const struct IntersectionStruct & intersection, const Ray & ray);
	Vec3Df specularInterreflect(Ray& ray, const struct IntersectionStruct& intersection, int depth);
	Vec3Df diffuseInterreflect(const struct IntersectionStruct&, int depth);
	Vec3Df mirrorInterreflect(const struct IntersectionStruct & intersection, Ray & ray, int depth);
	Vec3Df inline reflect(const Vec3Df& dir, const Vec3Df& normal);
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
