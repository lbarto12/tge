
#include <iostream>

#define TGE_BASIC_CONSTRUCT(cname)                                                                                     \
    cname() : tge::ComponentBase() {}

namespace tge {
class ComponentBase {
public:
    ComponentBase() {}
    virtual ~ComponentBase() = default;

    virtual void Init() {}
    virtual void Update() {}
    virtual void Render() {}

private:
};
} // namespace tge
