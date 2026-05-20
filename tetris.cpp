#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"
#include <algorithm>
#include <chrono>
#include <map>
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
    int res = offset.x + p[0].x * 2;
    for (const auto& i : p) res = std::min(res, offset.x + i.x * 2);
    return res;
}

int pmaxx(const tge::Vector2i& offset, const Piece& p) {
    int res = offset.x + p[0].x * 2;
    for (const auto& i : p) res = std::max(res, offset.x + i.x * 2);
    return res;
}

int pmaxy(const tge::Vector2i& offset, const Piece& p) {
    int res = (offset + p[0]).y;
    for (const auto& i : p) res = std::max(res, (offset + i).y);
    return res;
}

// Board Component
class Board : public tge::Rectangle {
public:
    Board() : tge::Rectangle() {}

    void Init() override { this->SetSize({20, 20}); }

    void Update() override {}

    void Render() override { tge::Rectangle::Render(); }
};

class Tetromino;
bool isBoardInvalid(const tge::Vector2i& offset, const Piece& p, const tge::IntRect& board,
                    const std::vector<std::shared_ptr<Tetromino>>& others);

// Tetromino Component
class Tetromino : public tge::ComponentBase {
public:
    Tetromino(Piece p, tge::Color c, tge::IntRect boardBounds,
              const std::vector<std::shared_ptr<Tetromino>>& others,
              const std::vector<int>& flashRows, const bool& flashHide)
        : tge::ComponentBase(), p(p), boardBounds(boardBounds), others(others),
          flashRows(flashRows), flashHide(flashHide) {
        this->SetBackgroundColor(c);
    }

    void Init() override {
        boardPos = (tge::Terminal::Size() / 2 - tge::Vector2i{20, 20} / 2) + tge::Vector2i{10, 0};
        this->SetPosition(boardPos);
        vel = {0, 1};
        manual = {0, 0};
    }

    void Update() override {
        if (set || isBoardInvalid(this->GetPosition() + tge::Vector2i{0, 1}, p, boardBounds, others)) {
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
        if (Await(&rotateLeftKey)) {
            Piece rotated = p;
            RotateLeft(rotated);
            if (!isBoardInvalid(this->GetPosition(), rotated, boardBounds, others)) p = std::move(rotated);
        } else if (Await(&rotateRightKey)) {
            Piece rotated = p;
            RotateRight(rotated);
            if (!isBoardInvalid(this->GetPosition(), rotated, boardBounds, others)) p = std::move(rotated);
        }

        // Handle downwards motion
        if (Await(&moveDelay)) {
            this->Move(vel);
        }

        // Handler manual input movement
        if (manual == tge::Vector2i{0, 0}) {
            this->arrowDelay.SetReadyNow();
            this->arrowDelay.SetInterval(0); // reset momentum
        } else if (Await(&arrowDelay)) {
            if (!isBoardInvalid(this->GetPosition() + manual, p, boardBounds, others)) {
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

        auto pos2 = this->GetPosition();
        for (const auto& i : p) {
            int absY = pos2.y + i.y;
            if (flashHide && std::find(flashRows.begin(), flashRows.end(), absY) != flashRows.end()) continue;
            render.DrawStringAtXY(pos2 + i * tge::Vector2i{2, 1}, L"  ", tge::Color::None,
                                  this->GetBackgroundColor());
        }
    }

    bool IsSet() { return set; }

    const Piece& GetPiece() const { return p; }

    void ApplyLineClear(const std::vector<int>& cleared) {
        Piece next;
        auto pos = this->GetPosition();
        for (const auto& cell : p) {
            int absY = pos.y + cell.y;
            if (std::find(cleared.begin(), cleared.end(), absY) != cleared.end()) continue;
            int shift = 0;
            for (int r : cleared)
                if (r > absY) shift++;
            next.push_back({cell.x, cell.y + shift});
        }
        p = std::move(next);
    }

private:
    bool set = false;

    Piece p;
    tge::Vector2i boardPos, vel, manual;

    tge::IntRect boardBounds;
    const std::vector<std::shared_ptr<Tetromino>>& others;
    const std::vector<int>& flashRows;
    const bool& flashHide;

    // Movement
    tge::Timer<std::chrono::milliseconds> moveDelay = 50;
    tge::Timer<std::chrono::milliseconds> arrowDelay = 0;

    // Rotations
    tge::KeyBuffer rotateLeftKey = tge::Key::Z;
    tge::KeyBuffer rotateRightKey = tge::Key::X;
};

bool isBoardInvalid(const tge::Vector2i& offset, const Piece& p, const tge::IntRect& board,
                    const std::vector<std::shared_ptr<Tetromino>>& others) {
    // Out of bounds
    if (pminx(offset, p) < board.x) return true;
    if (pmaxx(offset, p) > board.x + board.width - 2) return true;
    if (pmaxy(offset, p) > board.y + board.height - 1) return true;

    // Overlap with any already-set piece, in screen space
    for (const auto& cell : p) {
        auto pos = offset + cell * tge::Vector2i{2, 1};
        for (const auto& other : others) {
            if (!other) continue;
            auto otherOffset = other->GetPosition();
            for (const auto& otherCell : other->GetPiece()) {
                if (pos == otherOffset + otherCell * tge::Vector2i{2, 1}) return true;
            }
        }
    }
    return false;
}

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

        if (flashTicks > 0) {
            if (Await(&flashTimer)) {
                flashHide = !flashHide;
                if (--flashTicks == 0) {
                    auto current = GetShared<Tetromino>("current");
                    if (current) current->ApplyLineClear(flashRows);
                    for (auto& t : pieces) t->ApplyLineClear(flashRows);
                    flashRows.clear();
                    flashHide = false;
                    Next();
                }
            }
            return;
        }

        auto current = Get<Tetromino>("current");
        current->Update();
        if (current->IsSet()) {
            auto full = DetectFullRows();
            if (full.empty()) {
                Next();
            } else {
                flashRows = std::move(full);
                flashHide = false;
                flashTicks = 6;
                flashTimer.SetReadyNow();
            }
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

    std::vector<int> flashRows;
    bool flashHide = false;
    int flashTicks = 0;
    tge::Timer<std::chrono::milliseconds> flashTimer = 80;

    tge::KeyBuffer quitkey = tge::Key::Q;

private:
    Tetromino* Next() {
        if (auto old = GetShared<Tetromino>("current")) {
            pieces.push_back(std::move(old));
        }
        auto [p, c] = GetRandomPiece();
        return Component<Tetromino>("current")(p, c, Get("board")->GetBounds(), pieces, flashRows, flashHide);
    }

    std::vector<int> DetectFullRows() {
        auto bd = Get("board")->GetBounds();
        int required = bd.width / 2;
        std::map<int, int> count;
        auto add = [&](const std::shared_ptr<Tetromino>& t) {
            if (!t) return;
            auto pos = t->GetPosition();
            for (const auto& cell : t->GetPiece()) {
                int absY = pos.y + cell.y;
                if (absY >= bd.y && absY < bd.y + bd.height) count[absY]++;
            }
        };
        add(GetShared<Tetromino>("current"));
        for (const auto& t : pieces) add(t);
        std::vector<int> full;
        for (const auto& [y, c] : count)
            if (c >= required) full.push_back(y);
        return full;
    }
};

int main() {
    auto tetris = Tetris();
    tetris.Run();
}
