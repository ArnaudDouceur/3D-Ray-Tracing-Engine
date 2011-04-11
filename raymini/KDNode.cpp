#include "KDNode.h"

using namespace std;

template<class T_Cell> KDNode<T_Cell>::KDNode() {
	init();
}

template<class T_Cell> void KDNode<T_Cell>::init() {
	position = 0.f;
	axis = 0;
	content = NULL;
	childA = NULL;
	childB = NULL;
}

template<class T_Cell> KDNode<T_Cell>::~KDNode() {
	if(content != NULL)
		delete content;
	if(childA != NULL)
		delete childA;
	if(childB != NULL)
		delete childB;
}

template<class T_Cell> void KDNode<T_Cell>::build(const vector<Vertex>& V) {
	Vec3Df BBmin, BBmax;
	Mesh::getBoundingBox(V, BBmin, BBmax);
	build(V, BBmin, BBmax);
}


// Building the KDTree
template<class T_Cell> void KDNode<T_Cell>::build(vector<Vertex> V, const Vec3Df& BBmin, const Vec3Df& BBmax) {
	if(!isSplittable(V))
	{
		cout << "Create leaf with " << V.size() << " points" << endl;
		content = buildNewCell(V);
	}
	else
	{
	    // Build recursively
		vector<Vertex> Va, Vb;
		vector<Vertex>::size_type median;

		// Split array
		median = splitBB(V, BBmax - BBmin);
		Va.resize(median);
		copy(V.begin(), V.begin() + median, Va.begin());
		Vb.resize(V.size() - median);
		copy(V.begin() + median, V.end(), Vb.begin());
		V.clear();

		// Intermediate BB
		Vec3Df BBmaxA = BBmax;
		Vec3Df BBminB = BBmin;
		BBmaxA[axis] = Va[Va.size()-1].getPos()[axis];
		BBminB[axis] = Vb[0].getPos()[axis];

		// Recursif call
		childA = newKDNode();
		childA->build(Va, BBmin, BBmaxA);
		childB = newKDNode();
		childB->build(Vb, BBminB, BBmax);
	}
}


template <class T_Cell> T_Cell* KDNode<T_Cell>::getContent(const Vec3Df& pt, unsigned int maxDepth) const {
	if(maxDepth == 1)
		return content;
	else
	{
		if(pt[axis] < position)
			return (childA == NULL) ? content : childA->getContent(pt, maxDepth-1);
		else if(pt[axis] > position)
			return (childB == NULL) ? content : childB->getContent(pt, maxDepth-1);
		 else if(childA == NULL && childB == NULL)
		 	return content;
		 else
		 	throw std::invalid_argument("Search through kd-tree : points on the separation !\nNot yet implemented\n");
	}
}

template <class T_Cell> unsigned long KDNode<T_Cell>::countContentNodes(bool onlyLeaves) {
	unsigned long nb = 0;
	if(content != NULL)
		if(!onlyLeaves || (childA==NULL && childB==NULL))
			++nb;
	if(childA != NULL)
		nb += childA->countContentNodes(onlyLeaves);
	if(childB != NULL)
		nb += childB->countContentNodes(onlyLeaves);
	return nb;
}

// Seperate points in two
template<class T_Cell> vector<Vertex>::size_type KDNode<T_Cell>::splitBB(vector<Vertex>& V, const Vec3Df& BBsize) {
	vector<Vertex>::size_type rankL, rankR, rank;

	// Main axis
	axis = (BBsize[1] > BBsize[0]) ? 1 : 0;
	axis = (BBsize[2] > BBsize[axis] ? 2 : axis);

	// Sort points
	if(axis == 0)
		sort(V.begin(), V.end(), KDNode<T_Cell>::compareX);
	else if(axis == 1)
		sort(V.begin(), V.end(), KDNode<T_Cell>::compareY);
	else
		sort(V.begin(), V.end(), KDNode<T_Cell>::compareZ);

	// Seperate so that no point is above
	for(rankL = V.size() / 2 ; rankL>0 && V[rankL].getPos()[axis]==V[rankL-1].getPos()[axis] ; rankL--);
	for(rankR = V.size() / 2 ; rankR<V.size() && V[rankR].getPos()[axis]==V[rankR-1].getPos()[axis] ; rankR++);
	rank = (V.size() - rankR >= rankL) ? rankR : rankL;

	position = (double) (V[rank-1].getPos()[axis] + V[rank].getPos()[axis]) / 2.;
	return rank;
}


// Comparison functions
template<class T_Cell> bool KDNode<T_Cell>::compareX(const Vertex& a, const Vertex& b) {
	return (a.getPos()[0] < b.getPos()[0]);
}

template<class T_Cell> bool KDNode<T_Cell>::compareY(const Vertex& a, const Vertex& b) {
	return (a.getPos()[1] < b.getPos()[1]);
}

template<class T_Cell> bool KDNode<T_Cell>::compareZ(const Vertex& a, const Vertex& b) {
	return (a.getPos()[2] < b.getPos()[2]);
}
