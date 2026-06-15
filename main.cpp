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

    void Update() {
        if (Await(&quitKey)) Quit();
    }

private:
    tge::KeyBuffer quitKey = tge::Key::Q;
};

int main() {
    auto game = Game();
    game.Run();
}
