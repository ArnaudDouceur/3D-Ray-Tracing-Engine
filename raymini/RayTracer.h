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
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
