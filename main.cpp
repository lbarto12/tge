#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"
#include "tge/models/component/basic/Rectangle.h"

struct CustomComponent : public tge::Rectangle {
    CustomComponent() : tge::Rectangle({15, 7}) {}

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

        Construct("bg")([]() {
            auto ts = tge::render::Terminal::Size();
            auto c = new tge::Rectangle({ts.cols, ts.rows});
            c->SetBackgroundColor(tge::Color::White);
            return c;
        });

        Construct("test")([]() {
            auto c = new CustomComponent();
            c->SetForegroundColor(tge::Color::Red);
            return c;
        });

        Construct("wall")([]() {
            auto c = new tge::Rectangle();
            c->SetForegroundColor(tge::Color::Blue);
            c->SetSize({20, 20});
            return c;
        });
    }
    void Update() override {
        if (tge::Keyboard::GetKeyDown('q')) {
            this->Quit();
        }

        Get("test")->Update();
        Get("wall")->Update();

        // std::cout << Get("wall")->GetSize().x << " " << Get("wall")->GetSize().y << "";
    }
    void Render() override {
        Get("bg")->Render();
        Get("wall")->Render();
        Get("test")->Render();

        render.SwapBuffer();
    }

private:
};

int main() {
    auto game = Game();
    game.Run();
}
