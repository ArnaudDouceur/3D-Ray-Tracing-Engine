#include "KdTree.h"
using namespace std;
#define KDNODE_SIZE 64
#define EPSILON 0.000001
// Naive implementation for test purpose
void KdTree::choosePlane(const std::vector<Vec3Df> &V) {
    splitPosition = (bbox.getMin()[splitAxis] + bbox.getMax()[splitAxis])/2.0;
}

void KdTree::chooseSAH (const std::vector<Vec3Df> &V, const std::vector<Triangle> &T, unsigned int axis)
{
    float bestCost = 1000000.f;
    float cost;
    for (unsigned int i=0; i<T.size(); ++i)
    {
        float left_extreme = V[T[i].getVertex(0)][axis];
        float right_extreme = V[T[i].getVertex(0)][axis];

        for (unsigned int j=1; j<3; ++j)
        {
            if (left_extreme > V[T[i].getVertex(j)][axis])
                left_extreme = V[T[i].getVertex(j)][axis];
            if (right_extreme < V[T[i].getVertex(j)][axis])
                right_extreme = V[T[i].getVertex(j)][axis];
        }
        
        cost = calculateCostSAH (left_extreme);

        if (cost < bestCost)
        {
        	bestCost = cost;
            splitPosition = left_extreme;
        }

        cost = calculateCostSAH (right_extreme);

        if (cost < bestCost)
        {
	        bestCost = cost;
            splitPosition = right_extreme;
        }
    }
}

float KdTree::calculateCostSAH (float splitPos)
{
    float leftArea = 0, rightArea = 0;
    float leftCount = 0, rightCount = 0;
    float costTravel = 1.f;
    float costIntersection = 1.f;
    /* PSEUDO code */    
    /* calculate bbox from min to left_extreme
     * Then right extreme to max ?
	leftarea = calculateleftarea( splitpos )
	rightarea = calculaterightarea( splitpos )
	leftcount = calculateleftprimitivecount( splitpos )
	rightcount = calculaterightprimitivecount( splitpos )
*/
    return costTravel + costIntersection * (leftArea * leftCount + rightArea * rightCount);
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
