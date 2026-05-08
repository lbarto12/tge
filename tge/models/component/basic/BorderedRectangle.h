#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include "../../util/Vector2.h"
#include "Rectangle.h"

namespace tge {
struct BorderSet {
    wchar_t tl, tr, br, bl;
    wchar_t l, t, r, b;
};

class BorderedRectangle : public Rectangle {
public:
    using Rectangle::Rectangle; // Inherit constructors

    enum class Style {
        Line = 0,
        LineThick = 1,
        RoundedLine = 2,
        Solid = 3,
    };

    void Render() override {
        Rectangle::Render(); // Draw basic, then render border on top.

        auto bounds = this->GetBounds();
        auto pos = this->GetPosition();

        // Top left
        render.DrawAtXY(pos, {bs.tl, borderFgColor, borderBgColor});
        // Top right
        render.DrawAtXY(pos + Vector2i{bounds.width - 1, 0}, {bs.tr, borderFgColor, borderBgColor});
        // Bottom right
        render.DrawAtXY(pos + Vector2i{bounds.width - 1, bounds.height - 1}, {bs.br, borderFgColor, borderBgColor});
        // Bottom left
        render.DrawAtXY(pos + Vector2i{0, bounds.height - 1}, {bs.bl, borderFgColor, borderBgColor});

        // Top
        for (int i = 1; i < bounds.width - 1; ++i)
            render.DrawAtXY(pos + Vector2i{i, 0}, {bs.t, borderFgColor, borderBgColor});

        // Right
        for (int i = 1; i < bounds.height - 1; ++i)
            render.DrawAtXY(pos + Vector2i{bounds.width - 1, i}, {bs.r, borderFgColor, borderBgColor});

        // Bottom
        for (int i = 1; i < bounds.width - 1; ++i)
            render.DrawAtXY(pos + Vector2i{i, bounds.height - 1}, {bs.b, borderFgColor, borderBgColor});

        // Left
        for (int i = 1; i < bounds.height - 1; ++i)
            render.DrawAtXY(pos + Vector2i{0, i}, {bs.l, borderFgColor, borderBgColor});
    }

    void SetBorderFromStyle(Style style) { this->bs = getBorderSetFromStyle(style); }

    void SetBorder(BorderSet borderSet) { this->bs = borderSet; }

    void SetBorderForegroundColor(Color color) { this->borderFgColor = color; }

    void SetBorderBackgroundColor(Color color) { this->borderBgColor = color; }

private:
    BorderSet bs = getBorderSetFromStyle(Style::Line); // Default to solid, squared line
    Color borderBgColor = Color::None, borderFgColor = Color::White;

    BorderSet getBorderSetFromStyle(Style style) {
        switch (style) {
        case Style::Line:
            return BorderSet{L'┌', L'┐', L'┘', L'└', L'│', L'─', L'│', L'─'};
        case Style::LineThick:
            return BorderSet{L'┏', L'┓', L'┛', L'┗', L'┃', L'━', L'┃', L'━'};
        case Style::RoundedLine:
            return BorderSet{L'╭', L'╮', L'╯', L'╰', L'│', L'─', L'│', L'─'};
        case Style::Solid:
            return BorderSet{L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█'};
        default:
            return BorderSet{};
        }
    }
};
} // namespace tge
