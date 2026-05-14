#pragma once
// IWYU pragma: private, include <tge/graphics.h>
#include "component/Component.h"
#include <memory>
#include <unordered_map>
namespace tge {
/**
 * A class for managing components in the game.
 */
class ComponentManager {
public:
    ComponentManager() = default;
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;

    /**
     * Add a component to the component manager with no args.
     *
     * @param id the ID that should be used to reference the component
     * @return non-owning pointer to the created component
     */
    template <typename T = ComponentBase> T* addComponent(const std::string& id) {
        auto ptr = std::make_unique<T>();
        T* raw = ptr.get();
        this->components[id] = std::move(ptr);
        return raw;
    }

    /**
     * Add a component that has already been constructed.
     *
     * @param id the ID that should be used to reference the component
     * @param component the component to add (takes ownership)
     * @return non-owning pointer to the added component
     */
    template <typename T = ComponentBase> T* addConstructedComponent(const std::string& id, T component) {
        auto ptr = std::make_unique<T>(std::move(component));
        T* raw = ptr.get();
        this->components[id] = std::move(ptr);
        return raw;
    }

    template <typename T> T* addOwned(const std::string& id, std::unique_ptr<T> component) {
        T* raw = component.get();
        this->components[id] = std::move(component);
        return raw;
    }

    /**
     * Get a component by its ID.
     *
     * @param id the ID of the component
     * @return non-owning pointer, or nullptr if not found
     */
    template <typename T = ComponentBase> T* getComponent(const std::string& id) {
        auto it = this->components.find(id);
        if (it == this->components.end()) return nullptr;
        return static_cast<T*>(it->second.get());
    }

    /**
     * Remove a component by its ID.
     *
     * @param id the ID of the component
     */
    void removeComponent(const std::string& id) { this->components.erase(id); }

    ~ComponentManager() = default;

private:
    std::unordered_map<std::string, std::unique_ptr<ComponentBase>> components;
};
} // namespace tge
