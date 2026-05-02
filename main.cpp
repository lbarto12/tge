#include "src/models/GameManager.h"

struct CustomComponent : public tge::ComponentBase {
    TGE_BASIC_CONSTRUCT(CustomComponent);

    void Init() override {}
    void Update() override {}
};

class Game : public tge::GameManager {
public:
    Game() : tge::GameManager() {}

    void Start() override {
        this->SetFPS(165);
        this->SetTicksPerSecond(60);

        Construct("test")([]() { return new CustomComponent(); });
    }
    void Update() override { Get("test")->Update(); }
    void Render() override {}

private:
};

int main() {
    auto game = Game();
    game.Run();
}
