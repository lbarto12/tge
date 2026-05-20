#pragma once
// IWYU pragma: private, include <tge/game.h>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
namespace tge {
struct Event {
    virtual ~Event() = default;
};
class EventManager {
public:
    EventManager() {}
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    template <typename EventType = Event> void Push(EventType event) {
        auto key = std::type_index(typeid(EventType));
        this->events[key].push_back(std::make_unique<EventType>(std::move(event)));
    }

    template <typename EventType> std::vector<EventType*> Get() {
        auto key = std::type_index(typeid(EventType));
        auto it = this->nextTickEvents.find(key);
        if (it == this->nextTickEvents.end()) {
            return {};
        }
        std::vector<EventType*> result;
        result.reserve(it->second.size());
        for (auto& e : it->second) {
            result.push_back(static_cast<EventType*>(e.get()));
        }
        return result;
    }

    void StageNextTickEvents() {
        this->nextTickEvents.clear();
        this->nextTickEvents = std::move(this->events);
    }

    void DestroyAll() {
        this->events.clear();
        this->nextTickEvents.clear();
    }

    static EventManager globalEventManager;

    ~EventManager() { this->DestroyAll(); }

private:
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<Event>>> events, nextTickEvents;
};
inline EventManager EventManager::globalEventManager;
} // namespace tge
