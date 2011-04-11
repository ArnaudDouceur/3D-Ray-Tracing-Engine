#ifndef KDNODE_H
#define KDNODE_H

#include "BoundingBox.h"
#include "Vertex.h"
#include "Vec3D.h"
#include "Scene.h"
#include "Object.h"

class KdNode {
public:

    KdNode();
    virtual ~KdNode();
    inline int getSplitAxis()
    {
        return splitAxis;
    }

    inline KdNode* getParent()
    {
        return parent;
    }

    void setAngleNormalsLimit(float angleDeg);
    static bool compareX(const Vertex& a, const Vertex& b);
	static bool compareY(const Vertex& a, const Vertex& b);
	static bool compareZ(const Vertex& a, const Vertex& b);
    
    bool splittable(const std::vector<Vertex> &V) const;
    void build(std::vector<Vertex> &V, const Vec3Df &BBmin, const Vec3Df &BBmax);
    void buildKdTree(Scene& scene);
    std::vector<Vertex>::size_type splitBB(std::vector<Vertex> &V, const Vec3Df &BBsize);
    

private:
    int splitAxis;
    double splitAxisPosition;
    KdNode *parent, *left, *right;

	// TODO implement
	KdNode(const KdNode&);
    KdNode operator=(const KdNode&);
};
#endif /* KDNODE_H */
