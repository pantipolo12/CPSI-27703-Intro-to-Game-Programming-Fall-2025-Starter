#pragma once
#include "Component.h"


class GroundComponent : public Component {
    public:
    GroundComponent() = default;
    void update(float dt) override {}
};
