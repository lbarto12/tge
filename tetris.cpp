#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"
#include <algorithm>
#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace tge::async;

// State
struct GameState {};

// Events
struct GameQuitEvent : public tge::Event {};

typedef std::vector<tge::Vector2i> Piece; // array of 4 cells

std::pair<Piece, tge::Color> GetRandomPiece() {
    std::vector<std::pair<Piece, tge::Color>> opts = {
        {{{-2, 0}, {-1, 0}, {0, 0}, {1, 0}}, tge::Color::BrightBlue}, // I
        {{{-1, 0}, {-1, 1}, {0, 0}, {1, 0}}, tge::Color::BrightRed},  // L
        {{{-1, 0}, {0, 0}, {1, 0}, {1, 1}}, tge::Color::Blue},        // J
        {{{-1, 1}, {0, 1}, {0, 0}, {1, 0}}, tge::Color::Green},       // S
        {{{-1, 0}, {0, 0}, {0, 1}, {1, 1}}, tge::Color::Red},         // Z
        {{{-1, 0}, {0, 0}, {0, 1}, {1, 0}}, tge::Color::Magenta},     // T
        {{{-1, 0}, {-1, 1}, {0, 0}, {0, 1}}, tge::Color::Yellow},     // O
    };
    return tge::Random::Choice(opts);
}

void RotateRight(Piece& p) {
    for (auto& i : p) i = tge::Vector2i{-i.y, i.x};
}

void RotateLeft(Piece& p) {
    for (auto& i : p) i = tge::Vector2i{i.y, -i.x};
}

int pminx(const tge::Vector2i& offset, const Piece& p) {
    int res = (offset + p[0]).x;
    for (const auto& i : p) res = std::min(res, (offset + i).x);
    return res;
}

int pmaxx(const tge::Vector2i& offset, const Piece& p) {
    int res = (offset + p[0]).x;
    for (const auto& i : p) res = std::max(res, (offset + i).x);
    return res;
}

int pmaxy(const tge::Vector2i& offset, const Piece& p) {
    int res = (offset + p[0]).y;
    for (const auto& i : p) res = std::max(res, (offset + i).y);
    return res;
}

bool CannotMoveLeft(const tge::Vector2i& offset, const Piece& p, const tge::IntRect& board) {
    return pminx(offset, p) <= board.x + 1;
};

bool CannotMoveRight(const tge::Vector2i& offset, const Piece& p, const tge::IntRect& board) {
    return pmaxx(offset, p) >= board.x + board.width - 3;
}

bool CannotMoveDown(const tge::Vector2i& offset, const Piece& p, const tge::IntRect& board) {
    return pmaxy(offset, p) >= board.y + board.height - 1;
}

// Board Component
class Board : public tge::Rectangle {
public:
    Board() : tge::Rectangle() {}

    void Init() override { this->SetSize({20, 20}); }

    void Update() override {}

    void Render() override { tge::Rectangle::Render(); }
};

// Tetromino Component
class Tetromino : public tge::ComponentBase {
public:
    Tetromino(Piece p, tge::Color c, tge::IntRect boardBounds) : tge::ComponentBase(), p(p), boardBounds(boardBounds) {
        this->SetBackgroundColor(c);
    }

    void Init() override {
        boardPos = (tge::Terminal::Size() / 2 - tge::Vector2i{20, 20} / 2) + tge::Vector2i{10, 0};
        this->SetPosition(boardPos);
        vel = {0, 1};
        manual = {0, 0};
    }

    void Update() override {
        if (set || CannotMoveDown(this->GetPosition(), p, boardBounds)) {
            this->set = true;
            return;
        }

        // handle LR
        if (tge::Keyboard::GetKeyDown(tge::Key::Left))
            manual.x = -2;
        else if (tge::Keyboard::GetKeyDown(tge::Key::Right))
            manual.x = 2;
        else
            manual.x = 0;

        // Handle rot
        if (Await(&rotateLeftKey))
            RotateLeft(p);
        else if (Await(&rotateRightKey))
            RotateRight(p);

        // Handle downwards motion
        if (Await(&moveDelay)) {
            this->Move(vel);
        }

        // Handler manual input movement
        if (manual == tge::Vector2i{0, 0}) {
            this->arrowDelay.SetReadyNow();
            this->arrowDelay.SetInterval(0); // reset momentum
        } else if (Await(&arrowDelay)) {
            if ((manual == tge::Vector2i{-2, 0} && !CannotMoveLeft(this->GetPosition(), p, boardBounds)) ||
                (manual == tge::Vector2i{2, 0} && !CannotMoveRight(this->GetPosition(), p, boardBounds))) {
                this->Move(manual);
                if (this->arrowDelay.GetInterval() == 0) {
                    this->arrowDelay.SetInterval(100);
                } else {
                    this->arrowDelay.SetInterval(std::max(this->arrowDelay.GetInterval() - 15, 20LL));
                }
            }
        }
    }

    void Render() override {
        auto pos = std::to_wstring(this->GetPosition().x) + L" " + std::to_wstring(this->GetPosition().y);
        render.DrawStringAtXY({0, 0}, pos, tge::Color::White, tge::Color::Black);

        auto bounds = std::to_wstring(boardBounds.x) + L" " + std::to_wstring(boardBounds.y) + L" " +
                      std::to_wstring(boardBounds.width) + L" " + std::to_wstring(boardBounds.height);
        render.DrawStringAtXY({0, 1}, bounds);

        for (const auto& i : p) {
            render.DrawStringAtXY(this->GetPosition() + i * tge::Vector2i{2, 1}, L"  ", tge::Color::None,
                                  this->GetBackgroundColor());
        }
    }

    bool IsSet() { return set; }

private:
    bool set = false;

    Piece p;
    tge::Vector2i boardPos, vel, manual;

    tge::IntRect boardBounds;

    // Movement
    tge::Timer<std::chrono::milliseconds> moveDelay = 50;
    tge::Timer<std::chrono::milliseconds> arrowDelay = 0;

    // Rotations
    tge::KeyBuffer rotateLeftKey = tge::Key::Z;
    tge::KeyBuffer rotateRightKey = tge::Key::X;
};

// Game Manager
class Tetris : public tge::GameManager {
public:
    Tetris() : tge::GameManager() {
        this->SetFPS(165);
        this->SetTicksPerSecond(60);

        auto bg = Component<tge::Rectangle>("bg")(tge::Terminal::Size());
        bg->SetBackgroundColor(tge::Color::Black);

        auto board = Component<Board>("board")();
        board->SetCenter(bg->GetCenter());

        Next();
    }

    void Update() override {
        if (Await(&quitkey)) events.Push(GameQuitEvent{});
        if (!events.Get<GameQuitEvent>().empty()) Quit();

        auto current = Get<Tetromino>("current");
        current->Update();
        if (current->IsSet()) {
            Next();
        }
    }

    void Render() override {
        Get("bg")->Render();
        Get("board")->Render();

        for (auto& i : pieces) {
            i->Render();
        }
        Get<Tetromino>("current")->Render();
    }

private:
    GameState state;

    std::vector<std::shared_ptr<Tetromino>> pieces;

    tge::KeyBuffer quitkey = tge::Key::Q;

private:
    Tetromino* Next() {
        if (auto old = GetShared<Tetromino>("current")) {
            pieces.push_back(std::move(old));
        }
        auto [p, c] = GetRandomPiece();
        return Component<Tetromino>("current")(p, c, Get("board")->GetBounds());
    }
};

int main() {
    auto tetris = Tetris();
    tetris.Run();
}
