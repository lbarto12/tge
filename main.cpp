#include "src/input/Keyboard.h"
#include "src/models/GameManager.h"
#include "src/render/Terminal.h"

struct CustomComponent : public tge::ComponentBase {
    TGE_BASIC_CONSTRUCT(CustomComponent);

    void Init() override {}
    void Update() override {
        if (keypress.Is(gte::Key::Left)) {
            x -= 1;
        }
        if (keypress.Is(gte::Key::Right)) {
            x += 1;
        }
        if (keypress.Is(gte::Key::Up)) {
            y -= 1;
        }
        if (keypress.Is(gte::Key::Down)) {
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
        if (keypress.Is('q')) {
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
