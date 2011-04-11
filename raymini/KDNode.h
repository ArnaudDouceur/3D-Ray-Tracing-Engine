#ifndef KDNODE_H
#define KDNODE_H


#include <vector>
#include <algorithm>
#include <stdexcept>

#include "Vertex.h"
#include "Vec3D.h"
#include "Mesh.h"
#include "VertexCell.h"


template<class T_Cell> class KDNode {
public:
	// Split axis and position
	int axis;
	double position;
	// Content of this node (only for a leaf)
	T_Cell *content;
	// Children (for non-leaf nodes)
	KDNode<T_Cell> *childA, *childB;

    /**
     * Constructeurs and destroyers
     */
	void init();
	KDNode();
	virtual ~KDNode();

	/**
     *  Build KDTree.
     */
    virtual void build(const std::vector<Vertex>& V);

    /**
     * Search function to find point pt
     */
	virtual T_Cell* getContent(const Vec3Df& pt, unsigned int maxDepth = 0) const;

    /**
     * Count the number of nodes with content.
     * Only count leaves if onlyLeaves is true.
     */
	unsigned long countContentNodes(bool onlyLeaves = true);

	// Abstract methods
	virtual T_Cell* buildNewCell(const std::vector<Vertex>& V) const = 0;
	virtual KDNode<T_Cell>* newKDNode() const = 0;
	virtual bool isSplittable(const std::vector<Vertex>& V) const = 0;

protected:
	virtual void build(std::vector<Vertex> V, const Vec3Df& BBmin, const Vec3Df& BBmax);
    /**
     * Arranges the vector according to the biggest dimension of the BB.
     * Gives seperation rank and affects the axis and position attributes.
     * Returns the first index of the new partition.
     */
	virtual std::vector<Vertex>::size_type splitBB(std::vector<Vertex>& V, const Vec3Df& BBsize);

	static bool compareX(const Vertex& a, const Vertex& b);
	static bool compareY(const Vertex& a, const Vertex& b);
	static bool compareZ(const Vertex& a, const Vertex& b);

private:
	// TODO implement
	KDNode(const KDNode&);
    KDNode operator=(const KDNode&);
};

#include "KDNode.cpp"

#endif /* KDNODE_H */
