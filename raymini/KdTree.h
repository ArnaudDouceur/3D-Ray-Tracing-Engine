#ifndef KDTREE_H
#define KDTREE_H

#include "Vec3D.h"
#include "Vertex.h"
#include "Triangle.h"
#include "BoundingBox.h"
#include <algorithm>
#include <vector>

class KdTree {

public:
	inline KdTree(const BoundingBox &b, const std::vector<Triangle> &t)
    {
		triangles = t;
		bbox = b;
    }

	inline KdTree() {}
        
    //TODO
	//inline virtual ~KdTree();
    //inline void operator=(const KdTree &tree);

    void build(const std::vector<Vec3Df> & V);
	void choosePlane(const std::vector<Vec3Df> &V);


private:
    uint8_t splitAxis;
	float splitPosition;
	BoundingBox bbox;
    KdTree *left;
	KdTree *right;
    std::vector<Triangle> triangles;
};
#endif /* KD_TREE */
