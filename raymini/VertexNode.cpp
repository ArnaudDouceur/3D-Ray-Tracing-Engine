#include "VertexNode.h"

using namespace std;

// Tree nodes
float VertexNode::cosAngleLimit = 0.9; // Default value

void VertexNode::setAngleNormalsLimit(float angleDeg) {
	// atan(1) / 45 = (Pi/4) / 45 = 2.PI/360
	VertexNode::cosAngleLimit = cos(angleDeg * atan(1.) / 45.);
}

VertexNode::VertexNode() {}

VertexNode::VertexNode(const vector<Vertex>& V) {
	build(V);
}

VertexCell* VertexNode::buildNewCell(const vector<Vertex>& V) const {
	return new VertexCell(V);
}

KDNode<VertexCell>* VertexNode::newKDNode() const {
	return new VertexNode();
}


bool VertexNode::isSplittable(const vector<Vertex>& V) const {
	Vec3Df n(0.,0.,0.);
	vector<Vertex>::size_type k;

	// Normals' mean
	for(k=0; k<V.size(); ++k)
		n += V[k].getNormal();
	n.normalize();

	// Comparing offset to threshold
	for(k=0; k<V.size(); ++k)
		if(Vec3Df::dotProduct(n, V[k].getNormal()) < VertexNode::cosAngleLimit)
			return true;
	return false;
}

