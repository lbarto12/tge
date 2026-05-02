#include <iostream>
#include <unordered_map>

#include "component/Component.h"

namespace tge {
class ComponentManager {
public:
    template <typename T = ComponentBase> T* addComponent(const std::string& id) {
        this->components[id] = new T();
        return static_cast<T*>(this->components[id]);
    }

    template <typename T = ComponentBase> T* addConstructedComponent(const std::string& id, T* component) {
        this->components[id] = component;
        return static_cast<T*>(this->components[id]);
    }

    template <typename T = ComponentBase> T* getComponent(const std::string id) {
        return static_cast<T*>(this->components[id]);
    }

private:
    std::unordered_map<std::string, ComponentBase*> components;
};
} // namespace tge
