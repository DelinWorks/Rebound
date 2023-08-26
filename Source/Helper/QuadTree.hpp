#pragma once

#include "Helper/Math.h"
#include <vector>

namespace SpatialDataStructures
{
    struct QTBounds {
        F64 xMin, yMin, xMax, yMax;
        QTBounds(F64 xmin, F64 ymin, F64 xmax, F64 ymax) :
            xMin(xmin), yMin(ymin), xMax(xmax), yMax(ymax) {}

        bool contains(const V2D& point) const
        {
            return (point.x >= xMin && point.x <= xMax &&
                point.y >= yMin && point.y <= yMax);
        }
    };

    template <typename T>
    class QuadTree {
    public:
        struct QTNode {
            T nestedValue;
            QuadTree<T>* subnode;

            QTNode(const T& nestedValue, QuadTree<T>* subnode)
                : nestedValue(nestedValue), subnode(subnode)
            {
            }
        };

        QuadTree() = default;
        QuadTree(const QTBounds& bounds) : bounds(bounds) {
            subnodes = new QuadTree<T>*[4];

            for (int i = 0; i < 4; i++)
                subnodes[i] = nullptr;

            float midWidth = bounds.xMax - bounds.xMin;
            float midHeight = bounds.yMax - bounds.yMin;
            if (midWidth + midHeight < 1000)
                isLeaf = true;
        }

        ~QuadTree() {
            if (!isLeaf)
                for (int i = 0; i < 4; i++)
                    delete subnodes[i];
            delete[] subnodes;
        }

        void insert(T object, const V2D& position)
        {
            if (!contains(position)) return;

            F64 xMid = (bounds.xMin + bounds.xMax) / 2.0;
            F64 yMid = (bounds.yMin + bounds.yMax) / 2.0;

            bool b1 = QTBounds(bounds.xMin, bounds.yMin, xMid, yMid).contains(position);
            bool b2 = QTBounds(xMid, bounds.yMin, bounds.xMax, yMid).contains(position);
            bool b3 = QTBounds(bounds.xMin, yMid, xMid, bounds.yMax).contains(position);
            bool b4 = QTBounds(xMid, yMid, bounds.xMax, bounds.yMax).contains(position);

            if (!isLeaf && (b1 || b2 || b3 || b4)) {
                split();
                if (b1) subnodes[0]->insert(object, position);
                else if (b2) subnodes[1]->insert(object, position);
                else if (b3) subnodes[2]->insert(object, position);
                else if (b4) subnodes[3]->insert(object, position);
            }
            else objects.push_back(QTNode(object, this));
        }

        bool contains(const V2D& point) const
        {
            return (point.x >= bounds.xMin && point.x <= bounds.xMax &&
                point.y >= bounds.yMin && point.y <= bounds.yMax);
        }

    private:
        QTBounds bounds;
        ::std::vector<QTNode> objects;
        QuadTree<T>** subnodes = nullptr;
        bool isLeaf = false;

        void split()
        {
            if (isLeaf) return;

            F64 xMid = (bounds.xMin + bounds.xMax) / 2.0;
            F64 yMid = (bounds.yMin + bounds.yMax) / 2.0;

            subnodes[0] = new QuadTree<T>(QTBounds(bounds.xMin, bounds.yMin, xMid, yMid));
            subnodes[1] = new QuadTree<T>(QTBounds(xMid, bounds.yMin, bounds.xMax, yMid));
            subnodes[2] = new QuadTree<T>(QTBounds(bounds.xMin, yMid, xMid, bounds.yMax));
            subnodes[3] = new QuadTree<T>(QTBounds(xMid, yMid, bounds.xMax, bounds.yMax));
        }
    };
}
