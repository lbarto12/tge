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

// Events
struct GameExitEvent : public tge::Event {};
struct PauseToggledEvent : public tge::Event {};
struct RestartEvent : public tge::Event {};
struct StartGameEvent : public tge::Event {
    int level = 0;
    StartGameEvent() = default;
    explicit StartGameEvent(int l) : level(l) {}
};

enum class GamePhase { Start, Playing, Paused, GameOver };

int LevelToDropMs(int lvl) {
    static const int frames[] = {48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2};
    int idx = std::clamp(lvl, 0, 19);
    return frames[idx] * 1000 / 60;
}

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
              const std::vector<int>& flashRows, const bool& flashHide, int dropMs)
        : tge::ComponentBase(), p(p), boardBounds(boardBounds), others(others),
          flashRows(flashRows), flashHide(flashHide), dropMs(dropMs) {
        this->SetBackgroundColor(c);
    }

    void Init() override {
        boardPos = {boardBounds.x + boardBounds.width / 2, boardBounds.y};
        this->SetPosition(boardPos);
        vel = {0, 1};
        manual = {0, 0};
        moveDelay.SetInterval(dropMs);
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
    int dropMs;

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

// Start Menu (level select)
class StartMenu : public tge::BorderedRectangle {
public:
    StartMenu() : tge::BorderedRectangle() {}

    void Init() override {
        this->SetSize({26, 13});
        this->SetCenter(tge::Terminal::Size() / 2);
        this->SetBorderFromStyle(tge::BorderedRectangle::Style::RoundedLine);
    }

    void Render() override {
        tge::BorderedRectangle::Render();
        auto cntr = this->GetCenter();

        std::wstring title = L" TETRIS ";
        std::wstring subtitle = L" select level ";
        std::wstring footer = L" enter to start ";

        render.DrawStringAtXY(cntr + tge::Vector2i{-int(title.length()) / 2, -5}, title, tge::Color::White,
                              tge::Color::BrightBlack);
        render.DrawStringAtXY(cntr + tge::Vector2i{-int(subtitle.length()) / 2, -3}, subtitle, tge::Color::White);

        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 5; col++) {
                int level = row * 5 + col;
                std::wstring num = std::to_wstring(level);
                std::wstring cell = (level < 10) ? L" " + num + L"  " : L" " + num + L" ";
                bool sel = (selected == level);
                auto pos = cntr + tge::Vector2i{-10 + col * 4, -1 + row};
                render.DrawStringAtXY(pos, cell, tge::Color::White,
                                      sel ? tge::Color::BrightBlack : tge::Color::None);
            }
        }

        render.DrawStringAtXY(cntr + tge::Vector2i{-int(footer.length()) / 2, 4}, footer, tge::Color::White);
    }

    void Update() override {
        if (Await(&upkey) && selected >= 5) selected -= 5;
        if (Await(&downkey) && selected <= 14) selected += 5;
        if (Await(&leftkey) && (selected % 5) != 0) selected -= 1;
        if (Await(&rightkey) && (selected % 5) != 4) selected += 1;

        if (tge::Keyboard::GetKeyDown(tge::Key::Enter)) {
            PushEvent(StartGameEvent(selected));
        }
    }

private:
    int selected = 0;
    tge::KeyBuffer upkey = tge::Key::Up;
    tge::KeyBuffer downkey = tge::Key::Down;
    tge::KeyBuffer leftkey = tge::Key::Left;
    tge::KeyBuffer rightkey = tge::Key::Right;
};

// Pause Menu
class PauseMenu : public tge::BorderedRectangle {
public:
    PauseMenu() : tge::BorderedRectangle() {}

    void Init() override {
        this->SetSize({20, 8});
        this->SetCenter(tge::Terminal::Size() / 2);
        this->SetBorderFromStyle(tge::BorderedRectangle::Style::RoundedLine);
    }

    void Render() override {
        tge::BorderedRectangle::Render();
        auto cntr = this->GetCenter();

        std::wstring title = L" PAUSED ";
        std::wstring resumeTxt = L" resume ";
        std::wstring quitTxt = L"  quit  ";

        render.DrawStringAtXY(cntr + tge::Vector2i{-int(title.length()) / 2, -2}, title, tge::Color::White,
                              tge::Color::BrightBlack);
        render.DrawStringAtXY(cntr + tge::Vector2i{-int(resumeTxt.length()) / 2, 0}, resumeTxt, tge::Color::White,
                              selected == 0 ? tge::Color::BrightBlack : tge::Color::None);
        render.DrawStringAtXY(cntr + tge::Vector2i{-int(quitTxt.length()) / 2, 1}, quitTxt, tge::Color::White,
                              selected == 1 ? tge::Color::BrightBlack : tge::Color::None);
    }

    void Update() override {
        selected += Await(&downkey) - Await(&upkey);
        selected = tge::Math::Clamp(0, 1, selected);

        if (tge::Keyboard::GetKeyDown(tge::Key::Enter)) {
            switch (selected) {
            case 0:
                PushEvent(PauseToggledEvent{});
                break;
            case 1:
                PushEvent(GameExitEvent{});
                break;
            }
        }
    }

private:
    int selected = 0;
    tge::KeyBuffer upkey = tge::Key::Up;
    tge::KeyBuffer downkey = tge::Key::Down;
};

// Game Over Menu
class GameOverMenu : public tge::BorderedRectangle {
public:
    GameOverMenu() : tge::BorderedRectangle() {}

    void Init() override {
        this->SetSize({20, 8});
        this->SetCenter(tge::Terminal::Size() / 2);
        this->SetBorderFromStyle(tge::BorderedRectangle::Style::RoundedLine);
    }

    void Render() override {
        tge::BorderedRectangle::Render();
        auto cntr = this->GetCenter();

        std::wstring title = L" GAME OVER ";
        std::wstring resumeTxt = L" resume ";
        std::wstring quitTxt = L"  quit  ";

        render.DrawStringAtXY(cntr + tge::Vector2i{-int(title.length()) / 2, -2}, title, tge::Color::White,
                              tge::Color::BrightBlack);
        render.DrawStringAtXY(cntr + tge::Vector2i{-int(resumeTxt.length()) / 2, 0}, resumeTxt, tge::Color::White,
                              selected == 0 ? tge::Color::BrightBlack : tge::Color::None);
        render.DrawStringAtXY(cntr + tge::Vector2i{-int(quitTxt.length()) / 2, 1}, quitTxt, tge::Color::White,
                              selected == 1 ? tge::Color::BrightBlack : tge::Color::None);
    }

    void Update() override {
        selected += Await(&downkey) - Await(&upkey);
        selected = tge::Math::Clamp(0, 1, selected);

        if (tge::Keyboard::GetKeyDown(tge::Key::Enter)) {
            switch (selected) {
            case 0:
                PushEvent(RestartEvent{});
                break;
            case 1:
                PushEvent(GameExitEvent{});
                break;
            }
        }
    }

private:
    int selected = 0;
    tge::KeyBuffer upkey = tge::Key::Up;
    tge::KeyBuffer downkey = tge::Key::Down;
};

// Game Manager
class Tetris : public tge::GameManager {
public:
    Tetris() : tge::GameManager() {
        this->SetFPS(165);
        this->SetTicksPerSecond(60);
    }

    void Start() override {
        auto bg = Component<tge::Rectangle>("bg")(tge::Terminal::Size());
        bg->SetBackgroundColor(tge::Color::Black);

        auto board = Component<Board>("board")();
        board->SetCenter(bg->GetCenter());

        Component<StartMenu>("start_menu")();
        Component<PauseMenu>("pause_menu")();
        Component<GameOverMenu>("game_over_menu")();

        phase = GamePhase::Start;
    }

    void Update() override {
        if (!GetEvents<GameExitEvent>().empty()) {
            Quit();
            return;
        }

        switch (phase) {
        case GamePhase::Start: {
            Get("start_menu")->Update();
            auto se = GetEvents<StartGameEvent>();
            if (!se.empty()) BeginGame(se[0]->level);
            break;
        }
        case GamePhase::Playing: {
            if (Await(&pausekey)) {
                phase = GamePhase::Paused;
                break;
            }
            UpdatePlaying();
            break;
        }
        case GamePhase::Paused: {
            if (Await(&pausekey) || !GetEvents<PauseToggledEvent>().empty()) {
                phase = GamePhase::Playing;
                break;
            }
            Get("pause_menu")->Update();
            break;
        }
        case GamePhase::GameOver: {
            Get("game_over_menu")->Update();
            if (!GetEvents<RestartEvent>().empty()) BeginGame(level);
            break;
        }
        }
    }

    void Render() override {
        Get("bg")->Render();
        Get("board")->Render();

        for (auto& i : pieces) i->Render();
        if (auto current = GetShared<Tetromino>("current")) current->Render();

        switch (phase) {
        case GamePhase::Start:
            Get("start_menu")->Render();
            break;
        case GamePhase::Paused:
            Get("pause_menu")->Render();
            break;
        case GamePhase::GameOver:
            Get("game_over_menu")->Render();
            break;
        default:
            break;
        }
    }

private:
    void BeginGame(int lvl) {
        level = lvl;
        if (GetShared<Tetromino>("current")) Destroy("current");
        pieces.clear();
        flashRows.clear();
        flashHide = false;
        flashTicks = 0;
        phase = GamePhase::Playing;
        SpawnNext();
    }

    void UpdatePlaying() {
        if (flashTicks > 0) {
            if (Await(&flashTimer)) {
                flashHide = !flashHide;
                if (--flashTicks == 0) {
                    auto current = GetShared<Tetromino>("current");
                    if (current) current->ApplyLineClear(flashRows);
                    for (auto& t : pieces) t->ApplyLineClear(flashRows);
                    flashRows.clear();
                    flashHide = false;
                    SpawnNext();
                }
            }
            return;
        }

        auto current = Get<Tetromino>("current");
        if (!current) return;
        current->Update();
        if (current->IsSet()) {
            auto full = DetectFullRows();
            if (full.empty()) {
                SpawnNext();
            } else {
                flashRows = std::move(full);
                flashHide = false;
                flashTicks = 6;
                flashTimer.SetReadyNow();
            }
        }
    }

    void SpawnNext() {
        if (auto old = GetShared<Tetromino>("current")) {
            pieces.push_back(std::move(old));
            Destroy("current");
        }
        auto [piece, color] = GetRandomPiece();
        auto bd = Get("board")->GetBounds();
        tge::Vector2i spawn = {bd.x + bd.width / 2, bd.y};
        if (isBoardInvalid(spawn, piece, bd, pieces)) {
            phase = GamePhase::GameOver;
            return;
        }
        Component<Tetromino>("current")(piece, color, bd, pieces, flashRows, flashHide, LevelToDropMs(level));
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

private:
    GamePhase phase = GamePhase::Start;
    int level = 0;

    std::vector<std::shared_ptr<Tetromino>> pieces;

    std::vector<int> flashRows;
    bool flashHide = false;
    int flashTicks = 0;
    tge::Timer<std::chrono::milliseconds> flashTimer = 80;

    tge::KeyBuffer pausekey = tge::Key::Escape;
};

int main() {
    auto tetris = Tetris();
    tetris.Run();
}
