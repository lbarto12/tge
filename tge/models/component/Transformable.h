#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include "../util/Bounds.h"
#include "../util/Vector2.h"

/**
 * Interface for defining transformable classes.
 *
 * Implements a variety of methods related to position and boundaries.
 */
namespace tge {
class Transformable {
public:
    /**
     * Get the component's size.
     *
     * @return size
     */
    Vector2i GetSize() const { return this->size; }

    /**
     * [Not Automatic]
     * Set the component's size.
     *
     * @param newSize the size to set
     * @return newSize
     */
    Vector2i SetSize(const Vector2i& newSize) {
        this->size.x = newSize.x;
        this->size.y = newSize.y;
        return this->size;
    }

    /**
     * Get the component's position.
     *
     * @return position
     */
    Vector2i GetPosition() const { return this->position; }

    /**
     * [Not Automatic]
     * Set the component's position.
     *
     * @param newPos the position to set
     * @return newPos
     */
    Vector2i SetPosition(const Vector2i& newPos) {
        this->position.x = newPos.x;
        this->position.y = newPos.y;
        return this->size;
    }

    /**
     * Get the bounds of the component.
     *
     * @return bounds
     */
    IntRect GetBounds() const {
        return IntRect{
            this->position.x,
            this->position.y,
            this->size.x,
            this->size.y,
        };
    }

    /**
     * [Not Automatic]
     * Set the bounds of the component.
     *
     * @param bounds the bounds to set
     * @return bounds
     */
    IntRect SetBounds(IntRect bounds) {
        this->position.x = bounds.x;
        this->position.y = bounds.y;
        this->size.x = bounds.width;
        this->size.y = bounds.height;

        return bounds;
    }

    /**
     * [Not Automatic]
     * Move the component by a delta.
     *
     * @param delta the delta to move
     * @return the new position
     */
    Vector2i Move(const Vector2i& delta) { return this->position = this->position + delta; }

protected:
    Vector2i size = {0, 0}, position = {0, 0};
};
} // namespace tge
