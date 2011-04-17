#include "KdTree.h"

#define KDNODE_SIZE 12


// Build Kd-Tree
void KdTree::build(std::vector<Vec3Df> &vertices, BoundingBox &vbox, std::vector<int>::iterator start, std::vector<int>::iterator end)
{
    const int T = end - start;
    if (T <= KDNODE_SIZE)
        return;

    // Compute split direction
    const Vec3Df BBsize = vbox.getMax() - vbox.getMin();
    // Determine largest direction
    splitAxis = (BBsize[1] > BBsize[0]) ? 1 : 0;
    splitAxis = (BBsize[2] > BBsize[splitAxis] ? 2 : splitAxis);


    // Sort points according to chosen Axis (X, Y or Z)
    if(splitAxis == 0)
        std::sort(vertices.begin()+(*start), vertices.begin()+(*end-1), KdTree::compareX);
    else if(splitAxis == 1)
        std::sort(vertices.begin()+(*start), vertices.begin()+(*end-1), KdTree::compareY);
    else
        std::sort(vertices.begin()+(*start), vertices.begin()+(*end-1), KdTree::compareZ);

    // Split array in two
    std::vector<int>::iterator m = start;

    float bestScore = vbox.getArea() * T * 10.0f;
    std::vector<float> bboxleft;
    std::vector<float> bboxright;

    bbox[0] = BoundingBox(vertices[*start]);
    for(std::vector<int>::iterator i = start ; i != end ; ++i)
    {
        bbox[0].extendTo(vertices[*i]);
        bboxleft.push_back(bbox[0].getArea());
    }

    bbox[1] = BoundingBox(vertices[*(end-1)]);
    for(std::vector<int>::iterator i = end-1 ; i > start ; --i)
    {
        bbox[1].extendTo(vertices[*i]);
        bboxright.push_back(bbox[1].getArea());
    }
    std::vector<float>::iterator bl = bboxleft.begin();
    std::vector<float>::reverse_iterator br = bboxright.rbegin();
    for(std::vector<int>::iterator i = start + 1 ; i != end ; ++i, ++br, ++bl)
    {
        const float score = *bl * (i - start) + *br * (end - i);
        if (score < bestScore)
        {
            bestScore = score;
            m = i;
        }
    }

    if (m - start < T / 8 || end - m < T / 8)
        m = start + T / 2;

    child[0] = new KdTree();
    child[1] = new KdTree();

    // Compute BBox for sub-tree
    bbox[0] = BoundingBox(vertices[*start]);
    for(std::vector<int>::iterator it = start ; it != m ; ++it)
        bbox[0].extendTo(vertices[*it]);

    bbox[1] = BoundingBox(vertices[*m]);
    for(std::vector<int>::iterator it = m ; it != end ; ++it)
        bbox[1].extendTo(vertices[*it]);

    child[0]->build(vertices, bbox[0], start, m);
    child[1]->build(vertices, bbox[1], m, end);

}


void KdTree::fill(std::vector<Vec3Df> &vertices, std::vector<Triangle> &input, int subTreeCount)
{
    std::vector<Triangle> Tmp[2];
    for(unsigned int i = 0; i < input.size(); ++i)
    {
        bool found = false;
        for(int j = 0 ; j < 2 ; ++j)
        {
            if (!child[j])
                continue;

            const bool A = bbox[j].contains(vertices[input[i].getVertex(0)]);
            const bool B = bbox[j].contains(vertices[input[i].getVertex(1)]);
            const bool C = bbox[j].contains(vertices[input[i].getVertex(2)]);

            if (A && B && C)
            {
                Tmp[j].push_back(input[i]);
                found = true;
                break;
            }
        }
        if (!found)
            triangles.push_back(input[i]);
    }
    if (!triangles.empty())
    {
        tbbox = BoundingBox(vertices[triangles.front().getVertex(0)]);
        for(unsigned int i = 0 ; i < triangles.size() ; ++i)
        {
            tbbox.extendTo(vertices[triangles[i].getVertex(0)]);
            tbbox.extendTo(vertices[triangles[i].getVertex(1)]);
            tbbox.extendTo(vertices[triangles[i].getVertex(2)]);
        }
        // Too many triangles in voxel... We subdivide again!
        if (triangles.size() > KDNODE_SIZE && subTreeCount > 0 && (!Tmp[0].empty() || !Tmp[1].empty()))
        {
            std::vector<int> vtx;
            std::vector<Triangle> tmp;
            for(unsigned int i = 0; i < triangles.size() ; ++i)
            {
                vtx.push_back(triangles[i].getVertex(0));
                vtx.push_back(triangles[i].getVertex(1));
                vtx.push_back(triangles[i].getVertex(2));
                tmp.push_back(triangles[i]);
                
            }
            // Remove repeats at this stage
            std::sort(vtx.begin(), vtx.end());
            vtx.erase(std::unique(vtx.begin(), vtx.end()), vtx.end());
            // Create a sub-tree if need be
            if (vtx.size() > KDNODE_SIZE)
            {
                subTree = new KdTree();
                subTree->build(vertices, tbbox, vtx.begin(), vtx.end());
                subTree->fill(vertices, tmp, subTreeCount-1);
                triangles.clear();
            }
        }
    }

    for(int i = 0 ; i < 2 ; ++i)
        if (child[i] && !Tmp[i].empty())
            child[i]->fill(vertices, Tmp[i], subTreeCount);
}

