#include "src/models/GameManager.h"
#include <iostream>

struct CustomComponent : public tge::ComponentBase {
    TGE_BASIC_CONSTRUCT(CustomComponent);

    void Init() override { std::cout << "custom init\n"; }
};

class Game : public tge::GameManager {
public:
    Game() : tge::GameManager() {}

    void Start() override {
        std::cout << "start\n";
        this->SetFPS(165);
        this->SetTicksPerSecond(1);

        Construct("test2")([]() { return new CustomComponent(); });

        Component("test");
    }
    void Update() override { Get("test2")->Update(); }
    void Render() override {}

private:
};

int main() {
    auto game = Game();
    game.Run();
}
