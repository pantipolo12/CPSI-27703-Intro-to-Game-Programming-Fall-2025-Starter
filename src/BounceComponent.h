#pragma once
#include "Component.h"

class BounceComponent : public Component {
public:
    BounceComponent();
    
    void update(float dt) override;

private:
    void checkBounds();
};

