#include "tge/input.h"
#include "tge/game.h"
#include "tge/graphics.h"

using namespace tge::async;

class Game : public tge::GameManager {
public:
    void Start() override {
        auto modal = Component<tge::Input>("modal")();
        modal->SetCenter(tge::Terminal::Size() / 2);
    }

    void Update() override {
        if (tge::Keyboard::GetKeyDown(tge::Key::Q)) Quit();

        if (tge::Keyboard::GetKeyDown(tge::Key::Enter)) {
            Get<tge::Input>("modal")->SetBorderForegroundColor(tge::Color::Red);
        }

        Get("modal")->Update();
    }

    void Render() override { Get("modal")->Render(); }
};

int main() { Game().Run(); }
