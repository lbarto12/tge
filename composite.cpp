#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"

class ComponentA : public tge::BorderedRectangle {
public:
    ComponentA() : tge::BorderedRectangle() {
        this->SetSize({20, 10});
        this->SetBorderFromStyle(tge::BorderedRectangle::Style::RoundedLine);
        this->SetBorderForegroundColor(tge::Color::Blue);
        this->SetForegroundColor(tge::Color::Blue);
        this->SetFill(L'/');
    }

    void Render() override { tge::BorderedRectangle::Render(); }
};

class ComponentB : public tge::BorderedRectangle {
public:
    ComponentB() : tge::BorderedRectangle() {
        this->SetSize({20, 10});
        this->SetCenter({20, 10});
        this->SetForegroundColor(tge::Color::Red);
        this->SetFill(L'/');

        this->SetBorderFromStyle(tge::BorderedRectangle::Style::RoundedLine);
        this->SetBorderForegroundColor(tge::Color::Red);
    }

    void Init() override {
        auto inner = Component<tge::BorderedRectangle>("inner")();
        inner->SetSize({10, 6});
        inner->SetForegroundColor(tge::Color::Green);
        inner->SetFill(L'/');
        inner->SetBorderForegroundColor(tge::Color::Green);
        inner->SetBorderFromStyle(tge::BorderedRectangle::Style::RoundedLine);
        inner->SetCenter(this->GetCenter());
    }

    void Render() override {
        tge::BorderedRectangle::Render();
        Get("inner")->Render();
    }
};

class MyGame : public tge::GameManager {
public:
    MyGame() : tge::GameManager() {}

    void Start() override {
        Component<ComponentA>("A")();
        Component<ComponentB>("B")();
    }

    void Update() override {
        if (tge::Keyboard::GetKeyDown(tge::Key::Q)) {
            Quit();
        }
    }

    void Render() override {
        Get("A")->Render();
        Get("B")->Render();
    }
};

int main() {
    auto game = MyGame();
    game.Run();
}
