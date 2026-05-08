#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include "../../render/ScreenBuffer.h"

/**
 * Interface for defining renderable classes.
 *
 * Implementations of this class must override `void Render()`.
 *
 * Inheritence exposes a protected `tge::render::ScreenBuffer render` that must
 * be used for accurate rendering.
 */
namespace tge {
class Drawable {
public:
    /**
     * [Virtual] - [No Default]
     * Base render method.
     */
    virtual void Render() = 0;

    /**
     * [Not Automatic]
     * Set drawable background color.
     *
     * @param c the color to be set
     * @return c
     */
    Color SetBackgroundColor(Color c) { return this->background = c; }

    /**
     * Get the background color
     *
     * @return the object's background color
     */
    Color GetBackgroundColor() const { return this->background; }

    /**
     * [Not Automatic]
     * Set drawable foreground color.
     *
     * @param c the color to be set
     * @return c
     */
    Color SetForegroundColor(Color c) { return this->foreground = c; }

    /**
     * Get the foreground color.
     *
     * @return the object's background color
     */
    Color GetForegroundColor() const { return this->foreground; }

protected:
    render::ScreenBuffer& render = render::ScreenBuffer::globalScreenBuffer;

private:
    Color foreground = Color::White, background = Color::None;
};
} // namespace tge
