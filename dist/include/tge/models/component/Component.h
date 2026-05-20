#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include "../../event/Event.h"
#include "../../input/Keyboard.h"
#include "Drawable.h"
#include "Transformable.h"
#include <vector>

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

protected:
    tge::EventManager& events = tge::EventManager::globalEventManager;

    template <typename EventType = Event> std::vector<EventType*> GetEvents() { return events.Get<EventType>(); }

    template <typename EventType = Event> void PushEvent(EventType event) { this->events.Push(std::move(event)); }
};
} // namespace tge
