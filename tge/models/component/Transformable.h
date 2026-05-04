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

    Vector2i Move(const Vector2i& delta) {
        this->position.x += delta.x;
        this->position.y += delta.y;
        return this->position;
    }

protected:
    Vector2i size = {0, 0}, position = {0, 0};
};
} // namespace tge
