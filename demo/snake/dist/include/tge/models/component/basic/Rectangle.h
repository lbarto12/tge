#pragma once

#include "../../../render/Color.h"
#include "../Component.h"

/**
 *  Basic rectangle shape.
 *
 *  Derived from `ComponentBase`
 *
 */
namespace tge {
class Rectangle : public ComponentBase {
public:
    TGE_BASIC_CONSTRUCT(Rectangle);
    Rectangle(const Vector2i& size, const wchar_t fill = L'█') : ComponentBase(), fill(fill) { this->SetSize(size); }
    Rectangle(const wchar_t fill) : ComponentBase(), fill(fill) {}

    /**
     * Basic `tge::Rectangle` Render method.
     */
    void Render() override {
        for (int i = 0; i < this->GetSize().y; ++i) {
            for (int j = 0; j < this->GetSize().x; ++j) {
                render.DrawAtXY(this->GetPosition() + Vector2i{j, i},
                                {this->fill, this->GetForegroundColor(), this->GetBackgroundColor()});
            }
        }
    }

    /**
     * Set the fill char of the rectangle.
     *
     * @param fill the character to fill the rectangle with
     */
    void SetFill(const wchar_t fill) { this->fill = fill; }

private:
    wchar_t fill = L'█';
};
} // namespace tge
