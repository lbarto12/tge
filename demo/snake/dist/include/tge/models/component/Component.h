#pragma once
// IWYU pragma: private, include <tge/graphics.h>
#include <iostream>

#include "../../input/Keyboard.h"
#include "Drawable.h"
#include "Transformable.h"

#define TGE_BASIC_CONSTRUCT(cname)                                                                                     \
    cname() : tge::ComponentBase() {}

#define TGE_CUSTOM_CONSTRUCT(cname, pname)                                                                             \
    cname() : pname() {}

/**
 * The basis for general components.
 *
 * Derived from `tge::Transformable` and `tge::Drawable`
 */
namespace tge {
class ComponentBase : public Transformable, public Drawable {
public:
    ComponentBase() : Transformable(), Drawable() {}
    virtual ~ComponentBase() = default;

    virtual void Init() {}
    virtual void Update() {}
};
} // namespace tge
