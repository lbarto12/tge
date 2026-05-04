#include "tge/input/Keyboard.h"
#include "tge/input/Platform.h"
#include "tge/models/GameManager.h"
#include "tge/models/component/basic/Rectangle.h"

struct CustomComponent : public tge::Rectangle {
    CustomComponent() : tge::Rectangle({20, 10}) {}

    void Init() override {}
    void Update() override {
        if (tge::Keyboard::GetKeyDown(tge::Key::Left) || tge::Keyboard::GetKeyDown(tge::Key::A)) {
            Move({-2, 0});
        }
        if (tge::Keyboard::GetKeyDown(tge::Key::Right) || tge::Keyboard::GetKeyDown(tge::Key::D)) {
            Move({2, 0});
        }
        if (tge::Keyboard::GetKeyDown(tge::Key::Up) || tge::Keyboard::GetKeyDown(tge::Key::W)) {
            Move({0, -1});
        }
        if (tge::Keyboard::GetKeyDown(tge::Key::Down) || tge::Keyboard::GetKeyDown(tge::Key::S)) {
            Move({0, 1});
        }
    }
};

class Game : public tge::GameManager {
public:
    Game() : tge::GameManager() {}

    void Start() override {
        this->SetFPS(165);
        this->SetTicksPerSecond(60);

        Construct("test")([]() { return new CustomComponent(); });
    }
    void Update() override {
        if (tge::Keyboard::GetKeyDown('q')) {
            this->Quit();
        }

        Get<CustomComponent>("test")->Update();
    }
    void Render() override { Get<CustomComponent>("test")->Render(); }

private:
};

int main() {
    auto game = Game();
    game.Run();
}
