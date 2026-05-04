#pragma once
#include "../models/ComponentManager.h"
#include "../render/Terminal.h"
#include "../sync/Timer.h"

#include <chrono>
#include <cstdlib>
#include <functional>
#include <thread>

const long long DEFAULT_RENDER_FPS = 60; // fps
const long long DEFAULT_TICK_SPEED = 60; // tps

namespace tge {
class GameManager {
public:
    GameManager() {
        this->SetFPS(DEFAULT_RENDER_FPS);
        this->SetTicksPerSecond(DEFAULT_TICK_SPEED);
    }

    virtual void Start() {}
    virtual void Render() {}
    virtual void Update() {}

    bool Run() {
        render::Terminal::Init();

        // Safety net for abnormal exits - raw mode would otherwise leak into the user's shell.
        std::atexit([] {
            render::Terminal::DisableRawMode();
            tge::Keyboard::Shutdown();
        });

        if (!tge::Keyboard::Init()) {
            render::Terminal::UnMount();
            return false;
        }

        this->Start();
        while (this->running) {
            if (this->tickSpeed.Await()) {
                this->Update();
            }
            if (this->renderFps.Await()) {
                this->Render();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // no runaway cpu shit
        }

        tge::Keyboard::Shutdown();
        render::Terminal::UnMount();
        return true;
    }

protected:
    void Quit() { this->running = false; }

    void SetFPS(long long fps) {
        this->renderFps = sync::Timer<std::chrono::milliseconds>{calcTimerDurationFromFPS(fps)};
    }

    void SetTicksPerSecond(long long tps) {
        this->tickSpeed = sync::Timer<std::chrono::milliseconds>{calcTimerDurationFromFPS(tps)};
    }

private:
    tge::ComponentManager components;

protected:
    template <typename T = class ComponentBase> T* Component(const std::string& id) {
        T* component = this->components.addComponent<T>(id);
        component->Init();
        return component;
    }

    template <typename T = class ComponentBase>
    std::function<T*(std::function<T*()>)> Construct(const std::string& id) {
        return [this, id](std::function<T*()> creator) -> T* {
            T* component = creator();
            component->Init();
            this->components.addConstructedComponent(id, component);
            return components.getComponent<T>(id);
        };
    }

    template <typename T = class ComponentBase> T* Get(const std::string& id) {
        return this->components.getComponent<T>(id);
    }

private:
    bool running = true;
    sync::Timer<std::chrono::milliseconds> renderFps;
    sync::Timer<std::chrono::milliseconds> tickSpeed;

private:
    long long calcTimerDurationFromFPS(long long perSecond) { return 1000 / perSecond; }
};
} // namespace tge
