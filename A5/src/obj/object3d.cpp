#include "object3d.h"
#include "transform.h"

void Object3D::insertIntoGrid(Grid *g, Matrix *m) {
    if (m != nullptr && boundingBox != nullptr) {
        // only handles transformations && discards planes etc.
        auto transformedBB = Transform::TransformBoundingBox(*this, *m);
        int iMin[3], iMax[3];
        g->getGridCellIndex(transformedBB.getMin(), iMin);
        g->getGridCellIndex(transformedBB.getMax(), iMax);
        for (int i = iMin[0]; i <= iMax[0]; ++i) {
            for (int j = iMin[1]; j <= iMax[1]; ++j) {
                for (int k = iMin[2]; k <= iMax[2]; ++k) {
                    g->cells[i][j][k].emplace_back(this);
                }
            }
        }
    }
}
