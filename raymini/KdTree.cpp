#include "KdTree.h"
using namespace std;
#define KDNODE_SIZE 64
#define EPSILON 0.000001
// Naive implementation for test purpose
void KdTree::choosePlane(const std::vector<Vec3Df> &V) {
    splitPosition = (bbox.getMin()[splitAxis] + bbox.getMax()[splitAxis])/2.0;
}

// NOTE: We need to set triangles and bbox before calling build
void KdTree::build(const std::vector<Vec3Df> &V)
{
    unsigned int trianglesCount = triangles.size();
    
    if (trianglesCount <= KDNODE_SIZE) {
        return;
    }
        
    choosePlane(V);
    
    left = new KdTree();
    right = new KdTree();
    
    right->splitAxis = left->splitAxis = (splitAxis+1)%3;
    
    Vec3Df bbv = bbox.getMax();
    bbv[splitAxis] = splitPosition;
    left->bbox = BoundingBox(bbox.getMin(), bbv);
    bbv = bbox.getMin();
    bbv[splitAxis] = splitPosition;
    right->bbox = BoundingBox(bbv, bbox.getMax());
    
    // TODO : whe should avoid third test if first or second is true
    
    while(!triangles.empty()) {
        
        Triangle t = triangles.back();
        triangles.pop_back();
        
        float v0_position = V[t.getVertex(0)][splitAxis];
        float v1_position = V[t.getVertex(1)][splitAxis];
        float v2_position = V[t.getVertex(2)][splitAxis];
        
        if(v0_position < splitPosition + EPSILON || v1_position < splitPosition + EPSILON || v2_position < splitPosition + EPSILON)
            left->triangles.push_back(t);
        if(v0_position > splitPosition - EPSILON || v1_position > splitPosition - EPSILON  || v2_position > splitPosition - EPSILON )
            right->triangles.push_back(t);
    }
    
    left->build(V);
    right->build(V);
}