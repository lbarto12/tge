#pragma once
#include <iostream>

#include "../../input/Keyboard.h"
#include "ComponentRenderManager.h"
#include "Transformable.h"

#define TGE_BASIC_CONSTRUCT(cname)                                                                                     \
    cname() : tge::ComponentBase() {}

#define TGE_CUSTOM_CONSTRUCT(cname, pname)                                                                             \
    cname() : pname() {}

namespace tge {
class ComponentBase : public Transformable {
public:
    ComponentBase() : Transformable() {}
    virtual ~ComponentBase() = default;

    virtual void Init() {}
    virtual void Update() {}
    virtual void Render() {}

protected:
    internal::components::ComponentRenderManager render;
};
} // namespace tge
