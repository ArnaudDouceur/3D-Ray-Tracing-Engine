#include "VertexCell.h"

using namespace std;
// Cells
unsigned long VertexCell::indexCount = 0;

VertexCell::VertexCell() : p(0.,0.,0.) {
	index = VertexCell::indexCount++;
}

VertexCell::VertexCell(const Vec3Df& _p) : p(_p) {
	index = VertexCell::indexCount++;
}

VertexCell::VertexCell(const vector<Vertex>& V) : p(0.,0.,0.) {
	index = VertexCell::indexCount++;
	for(vector<Vertex>::size_type i=0; i<V.size(); ++i)
		p += V[i].getPos();
	p /= V.size();
}

unsigned long VertexCell::resetIndexCount(unsigned long newValue) {
	unsigned long oldValue = VertexCell::indexCount;
	VertexCell::indexCount = newValue;
	return oldValue;
}


