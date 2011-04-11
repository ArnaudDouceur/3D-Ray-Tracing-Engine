#ifndef VERTEXCELL_H
#define VERTEXCELL_H

#include "Vec3D.h"
#include "Vertex.h"
#include <vector>

class VertexCell {
public:
	Vec3Df p;
	unsigned long index;

	VertexCell();
	VertexCell(const Vec3Df&);
	VertexCell(const std::vector<Vertex>&);


	/**
     * Resets index counter with newValue.
     * Returns old value.
     */
	static unsigned long resetIndexCount(unsigned long newValue = 0);
	static unsigned long indexCount;
};
#endif /* VERTEXCELL_H */
