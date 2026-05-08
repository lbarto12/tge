#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include <unordered_map>

#include "component/Component.h"

namespace tge {
/**
 * A class for managing components in the game.
 */
class ComponentManager {
public:
    /**
     * Add a component to the component manager with no args.
     *
     * Component is created as a basic element from the given template.
     *
     * @param id the ID that should be used to reference the component
     * @return the created component
     */
    template <typename T = ComponentBase> T* addComponent(const std::string& id) {
        this->components[id] = new T();
        return static_cast<T*>(this->components[id]);
    }

    /**
     * Add a component that has already been constructed.
     *
     * @param id the ID that should be used to reference the component
     * @param component the component to add
     * @return the added component
     */
    template <typename T = ComponentBase> T* addConstructedComponent(const std::string& id, T* component) {
        this->components[id] = component;
        return static_cast<T*>(this->components[id]);
    }

    /**
     * Get a component by its ID.
     *
     * Non-templated calls to this function will return components as a `ComponentBase` object
     *
     * @param id the ID of the component
     */
    template <typename T = ComponentBase> T* getComponent(const std::string id) {
        return static_cast<T*>(this->components[id]);
    }

    /**
     * Destructor
     */
    ~ComponentManager() {
        for (auto& [id, obj] : this->components) {
            delete obj;
        }
        this->components.clear();
    }

private:
    std::unordered_map<std::string, ComponentBase*> components;
};
} // namespace tge
