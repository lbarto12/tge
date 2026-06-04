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

    void Start() override { t.Run("Hello world"); }

    void Update() override {
        if (tge::Keyboard::GetKeyDown(tge::Key::Q)) {
            Quit();
        }

        if (Await(&t)) {
            res = t.Result();
            t.Run("new");
            cycle--;
        }
        cycle += 1;
    }

    void Render() override {
        render.DrawStringAtXY({0, 0}, std::to_wstring(res));
        render.DrawStringAtXY({0, 1}, std::to_wstring(cycle));
    }

private:
    Thread<std::string, int> t = {strLen};
    int res = 0;
    int cycle = 0;

private:
    static int strLen(std::string s) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        return s.length();
    }
};

int main() {
    auto game = Game();
    game.Run();
}
