#include "../util/Bounds.h"
#include "../util/Vector2.h"

namespace tge {
class Transformable {
public:
    Vector2i GetSize() const { return this->size; }

    Vector2i SetSize(const Vector2i& newSize) {
        this->size.x = newSize.x;
        this->size.y = newSize.y;
        return this->size;
    }

    Vector2i GetPosition() const { return this->position; }

    Vector2i SetPosition(const Vector2i& newPos) {
        this->position.x = newPos.x;
        this->position.y = newPos.y;
        return this->size;
    }

    IntRect GetBounds() const {
        return IntRect{
            this->position.x,
            this->position.y,
            this->size.x,
            this->size.y,
        };
    }

    IntRect SetBounds(IntRect bounds) {
        this->position.x = bounds.x;
        this->position.y = bounds.y;
        this->size.x = bounds.width;
        this->size.y = bounds.height;

        return bounds;
    }

    Vector2i Move(const Vector2i& delta) {
        this->position.x += delta.x;
        this->position.y += delta.y;
        return this->position;
    }

protected:
    Vector2i size = {0, 0}, position = {0, 0};
};
} // namespace tge
