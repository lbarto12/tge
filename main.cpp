#include "src/input/Keyboard.h"
#include "src/input/Platform.h"
#include "src/models/GameManager.h"
#include "src/render/Terminal.h"

struct CustomComponent : public tge::ComponentBase {
    TGE_BASIC_CONSTRUCT(CustomComponent);

    void Init() override {}
    void Update() override {
        if (tge::Keyboard::GetKeyDown(tge::Key::Left) || tge::Keyboard::GetKeyDown(tge::Key::A)) {
            x -= 1;
        }
        if (tge::Keyboard::GetKeyDown(tge::Key::Right) || tge::Keyboard::GetKeyDown(tge::Key::D)) {
            x += 1;
        }
        if (tge::Keyboard::GetKeyDown(tge::Key::Up) || tge::Keyboard::GetKeyDown(tge::Key::W)) {
            y -= 1;
        }
        if (tge::Keyboard::GetKeyDown(tge::Key::Down) || tge::Keyboard::GetKeyDown(tge::Key::S)) {
            y += 1;
        }
    }

    void Render() override {
        tge::render::Terminal::MoveTo(x, y);
        std::cout << "X" << std::flush;
    }

    int x = 5, y = 5;
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
