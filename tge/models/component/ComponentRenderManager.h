#pragma once
#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

#include "../../render/Terminal.h"
#include "../util/Vector2.h"

namespace tge::internal::components {
class ComponentRenderManager {
public:
    void DrawCell(Vector2i where, char what) {
        cellsDrawn.push_back(where);
        tge::render::Terminal::MoveTo(where.x, where.y);
        std::cout << what << std::flush;
    }

    void SwapBuffer() {
        std::vector<Vector2i> toErase;

        for (const Vector2i& v : previousBuffer) {

            // No need to rerender
            if (std::find(cellsDrawn.begin(), cellsDrawn.end(), v) != cellsDrawn.end()) {
                continue;
            }

            tge::render::Terminal::MoveTo(v.x, v.y);
            std::cout << " " << std::flush;
        }

        previousBuffer = cellsDrawn;
        cellsDrawn.clear();
    }

private:
    std::vector<Vector2i> cellsDrawn;
    std::vector<Vector2i> previousBuffer;
};
} // namespace tge::internal::components
