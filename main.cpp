#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"

using namespace tge::async;

class Game : public tge::GameManager {
public:
    Game() : tge::GameManager() {}

    void Update() override {
        if (Await(&quit)) Quit();
    }

private:
    tge::KeyChord quit = {tge::Key::LeftCtrl, tge::Key::Q};
};

int main() {
    auto game = Game();
    game.Run();
}
