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
    // Places a character `what` at a given x, y location `where`.
    void DrawCell(Vector2i where, wchar_t what) {
        cellsDrawn.push_back(where);
        tge::render::Terminal::MoveTo(where.x, where.y);
        std::printf("%lc", static_cast<wint_t>(what));
        std::fflush(stdout);
    }

    void StartForegroundColor(Color c) const { render::Terminal::SetForeground(c); }
    void StartBackgroundColor(Color c) const { render::Terminal::SetBackground(c); }
    void StopStyling() const { render::Terminal::ResetStyle(); }

    // Finds, and erases all cells that
    //  1) were drawn `before` the last call of `SwapBuffer` and
    //  2) were not re-drawn since the last call of `SwapBuffer`.
    //
    // This limits cell clearing to mitigate render-flickering.
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

    // Clears both the current and previous object buffers. Does *not* erase any currently rendered cells.
    void ClearBuffer() {
        this->previousBuffer.clear();
        this->cellsDrawn.clear();
    }

private:
    std::vector<Vector2i> cellsDrawn;
    std::vector<Vector2i> previousBuffer;
};
} // namespace tge::internal::components
