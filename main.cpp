#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using namespace tge::async;

class Game : public tge::GameManager {
public:
    Game() : tge::GameManager() {}

    void Update() override {
        if (Await(&quitKey)) Quit();
        if (Await(&incr)) {
            count += 1;
        }
    }

    void Render() override { render.DrawStringAtXY({0, 0}, std::to_wstring(count)); }

private:
    tge::KeyBuffer quitKey = tge::Key::Q;
    tge::KeyChord incr = {tge::Key::LeftCtrl, tge::Key::W};
    int count = 0;
};

int main() {
    auto game = Game();
    game.Run();
}
