#pragma once
#include "Component.h"

class BackgroundComponent : public Component {
public:
    void update(float dt) override;
};