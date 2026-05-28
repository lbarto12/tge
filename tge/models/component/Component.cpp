
#include "Component.h"
#include "../ComponentManager.h"

// Needed this to be able to include component manager functions in circular dep loop
namespace tge {
template <typename T> [[nodiscard]] auto ComponentBase::Component(const std::string& id) {
    return [this, id](auto&&... args) -> T* {
        auto ptr = std::make_unique<T>(std::forward<decltype(args)>(args)...);
        ptr->Init();
        ptr->__setComponentManager(this->components);
        T* raw = ptr.get();
        this->components->addOwned(id, std::move(ptr));
        return raw;
    };
}

template <typename T> T* ComponentBase::Get(const std::string& id) { return this->components->getComponent<T>(id); }

template <typename T> std::shared_ptr<T> ComponentBase::GetShared(const std::string& id) {
    return this->components->getComponentShared<T>(id);
}

void ComponentBase::Destroy(const std::string& id) { this->components->removeComponent(id); }

} // namespace tge
