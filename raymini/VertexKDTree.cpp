#include "VertexKDTree.h"

#include "Triangle.h"
#include "Vertex.h"

using namespace std;

// Tree
VertexKDTree::VertexKDTree(Mesh& _mesh) : a(NULL), mesh(_mesh) {}

void VertexKDTree::simplify(float angleDeg) {
	simplify(angleDeg, mesh);
}

void VertexKDTree::simplify(float angleDeg, Mesh& out_mesh) {
	vector<Vec3Df> V;
	vector<Triangle> T;
	Triangle t;
	VertexCell *cell;

	// initialisations
	cout << "Number of initial points : " << mesh.getVertices().size() << endl;
	VertexCell::resetIndexCount();
	VertexNode::setAngleNormalsLimit(angleDeg);

	// Build Tree
	a = new VertexNode(mesh.getVertices());

	// Fill V
	V.resize(VertexCell::indexCount);
	for(vector<Vertex>::size_type k=0; k<mesh.getVertices().size(); ++k)
	{
		cell = a->getContent(mesh.getVertices()[k].getPos());
		V[cell->index] = cell->p;
	}

	// Fill T
	T.clear();
	for(vector<Triangle>::size_type k=0; k<mesh.getTriangles().size(); ++k)
	{
		t = mesh.getTriangles()[k];
		for(int i=0;i<3; ++i)
			t.setVertex(i, a->getContent(mesh.getVertices()[t.getVertex(i)].getPos())->index);
		if(t.isRealTriangle())
			T.push_back(t);
	}

	// Export Tre
	out_mesh.buildFromPoints(V, T);
	cout << "Final number of points : " << V.size() << endl;

	// Free memory
	delete a;
	a = NULL;
    
}
