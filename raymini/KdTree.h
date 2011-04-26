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
		splitAxis = 0;
		left = NULL;
		right = NULL;
    }

	inline KdTree() {
		splitAxis = 0;
		left = NULL;
		right = NULL;
	}
	
	inline unsigned int getSplitAxis() {
		return splitAxis;
	}
	
   	inline const unsigned int getSplitAxis() const {
		return splitAxis;
	}

	inline float getSplitPosition() {
		return splitPosition;
	}

   	inline const float getSplitPosition() const {
		return splitPosition;
	}

       
	inline BoundingBox getBbox() {
		return bbox;
	}

   inline const BoundingBox getBbox() const {
		return bbox;
	}

	
	inline KdTree* getLeft() {
		return left;
	}
	
	inline const KdTree* getLeft() const {
		return left;
	}

	inline KdTree* getRight() {
		return right;
	}
	
	inline const KdTree* getRight() const {
		return right;
	}
	inline std::vector<Triangle> getTriangles() {
		return triangles;
	}

   	inline const std::vector<Triangle> getTriangles() const {
		return triangles;
	}

    float calculateCostSAH (float splitPos);
    void chooseSAH (const std::vector<Vec3Df> &V, const std::vector<Triangle> &T, unsigned int axis);


	
    //TODO
	//inline virtual ~KdTree();
    /*inline void operator=(const KdTree &tree)
    {
        splitAxis = tree.getSplitAxis();
        splitPosition = tree.getSplitPosition();
        bbox = tree.getBbox();
        *left = *(tree.getLeft());
        *right = *(tree.getRight());
        triangles = tree.getTriangles();

    }*/


    void build(const std::vector<Vec3Df> & V);
	void choosePlane(const std::vector<Vec3Df> &V);


private:
    unsigned int splitAxis;
	float splitPosition;
	BoundingBox bbox;
    KdTree *left;
	KdTree *right;
    std::vector<Triangle> triangles;
};
#endif /* KD_TREE */
