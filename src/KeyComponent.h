#pragma once
#include "Component.h"

class KeyComponent : public Component {
public:
    KeyComponent() = default;
    void update(float dt) override;
    bool isPickedUpByPlayer() const;
    
private:
    bool isPickedUp = false;
    Object* player = nullptr;
};

