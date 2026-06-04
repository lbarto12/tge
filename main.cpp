

#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"

class Game : public tge::GameManager {
public:
    void Start() override { Component<Input>("modal")(); }

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
