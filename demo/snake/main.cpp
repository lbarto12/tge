#include "tge/render/Color.h"
#include <chrono>
#include <cstdlib>
#include <string>
#include <tge/game.h>
#include <tge/graphics.h>
#include <tge/input.h>
#include <vector>

class Snake : public tge::ComponentBase {
public:
    void Init() override {
        this->moveDelay = tge::sync::Timer<std::chrono::milliseconds>(50);

        auto ts = tge::render::Terminal::Size();

        auto start = tge::Vector2i{ts.x / 4 * 2 + 1, ts.y / 4 * 2 + 1};
        segments.push_back(start);
        segments.push_back(start - tge::Vector2i{0, 1});
        segments.push_back(start - tge::Vector2i{0, 2});
        segments.push_back(start - tge::Vector2i{0, 3});

        vel = {0, -1};

        randomizeFruit();
    }

    void Update() override {
        if (tge::Keyboard::GetKeyDown(tge::Key::Up)) vel = {0, -1};
        if (tge::Keyboard::GetKeyDown(tge::Key::Right)) vel = {2, 0};
        if (tge::Keyboard::GetKeyDown(tge::Key::Left)) vel = {-2, 0};
        if (tge::Keyboard::GetKeyDown(tge::Key::Down)) vel = {0, 1};

        if (moveDelay.Await()) {
            auto newHead = segments[0] + vel;
            segments.insert(segments.begin(), newHead);
            segments.pop_back();

            if (newHead == fruit) {
                score += 1;
                randomizeFruit();
                segments.push_back(segments.back());
            }
        }
    }

    void Render() override {
        render.DrawStringAtXY(fruit, L"██", tge::Color::Red);
        for (const auto& segment : segments) {
            render.DrawAtXY(segment, tge::render::Pixel{L'█', tge::Color::Blue});
            render.DrawAtXY(tge::Vector2i{segment.x + 1, segment.y}, tge::render::Pixel{L'█', tge::Color::Blue});
        }

        render.DrawStringAtXY({0, 0}, L"length: " + std::to_wstring(segments.size()));
        render.DrawStringAtXY({0, 1}, L"score: " + std::to_wstring(score));
    }

private:
    tge::Vector2i vel, fruit;
    std::vector<tge::Vector2i> segments;
    int score = 0;
    tge::sync::Timer<std::chrono::milliseconds> moveDelay;

private:
    void randomizeFruit() {
        auto ts = tge::render::Terminal::Size();
        this->fruit = {(std::rand() % ts.x) / 2 * 2 + 1, (std::rand() % ts.y) / 2 * 2 + 1};
    }
};

class SnakeGame : public tge::GameManager {
public:
    SnakeGame() : tge::GameManager() {
        this->SetFPS(165);
        this->SetTicksPerSecond(60);
    }

    void Start() override { Component<Snake>("snake"); }

    void Update() override {
        if (tge::Keyboard::GetKeyDown('q')) {
            this->Quit();
        }

        Get("snake")->Update();
    }

    void Render() override { Get("snake")->Render(); }
};

int main() {
    auto game = SnakeGame();
    game.Run();
}
