#include "KdTree.h"
using namespace std;
#define KDNODE_SIZE 12

// Naive implementation for test purpose
void KdTree::choosePlane(const std::vector<Vec3Df> &V) {
    splitPosition = (bbox.getMin()[splitAxis] + bbox.getMax()[splitAxis])/2.0;
}

// NOTE: We need to set triangles and bbox before calling build
void KdTree::build(const std::vector<Vec3Df> &V)
{
    unsigned int trianglesCount = triangles.size();
    
    if (trianglesCount <= KDNODE_SIZE) {
        cout << "Creating a leaf with : " << trianglesCount << endl;
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
    left->bbox = BoundingBox(bbv, bbox.getMax());
    
    // TODO : whe should avoid third test if first or second is true
    for(unsigned int i = 0; i < triangles.size(); i++) {
        
        float v0_position = V[triangles[i].getVertex(0)][splitAxis];
        float v1_position = V[triangles[i].getVertex(1)][splitAxis];
        float v2_position = V[triangles[i].getVertex(2)][splitAxis];
        
        if(v0_position < splitPosition || v1_position < splitPosition || v2_position < splitPosition)
            left->triangles.push_back(triangles[i]);
        if(v0_position > splitPosition || v1_position > splitPosition || v2_position > splitPosition)
            right->triangles.push_back(triangles[i]);
        if(v0_position == splitPosition || v1_position == splitPosition || v2_position == splitPosition) {
            left->triangles.push_back(triangles[i]);
            right->triangles.push_back(triangles[i]);
        }
    }
    
    left->build(V);
    right->build(V);    
}