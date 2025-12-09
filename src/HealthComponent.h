#pragma once
#include "Component.h"

// HealthComponent manages player health
// Player starts with 3 health and dies after 3 bee collisions (each collision does 1 damage)
class HealthComponent : public Component {
public:
    HealthComponent(int maxHealth = 3); // Default: 3 health (player dies after 3 collisions)
    void update(float dt) override;
    
    int getHealth() const { return currentHealth; }
    int getMaxHealth() const { return maxHealth; }
    void takeDamage(int damage = 1);
    bool isDead() const { return currentHealth <= 0; }
    void reset() { 
        currentHealth = maxHealth; 
        invulnerabilityTime = 0.0f;
    }
    
private:
    int currentHealth;
    int maxHealth;
    float invulnerabilityTime = 0.0f; // Time player is invulnerable after taking damage
    const float INVULNERABILITY_DURATION = 1.0f; // 1 second of invulnerability
};

