#pragma once
#include "Component.h"

class BounceComponent : public Component {
public:
    BounceComponent();
    
    void update() override;

private:
    void checkBounds();
};

