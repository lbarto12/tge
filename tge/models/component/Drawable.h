#include "ComponentRenderManager.h"

namespace tge {
class Drawable {
public:
    virtual void Render() = 0;

    Color SetBackgroundColor(Color c) { return this->background = c; }

    Color GetBackgroundColor() const { return this->background; }

    Color SetForegroundColor(Color c) { return this->foreground = c; }

    Color GetForegroundColor() const { return this->foreground; }

protected:
    internal::components::ComponentRenderManager render;

private:
    Color foreground = Color::White, background = Color::White;
};
} // namespace tge
