#include "KdNode.h"

float cosAngleLimit = 0.9;

using namespace std;

KdNode::KdNode()
{
    splitAxis = 0; // X-axis by default
    splitAxisPosition = 0.f;
    parent = NULL;
    left = NULL;
    right = NULL;
}

KdNode::~KdNode()
{
    if (this != NULL)
        delete this;
    if (left != NULL)
            delete left;
    if (right != NULL)
            delete right;
}

void KdNode::setAngleNormalsLimit(float angleDeg) {
	// atan(1) / 45 = (Pi/4) / 45 = 2.PI/360
	cosAngleLimit = cos(angleDeg * atan(1.) / 45.);
}

bool KdNode::splittable(const vector<Vertex> &V) const
{
    Vec3Df n(0.,0.,0.);
	vector<Vertex>::size_type k;

	// Normals' mean
	for(k=0; k<V.size(); ++k)
		n += V[k].getNormal();
	n.normalize();

	// Comparing offset to threshold
	for(k=0; k<V.size(); ++k)
		if(Vec3Df::dotProduct(n, V[k].getNormal()) < cosAngleLimit)
			return true;
	return false;

}


// Comparison functions
bool KdNode::compareX(const Vertex& a, const Vertex& b) 
{
	return (a.getPos()[0] < b.getPos()[0]);
}

bool KdNode::compareY(const Vertex& a, const Vertex& b) 
{
	return (a.getPos()[1] < b.getPos()[1]);
}

bool KdNode::compareZ(const Vertex& a, const Vertex& b) 
{
	return (a.getPos()[2] < b.getPos()[2]);
}

vector<Vertex>::size_type KdNode::splitBB(vector<Vertex> &V, const Vec3Df &BBsize)
{
    vector<Vertex>::size_type rankL, rankR, rank;

	// Main axis
	splitAxis = (BBsize[1] > BBsize[0]) ? 1 : 0;
	splitAxis = (BBsize[2] > BBsize[splitAxis] ? 2 : splitAxis);

	// Sort points according to chosen Axis (X, Y or Z)
	if(splitAxis == 0)
		sort(V.begin(), V.end(), KdNode::compareX);
	else if(splitAxis == 1)
		sort(V.begin(), V.end(), KdNode::compareY);
	else
		sort(V.begin(), V.end(), KdNode::compareZ);

	// Seperate so that no point is above
	for(rankL = V.size() / 2 ; rankL>0 && V[rankL].getPos()[splitAxis]==V[rankL-1].getPos()[splitAxis]; rankL--);
	for(rankR = V.size() / 2 ; rankR<V.size() && V[rankR].getPos()[splitAxis]==V[rankR-1].getPos()[splitAxis]; rankR++);
	rank = (V.size() - rankR >= rankL) ? rankR : rankL;

	splitAxisPosition = (double) (V[rank-1].getPos()[splitAxis] + V[rank].getPos()[splitAxis]) / 2.;
	return rank;
}

void KdNode::build(vector<Vertex> &V, const Vec3Df &BBmin, const Vec3Df &BBmax)
{
     if(!splittable(V))
    {
        //TODO end building ?
        cout << "Kd-Tree is built" << endl;
        return;
    }
    else
    {
        // Build recursively
		vector<Vertex> Va, Vb;
		vector<Vertex>::size_type median;
        
        // Split V into Va and Vb
		median = splitBB(V, BBmax - BBmin);
		Va.resize(median);
		copy(V.begin(), V.begin() + median, Va.begin());
		Vb.resize(V.size() - median);
		copy(V.begin() + median, V.end(), Vb.begin());
		V.clear();

		// Intermediate BB
		Vec3Df BBmaxA = BBmax;
		Vec3Df BBminB = BBmin;
		BBmaxA[splitAxis] = Va[Va.size()-1].getPos()[splitAxis];
		BBminB[splitAxis] = Vb[0].getPos()[splitAxis];

		// Recursif call
        // TODO
		left = new KdNode();
		left->build(Va, BBmin, BBmaxA);
		right = new KdNode();
		right->build(Vb, BBminB, BBmax);
	}

}

void KdNode::buildKdTree(Scene& scene)
{
    vector<Vertex> V; // Array containing all vertices
    int size = 0;
    vector<Object> &objects = scene.getObjects();
    const BoundingBox &Bbox = scene.getBoundingBox();

    // Fill V
    for(vector<Object>::size_type i=0; i<objects.size(); ++i)
    {
        vector<Vertex> Vtmp = objects[i].getMesh().getVertices();
        
        for(vector<Vertex>::size_type j=0; j<Vtmp.size(); ++j)
        {
            size += Vtmp.size();
            V.resize(size);
            copy(Vtmp.begin(), Vtmp.end(), V.end());
        }
    }
    
    build(V, Bbox.getMin(), Bbox.getMax());
   
}

