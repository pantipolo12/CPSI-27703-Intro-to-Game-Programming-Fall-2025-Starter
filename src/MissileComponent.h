#pragma once
#include "Component.h"

class MissileComponent : public Component {
public:
    MissileComponent(Object* target);
    
    // Getter
    Object* getTarget() const { return target; }
    
    // Setter
    void setTarget(Object* newTarget) { target = newTarget; }
    
    // Blank update for now
    void update() override;

private:
    Object* target;
};

