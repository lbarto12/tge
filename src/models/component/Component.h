#pragma once
#include <iostream>

#include "../../input/Keyboard.h"

#define TGE_BASIC_CONSTRUCT(cname)                                                                                     \
    cname() : tge::ComponentBase() {}

namespace tge {
class ComponentBase {
public:
    ComponentBase() {}
    virtual ~ComponentBase() = default;

    virtual void Init() {}
    virtual void Update() {}
    virtual void Render() {}

    void SetTickKeyEvent(gte::KeyEvent event) { this->keypress = event; }

protected:
    gte::KeyEvent keypress;

private:
};
} // namespace tge
