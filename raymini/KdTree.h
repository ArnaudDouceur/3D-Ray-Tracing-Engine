#ifndef KDTREE_H
#define KDTREE_H

#include "Vec3D.h"
#include "Vertex.h"
#include "Triangle.h"
#include "BoundingBox.h"
#include <algorithm>
#include <vector>

class KdTree {
    friend class Ray;
public:
    inline KdTree()
    {
        for(int i = 0 ; i < 2 ; ++i)
            child[i] = NULL;
        subTree = NULL;
    }

    inline KdTree(const KdTree &t)
    {
        for(int i = 0 ; i < 2 ; ++i)
            child[i] = NULL;
        subTree = NULL;
        *this = t;
    }

    void build(std::vector<Vec3Df> &vertices, BoundingBox &vbox, std::vector<int>::iterator start, std::vector<int>::iterator end);

    void fill(std::vector<Vec3Df> &vertices, std::vector<Triangle> &input, int subTreeCount = 3);

    inline int depth() const
    {
        int d = 0;
        for(int i = 0 ; i < 2 ; ++i)
        {
            if (child[i])
                d = std::max(d, child[i]->depth());
        }
        if (subTree)
            d = std::max(d, subTree->depth());

        return d + 1;
    }

    inline int worse() const
    {
        int d = 0;

        for(int i = 0 ; i < 2 ; ++i)
        {
            if (child[i])
            {
                d = std::max(d, child[i]->worse());
            }
        }
        if (subTree)
        {
            d += subTree->worse();
        }
        else
        {
            d += triangles.size();
        }

        return d;
    }

    inline int max() const
    {
        int d = 0;

        for(int i = 0 ; i < 2 ; ++i)
        {
            if (child[i])
            {
                d = std::max(d, child[i]->max());
            }
        }
        if (subTree)
        {
            d = std::max(d, subTree->max());
        }
        else
        {
            d = std::max(d, int(triangles.size()));
        }

        return d;
    }

    inline int total() const
    {
        int d = 0;

        for(int i = 0 ; i < 2 ; ++i)
        {
            if (child[i])
            {
                d += child[i]->total();
            }
        }
        if (subTree)
        {
            d += subTree->total();
        }
        else
        {
            d += triangles.size();
        }

        return d;
    }

    inline float mean() const
    {
        return float(total()) / nodes();
    }


    inline int nodes() const
    {
        int d = 1;

        for(int i = 0 ; i < 2 ; ++i)
        {
            if (child[i])
            {
                d += child[i]->nodes();
            }
        }
        if (subTree)
        {
            d += subTree->nodes();
        }

        return d;
    }

    inline int size() const
    {
        int d = sizeof(*this);
        for(int i = 0 ; i < 2 ; ++i)
        {
            if (child[i])
            {
                d += child[i]->size();
            }
        }
        if (subTree)
        {
            d += subTree->size();
        }

        return d + sizeof(Triangle) * triangles.size();
    }

    static inline bool compareX(const Vec3Df& a, const Vec3Df& b)
    {
        return (a[0] < b[0]);
    }

    static inline bool compareY(const Vec3Df& a, const Vec3Df& b) 
    {
        return (a[1] < b[1]);
    }

    static inline bool compareZ(const Vec3Df& a, const Vec3Df& b) 
    {
        return (a[2] < b[2]);
    }
/*
    static inline bool compareX(const Vertex& a, const Vertex& b)
    {
        return (a.getPos()[0] < b.getPos()[0]);
    }

    static inline bool compareY(const Vertex& a, const Vertex& b) 
    {
        return (a.getPos()[1] < b.getPos()[1]);
    }

    static inline bool compareZ(const Vertex& a, const Vertex& b) 
    {
        return (a.getPos()[2] < b.getPos()[2]);
    }
*/

private:
    int splitAxis;         // Determine direction in which to split BBox
    BoundingBox bbox[2];   // BBox for children
    BoundingBox tbbox;     // BBox for all triangles contained in voxel
    KdTree *child[2];      // Left and right children
    std::vector<Triangle> triangles; //List of triangles contained in voxel
    KdTree *subTree;       //Another KdTree for extra triangles

    //TODO
    inline ~KdTree();
    inline void operator=(const KdTree &tree);

};
#endif /* KD_TREE */
