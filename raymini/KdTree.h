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
    }

	inline KdTree() {
		splitAxis = 0;
	}
	
	inline unsigned int getSplitAxis() {
		return splitAxis;
	}
	
	inline float getSplitPosition() {
		return splitPosition;
	}
       
	inline BoundingBox getBbox() {
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

	
    //TODO
	//inline virtual ~KdTree();
    //inline void operator=(const KdTree &tree);

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
