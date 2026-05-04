#include "ComponentRenderManager.h"

namespace tge {
class Drawable {
public:
    virtual void Render() = 0;

protected:
    internal::components::ComponentRenderManager render;

private:
};
} // namespace tge
