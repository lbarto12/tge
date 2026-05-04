#pragma once

#include "../Component.h"

namespace tge {
class Rectangle : public ComponentBase {
public:
    TGE_BASIC_CONSTRUCT(Rectangle);
    Rectangle(const Vector2i& size) : ComponentBase() { this->SetSize(size); }

    void Render() override {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 20; ++j) {
                render.DrawCell({this->GetPosition().x + j, this->GetPosition().y + i}, '#');
            }
        }
        render.SwapBuffer();
    }
};
} // namespace tge
