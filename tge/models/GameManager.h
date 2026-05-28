#pragma once
// IWYU pragma: private, include <tge/game.h>

#include "../event/Event.h"
#include "../input/Keyboard.h"
#include "../render/ScreenBuffer.h"
#include "../render/Terminal.h"
#include "../sync/Timer.h"
#include "ComponentManager.h"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <thread>

const long long DEFAULT_RENDER_FPS = 60; // fps
const long long DEFAULT_TICK_SPEED = 60; // tps

using namespace tge::async;

namespace tge {
class GameManager {
public:
    GameManager() {
        this->SetFPS(DEFAULT_RENDER_FPS);
        this->SetTicksPerSecond(DEFAULT_TICK_SPEED);
        this->components = &ComponentManager::globalComponentManager;
        std::setlocale(LC_ALL, "");
        std::srand(time(NULL));
    }

    virtual void Start() {}
    virtual void Render() {}
    virtual void Update() {}

private: // Internal loops and inits
    void internal_update() {
        this->Update();
        this->events.StageNextTickEvents();
    }

    void internal_render() {
        this->Render();
        this->render.SwapBuffer();
    }

public:
    /**
     * Run the game :)
     *
     * @return graceful exit
     */
    bool Run() {
        Terminal::Init();

        // Safety net for abnormal exits - raw mode would otherwise leak into the user's shell.
        std::atexit([] {
            Terminal::DisableRawMode();
            tge::Keyboard::Shutdown();
        });

        if (!tge::Keyboard::Init()) {
            Terminal::UnMount();
            return false;
        }

        this->Start();
        while (this->running) {
            if (Await(&tickSpeed)) {
                this->internal_update();
            }
            if (Await(&renderFps)) {
                this->internal_render();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // no runaway cpu shit
        }

        tge::Keyboard::Shutdown();
        Terminal::UnMount();
        return true;
    }

protected:
    /**
     * Stop the application
     */
    void Quit() { this->running = false; }

    /**
     * Set the game's FPS. FPS is used only for rendering.
     *
     * @param fps frames to render per second
     */
    void SetFPS(long long fps) { this->renderFps = Timer<std::chrono::milliseconds>{calcTimerDurationFromFPS(fps)}; }

    /**
     * Set the game's TPS. TPS is used only for logic updates.
     *
     * @param tps game logic ticks per second
     */
    void SetTicksPerSecond(long long tps) {
        this->tickSpeed = Timer<std::chrono::milliseconds>{calcTimerDurationFromFPS(tps)};
    }

private:
    tge::ComponentManager* components;

protected:
    tge::render::ScreenBuffer& render = tge::render::ScreenBuffer::globalScreenBuffer;
    tge::EventManager& events = tge::EventManager::globalEventManager;

protected:
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
    template <typename T> [[nodiscard]] auto Component(const std::string& id) {
        return [this, id](auto&&... args) -> T* {
            auto ptr = std::make_unique<T>(std::forward<decltype(args)>(args)...);
            ptr->Init();
            ptr->__setComponentManager(this->components);
            T* raw = ptr.get();
            this->components->addOwned(id, std::move(ptr));
            return raw;
        };
    }

    /**
     * Get a component from the game's storage.
     *
     * @param id the ID of the component
     * @return the component.
     */
    template <typename T = class ComponentBase> T* Get(const std::string& id) {
        return this->components->getComponent<T>(id);
    }

    /**
     * Get a shared_ptr to a component from the game's storage. Use this to
     * extend the component's lifetime past a future overwrite or Destroy().
     *
     * @param id the ID of the component
     * @return shared_ptr, or nullptr if not found
     */
    template <typename T = class ComponentBase> std::shared_ptr<T> GetShared(const std::string& id) {
        return this->components->getComponentShared<T>(id);
    }

    /**
     * Remove a component from the game's storage.
     *
     * @param id the ID of the component
     */
    void Destroy(const std::string& id) { this->components->removeComponent(id); }

private:
    bool running = true;
    Timer<std::chrono::milliseconds> renderFps;
    Timer<std::chrono::milliseconds> tickSpeed;

private:
    long long calcTimerDurationFromFPS(long long perSecond) { return 1000 / perSecond; }
};
} // namespace tge
