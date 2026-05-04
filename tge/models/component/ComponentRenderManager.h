#pragma once
#include <algorithm>
#include <cstdio>
#include <cwchar>
#include <vector>

#include "../../render/Terminal.h"
#include "../util/Vector2.h"

namespace tge::internal::components {
class ComponentRenderManager {
public:
    void DrawCell(Vector2i where, wchar_t what) {
        cellsDrawn.push_back(where);
        tge::render::Terminal::MoveTo(where.x, where.y);
        std::printf("%lc", static_cast<wint_t>(what));
        std::fflush(stdout);
    }

    void SwapBuffer() {
        for (const Vector2i& v : previousBuffer) {

            // No need to rerender
            if (std::find(cellsDrawn.begin(), cellsDrawn.end(), v) != cellsDrawn.end()) {
                continue;
            }

            tge::render::Terminal::MoveTo(v.x, v.y);
            std::fputs(" ", stdout);
            std::fflush(stdout);
        }

        previousBuffer = cellsDrawn;
        cellsDrawn.clear();
    }

private:
    std::vector<Vector2i> cellsDrawn;
    std::vector<Vector2i> previousBuffer;
};
} // namespace tge::internal::components
