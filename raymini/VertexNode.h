#ifndef VERTEXNODE_H
#define VERTEXNODE_H

#include "VertexCell.h"
#include "KDNode.h"
#include <vector>

class VertexNode : public KDNode<VertexCell> {
public:
	VertexNode();
	VertexNode(const std::vector<Vertex>& V);

	/**
     * 	Rule to fine tune simplification.
     * 	angleDeg is the maximum angle between the normals' mean and max.
     */
	static void setAngleNormalsLimit(float angleDeg);

protected:
	static float cosAngleLimit;
	virtual VertexCell* buildNewCell(const std::vector<Vertex>& V) const;
	virtual KDNode<VertexCell>* newKDNode() const;
	virtual bool isSplittable(const std::vector<Vertex>& V) const;
};
#endif /* VERTEXNODE_H */
