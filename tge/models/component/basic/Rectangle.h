#pragma once

#include "../../../render/Color.h"
#include "../Component.h"

namespace tge {
class Rectangle : public ComponentBase {
public:
    TGE_BASIC_CONSTRUCT(Rectangle);
    Rectangle(const Vector2i& size, const wchar_t fill = L'█') : ComponentBase(), fill(fill) { this->SetSize(size); }
    Rectangle(const wchar_t fill) : ComponentBase(), fill(fill) {}

    void Render() override {
        render.StartForegroundColor(this->GetForegroundColor());
        render.StartBackgroundColor(this->GetBackgroundColor());

        for (int i = 0; i < this->GetSize().y; ++i) {
            for (int j = 0; j < this->GetSize().x; ++j) {
                render.DrawCell({this->GetPosition().x + j, this->GetPosition().y + i}, this->fill);
            }
        }

        render.StopStyling();
        render.SwapBuffer();
    }

    void SetFill(const wchar_t fill) { this->fill = fill; }

private:
    wchar_t fill = L'█';
};
} // namespace tge
