#pragma once
#include <iostream>

#include "../../input/Keyboard.h"
#include "ComponentRenderManager.h"

#define TGE_BASIC_CONSTRUCT(cname)                                                                                     \
    cname() : tge::ComponentBase() {}

#define TGE_CUSTOM_CONSTRUCT(cname, pname)                                                                             \
    cname() : pname() {}

namespace tge {
class ComponentBase {
public:
    ComponentBase() {}
    virtual ~ComponentBase() = default;

    virtual void Init() {}
    virtual void Update() {}
    virtual void Render() {}

protected:
    internal::components::ComponentRenderManager render;

protected:
    Vector2i size = {0, 0}, position = {0, 0};

    Vector2i GetSize() const { return size; }

    Vector2i SetSize(const Vector2i& newSize) {
        size.x = newSize.x;
        size.y = newSize.y;
        return size;
    }

    Vector2i GetPosition() const { return position; }

    Vector2i SetPosition(const Vector2i& newSize) {
        position.x = newSize.x;
        position.y = newSize.y;
        return position;
    }

    Vector2i Move(const Vector2i& delta) {
        position.x += delta.x;
        position.y += delta.y;
        return position;
    }
};
} // namespace tge
