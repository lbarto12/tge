

#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"

using namespace tge::async;

class AnimatedModal : public tge::Input {
public:
    void Init() override {
        tge::Input::SetValue(L"Animated Modal!");
        this->SetSize({0, 3});
    }

    void Update() override {
        if (Await(&openkey)) {
            shouldOpen = true;
        }
        if (Await(&closekey)) {
            shouldClose = true;
        }

        if (shouldOpen && this->GetSize().x < targetwidth) {
            this->SetSize(this->GetSize() + tge::Vector2i{1, 0});
            return;
        } else {
            this->shouldOpen = false;
        }
        if (shouldClose && this->GetSize().x > 1) {
            this->SetSize(this->GetSize() - tge::Vector2i{1, 0});
            return;
        } else {
            this->shouldClose = false;
        }
    }

    void Render() override {
        if (this->GetSize().x > 1) {
            tge::Input::Render();
        }
    }

private:
    int targetwidth = 40;
    bool shouldOpen = false;
    bool shouldClose = false;
    tge::KeyBuffer openkey = tge::Key::O;
    tge::KeyBuffer closekey = tge::Key::C;
};

class Game : public tge::GameManager {
public:
    void Start() override {
        SetTicksPerSecond(90);
        SetFPS(90);
        Component<AnimatedModal>("modal")();
    }

    void Update() override {
        if (tge::Keyboard::GetKeyDown(tge::Key::Q)) Quit();

        Get("modal")->Update();
    }

    void Render() override { Get("modal")->Render(); }
};

int main() {
    auto game = Game();
    game.Run();
}
