#ifndef VERTEXKDTREE_H
#define VERTEXKDTREE_H


#include <cmath>
#include <vector>

#include "Vec3D.h"
#include "Mesh.h"
#include "KDNode.h"
#include "VertexCell.h"
#include "VertexNode.h"

class VertexKDTree {
public:
	VertexNode *a;
	Mesh& mesh;
	float angleMax;

	VertexKDTree(Mesh& mesh);
    inline virtual ~VertexKDTree() {}

	// Simplify the grid
    virtual void simplify(float angleDeg);
	virtual void simplify(float angleDeg, Mesh& out_mesh);
};
#endif /* VERTEXKDTREE_H */
