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
class ComponentManager;

class ComponentBase : public Transformable, public Drawable {
public:
    ComponentBase() : Transformable(), Drawable() {}
    virtual ~ComponentBase() = default;

    virtual void Init() {}
    virtual void Update() {}

    void __setComponentManager(ComponentManager* c) { this->components = c; }

protected:
    tge::EventManager& events = tge::EventManager::globalEventManager;

    template <typename EventType = Event> std::vector<EventType*> GetEvents() { return events.Get<EventType>(); }

    template <typename EventType = Event> void PushEvent(EventType event) { this->events.Push(std::move(event)); }

protected:
    /**
     * Create a component from a template-type and
     * Add component to component manager.
     *
     * @param id the ID for the created component
     * @return A function to construct the component
     */
    template <typename T> [[nodiscard]] auto Component(const std::string& id);

    /**
     * Get a component from the game's storage.
     *
     * @param id the ID of the component
     * @return the component.
     */
    template <typename T = class ComponentBase> T* Get(const std::string& id);
    /**
     * Get a shared_ptr to a component from the game's storage. Use this to
     * extend the component's lifetime past a future overwrite or Destroy().
     *
     * @param id the ID of the component
     * @return shared_ptr, or nullptr if not found
     */

    template <typename T = class ComponentBase> std::shared_ptr<T> GetShared(const std::string& id);

    /**
     * Remove a component from the game's storage.
     *
     * @param id the ID of the component
     */
    void Destroy(const std::string& id);

private:
    tge::ComponentManager* components = nullptr;
};

} // namespace tge
