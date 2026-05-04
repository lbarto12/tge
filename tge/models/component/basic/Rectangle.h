#pragma once

#include "../Component.h"

namespace tge {
class Rectangle : public ComponentBase {
public:
    TGE_BASIC_CONSTRUCT(Rectangle);
    Rectangle(const Vector2i& size, const wchar_t fill = '#') : ComponentBase(), fill(fill) { this->SetSize(size); }
    Rectangle(const wchar_t fill) : ComponentBase(), fill(fill) {}

    void Render() override {
        for (int i = 0; i < this->size.y; ++i) {
            for (int j = 0; j < this->size.x; ++j) {
                render.DrawCell({this->GetPosition().x + j, this->GetPosition().y + i}, this->fill);
            }
        }
        render.SwapBuffer();
    }

    void SetFill(const wchar_t fill) { this->fill = fill; }

private:
    wchar_t fill;
};
} // namespace tge
