#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <string>
#include <vector>

class Snake : public tge::ComponentBase {
public:
    void Init() override {
        auto start = tge::Terminal::Size() / 4 * 2 + 1;
        segments.push_back(start);
        segments.push_back(start - tge::Vector2i{0, 1});
        segments.push_back(start - tge::Vector2i{0, 2});
        segments.push_back(start - tge::Vector2i{0, 3});

        vel = {0, 1};

        randomizeFruit();
    }

    void Update() override {
        if (tge::Keyboard::GetKeyDown(tge::Key::Up) && vel != tge::Vector2i{0, 1}) vel = {0, -1};
        if (tge::Keyboard::GetKeyDown(tge::Key::Right) && vel != tge::Vector2i{-2, 0}) vel = {2, 0};
        if (tge::Keyboard::GetKeyDown(tge::Key::Left) && vel != tge::Vector2i{2, 0}) vel = {-2, 0};
        if (tge::Keyboard::GetKeyDown(tge::Key::Down) && vel != tge::Vector2i{0, -1}) vel = {0, 1};

        if (!moveDelay.Await()) return;

        auto newHead = tge::Math::Wrap({0, 0}, tge::Terminal::Size() / 2 * 2, segments[0] + vel);
        segments.insert(segments.begin(), newHead);
        segments.pop_back();

        if (newHead == fruit) {
            score += 1;
            randomizeFruit();
            segments.push_back(segments.back());
        }
    }

    void Render() override {
        render.DrawStringAtXY(fruit, L"██", tge::Color::Red);
        for (const auto& segment : segments) {
            render.DrawStringAtXY(segment, L"██", tge::Color::Blue);
        }

        render.DrawStringAtXY({0, 0}, L"length: " + std::to_wstring(segments.size()));
        render.DrawStringAtXY({0, 1}, L"score: " + std::to_wstring(score));
    }

private:
    tge::Vector2i vel, fruit;
    std::vector<tge::Vector2i> segments;
    int score = 0;
    tge::sync::Timer<std::chrono::milliseconds> moveDelay = 50;

private:
    void randomizeFruit() {
        auto ts = tge::Terminal::Size();
        this->fruit = tge::Vector2i{(std::rand() % ts.x - 1), (std::rand() % ts.y - 1)} / 2 * 2 + 1;
    }
};

class PauseMenu : public tge::BorderedRectangle {
public:
    PauseMenu(bool& paused, bool& kill) : tge::BorderedRectangle(), paused(paused), kill(kill) {}

    void Init() override {
        auto sz = tge::Terminal::Size() / 2;

        this->SetSize(sz);
        this->SetCenter(sz);
        this->SetBorderFromStyle(tge::BorderedRectangle::Style::RoundedLine);
    }

    void Render() override {
        tge::BorderedRectangle::Render();
        auto cntr = this->GetCenter();

        std::wstring menuTitle = L" PAUSED ";
        std::wstring resumeTxt = L" resume ";
        std::wstring quitTxt = L"  quit  ";

        render.DrawStringAtXY(cntr + tge::Vector2i{-int(menuTitle.length() / 2), -1}, menuTitle, tge::Color::White,
                              tge::Color::BrightBlack);
        render.DrawStringAtXY(cntr + tge::Vector2i{-int(resumeTxt.length() / 2), 1}, resumeTxt, tge::Color::White,
                              selected == 0 ? tge::Color::BrightBlack : tge::Color::None);
        render.DrawStringAtXY(cntr + tge::Vector2i{-int(quitTxt.length() / 2), 2}, quitTxt, tge::Color::White,
                              selected == 1 ? tge::Color::BrightBlack : tge::Color::None);
    }

    void Update() override {
        selected += downkey.SinglePress() - upkey.SinglePress();
        selected = tge::Math::Clamp(0, 1, selected);

        // enter keypress
        if (tge::Keyboard::GetKeyDown(tge::Key::Enter)) {
            switch (selected) {
            case 0:
                paused = false;
                break;
            case 1:
                kill = true;
                break;
            }
        }
    }

private:
    int selected = 0;
    bool &paused, &kill;

    tge::KeyBuffer upkey = tge::Key::Up;
    tge::KeyBuffer downkey = tge::Key::Down;
};

class SnakeGame : public tge::GameManager {
public:
    SnakeGame() : tge::GameManager() {
        this->SetFPS(165);
        this->SetTicksPerSecond(60);
    }

    void Start() override {
        Component<Snake>("snake");
        // TODO: this concept is better as a view model I think
        Construct("pause_menu")([this]() { return new PauseMenu(this->paused, this->kill); });
    }

    void Update() override {
        if (!tge::Terminal::IsFocused()) {
            return;
        }

        if (this->kill) {
            this->Quit();
        }

        // Buffered key press
        if (pausekey.SinglePress()) {
            paused = !paused;
        }

        if (!paused) {
            Get("snake")->Update();
        } else
            Get("pause_menu")->Update();
    }

    void Render() override {
        Get("snake")->Render();
        if (paused) {
            Get("pause_menu")->Render();
        }
    }

private:
    tge::KeyBuffer pausekey = tge::Key::Escape;
    bool paused = false, kill = false;
};

int main() {
    auto game = SnakeGame();
    game.Run();
}

// #include "tge/game.h"
// #include "tge/graphics.h"
// #include "tge/input.h"
// #include "tge/models/component/basic/Rectangle.h"
// #include "tge/render/Terminal.h"
//
// struct CustomComponent : public tge::Rectangle {
//     CustomComponent() : tge::Rectangle({15, 7}) {}
//
//     void Init() override {}
//     void Update() override {
//         if (tge::Keyboard::GetKeyDown(tge::Key::Left) || tge::Keyboard::GetKeyDown(tge::Key::A)) {
//             Move({-2, 0});
//         }
//         if (tge::Keyboard::GetKeyDown(tge::Key::Right) || tge::Keyboard::GetKeyDown(tge::Key::D)) {
//             Move({2, 0});
//         }
//         if (tge::Keyboard::GetKeyDown(tge::Key::Up) || tge::Keyboard::GetKeyDown(tge::Key::W)) {
//             Move({0, -1});
//         }
//         if (tge::Keyboard::GetKeyDown(tge::Key::Down) || tge::Keyboard::GetKeyDown(tge::Key::S)) {
//             Move({0, 1});
//         }
//     }
// };
//
// class Game : public tge::GameManager {
// public:
//     Game() : tge::GameManager() {}
//
//     void Start() override {
//         this->SetFPS(165);
//         this->SetTicksPerSecond(60);
//
//         Construct("bg")([]() {
//             auto c = new tge::Rectangle(tge::Terminal::Size());
//             c->SetForegroundColor(tge::Color::BrightBlack);
//             return c;
//         });
//
//         Construct("test")([]() {
//             auto c = new CustomComponent();
//             c->SetForegroundColor(tge::Color::Red);
//             return c;
//         });
//
//         Construct("wall")([]() {
//             auto c = new tge::Rectangle();
//             c->SetForegroundColor(tge::Color::Blue);
//             c->SetSize({20, 20});
//             return c;
//         });
//     }
//     void Update() override {
//         if (tge::Keyboard::GetKeyDown('q')) {
//             this->Quit();
//         }
//
//         Get("test")->Update();
//         Get("wall")->Update();
//     }
//     void Render() override {
//         Get("bg")->Render();
//         Get("wall")->Render();
//         Get("test")->Render();
//     }
//
// private:
// };
//
// int main() {
//     auto game = Game();
//     game.Run();
// }
